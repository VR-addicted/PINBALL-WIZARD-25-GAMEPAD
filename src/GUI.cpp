#include "GUI.h"
#include "GFX_ARRAYS.h"                               // jpg array
#include <math.h>
#include <cstring>                                    // Für strncpy  // evtl nach deepseek nicht mehr nötig
#include <String.h>                                   // nur wegen mac adressen auflösung drin. könnte man sicher anders lösen
#include <NimBLEDevice.h>

// #ifndef BLE_ADDR_PUBLIC
// #define BLE_ADDR_PUBLIC 0
// #endif
// #ifndef BLE_ADDR_RANDOM
// #define BLE_ADDR_RANDOM 1
// #endif

/*  kommandos um aus ui zu senden
ui->setGamepad(hid->gamepad);
ui->setKeyboard(hid->keyboard);
*/

// #define DEVICE_NAME         "PW25"

extern int8_t emulationMode; 
extern int8_t emulationModeOverride;                  // bluetooth manual override. 0 ignorieren, zahlen größer 0 ersetzen die orignal 
extern int dbglvl;                                    // Zugriff auf die globale Debug-Variable aus main.cpp
extern uint8_t dbglvlOSD; 
extern int PWM_POWER;                                 // display hintergrund beleuchtung
extern uint8_t sleepTimer;
extern uint8_t ledTimeOff;
extern unsigned long ledTimeOffMillis;
extern int tiltGain;
extern int tiltTresholdMenuX;
extern int tiltTresholdMenuY;
extern int tiltLimiter;
extern uint8_t stdMenu;                               // fallback menu, after X seconds, next variable
extern unsigned long stdMenuTimeMillis;
extern bool drawOnce;                                 //GLOBAL!
// extern unsigned long loopStartTime;                   // enthält millis() aus main loop nur für benchmark!
extern unsigned long milliTimeCopy;                   // diese copy für alles timing basierte nehmen!
extern uint8_t stdMenuTime;                           // fallback menu time
extern unsigned long sleepTimerMillis; 
extern unsigned long keyAbenchmarkTimeMark;
extern int8_t CheatLockRecordMode;
extern int skillShotMillisSend;
extern bool sendTimedPlungerButtonA;
extern unsigned long sendTimedPlungerButtonATimerReleaseFlag;

extern int tiltCounterGlob;
extern int dbgGamePad;
extern int flipFlopFlagSideLeft;
extern int THRESHOLD_X, THRESHOLD_Y, THRESHOLD_Z;     // Thresholds for tilt detection
extern bool secondKeyButtonFlag;
//extern float accX,accY;
extern float pitch, roll;
extern uint32_t ButtonFlipperLeftCounterToday;
extern uint32_t ButtonFlipperLeftCounterAlltime;
extern uint32_t ButtonFlipperRightCounterToday;
extern uint32_t ButtonFlipperRightCounterAlltime;
extern bool btPairingActive;
extern String macAdress;
extern uint8_t myMAC[6];

extern const char* DEVICE_NAME;
extern const char* DEVICE_MANUFACTURER;


extern bool isBleConnected();

extern void formatNVS();
extern void sendBTcommandActionKey(bool inputMode);
extern bool wasConnected;
extern unsigned long lastPacketTime;
extern int TIMEOUT_MS;

extern int PWM_POWER_TEMP;

extern volatile uint8_t espnowAirButtonCurrentState;
extern volatile uint8_t espnowAirButtonCurrentStateSend;

extern volatile uint16_t g_lastConnHandle;  // kommt aus main.cpp (siehe unten)

extern int8_t batteryESP32StatusLastround;  

//#define BUILD_NUMBER  wird über script.py definiert und ist dann hier als BUILD_VERSION oder BUILD_VERSION bekannt.

uint32_t _oldButtonFlipperLeftCounterToday    = 0 ;   //ButtonFlipperLeftCounterToday;    
uint32_t _oldButtonFlipperRightCounterToday   = 0 ;   //ButtonFlipperRightCounterToday;   
uint32_t _oldButtonFlipperLeftCounterAlltime  = 0 ;   //ButtonFlipperLeftCounterAlltime;   
uint32_t _oldButtonFlipperRightCounterAlltime = 0 ;   //ButtonFlipperRightCounterAlltime; 

struct HostDevice {
    const char* mac;
    const char* name;
};



MenuItem::MenuItem(const char* text, int min, int max, int initial, bool mod) {
    strncpy(label, text, sizeof(label) - 1);           // Maximal 14 Zeichen + 1 Nullterminator
    label[sizeof(label) - 1] = '\0';                   // Nullterminator sicherstellen

    minValue = min;
    maxValue = max;
    currentValue = initial;
    modMode = mod;
}



void MenuItem::setButtonY(int yStart) {
    btnMinus.yStart = btnPlus.yStart = yStart;
    btnMinus.yEnd = btnPlus.yEnd = yStart + 30;
}



void MenuItem::draw(TFT_eSPI& tft) {
    
    tft.fillSmoothRoundRect(2, btnMinus.yStart -2, 236, 34, 5, TFT_LIGHTGREY);

    tft.setTextFont(2); 
    tft.setTextSize(0);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(95, btnMinus.yStart + 7);
    tft.print(label);

    // // Print den aktuellen Wert
    tft.setFreeFont(&G7_Segment7_S510pt7b);
    tft.setTextSize(1);
        
    char buffer[4];  // 8 Zeichen + Nullterminator
    snprintf(buffer, sizeof(buffer), "%03" PRIu32, currentValue);   // Führende Nullen
    tft.setTextColor(TFT_RED);
    tft.drawString(buffer, 194, btnMinus.yStart + 7);  

    // Zeichne die Buttons
    tft.setTextFont(2);  // FIX: einfach 3 rechtecke zeichnen für [+][-] spart viele zeilen
    tft.setTextSize(2);  // schrift dicker, damit man die + und - zeichen besser sieht
    tft.fillSmoothRoundRect(10, btnMinus.yStart, 30, 30, 4, TFT_RED, TFT_LIGHTGREY);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(20, btnMinus.yStart - 2);
    tft.print("-");
    tft.fillSmoothRoundRect(50, btnMinus.yStart, 30, 30, 4, TFT_DARKGREEN, TFT_LIGHTGREY);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(60, btnPlus.yStart - 2);
    tft.print("+");
}



int MenuItem::checkTouch(int touchX, int touchY) {
    if (touchX >= 0 && touchX <= 49 && touchY >= btnMinus.yStart && touchY <= btnMinus.yEnd) {
        return -1;  // Minus-Button gedrückt
    }
    if (touchX >= 50 && touchX <= 100 && touchY >= btnPlus.yStart && touchY <= btnPlus.yEnd) {
        return 1;   // Plus-Button gedrückt
    }
    return 0;       // Kein Button gedrückt
}



int MenuItem::updateValue(int delta) { 
    currentValue += delta;   // (-)1,  0 = [no] button , (+)1 its the return from the 2 button request (-/+)
    if (currentValue < minValue) currentValue = minValue;
    if (currentValue > maxValue) currentValue = maxValue;
    return currentValue;     // currentValue return the absolut number in defined range
}



int MenuItem::getValue() const {         // getter
    return currentValue;
}



const char* MenuItem::getLabel() const {  //getter
    return label;
}



// end my MenuItem class //////////////////////////////////////////////////////////////////////////////////////////////////////////

extern uint8_t  UIinterval;
unsigned long UIintervalDBG    = 1000;             // von 0 bis 1000ms. (blaues mini debug fenster)

extern uint8_t UImenu ;                            // Startmenü-Index importieren
int touchX, touchY;                                // oberhalb der funktionen anlegen für gemeinsame nutzung.






GUI::GUI(TFT_eSPI& display, GamepadHID* gp, KeyboardHID* kb)
    : _tft(display), _gamepad(gp), _keyboard(kb)
{}




void GUI::setGamepad(GamepadHID* gp) {
    _gamepad = gp;
}

void GUI::setKeyboard(KeyboardHID* kb) {
    _keyboard = kb;
}




void GUI::begin() {
    initDisplay();                                 // Display initialisieren
  
}



void GUI::initDisplay() {
    _tft.begin();
    _tft.writecommand(0xB3);                       // Frame Rate Control
    _tft.writedata(0x08);                          // 
    _tft.writedata(0x0F);                          // 60Hz Frame Rate
    _tft.writedata(0x0F);                          // 60Hz Frame Rate

    _tft.setSwapBytes(true);                       // Fix für Farbfehler
    _tft.setRotation(0);                           // Rotation anpassen
    _tft.fillScreen(TFT_BLACK);                    // Bildschirm löschen

    // Create ball sprite
    spr.createSprite(102, 102);
    spr.fillSprite(bgColor);
    spr.fillCircle(51, 51, 50, TFT_WHITE);
}



void GUI::intro() {
    
    _tft.fillScreen(TFT_BLACK);
    uint16_t buffer[SCREEN_WIDTH];                 // Buffer für eine Zeile

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t rgb = pgm_read_byte(&fake_display[y * SCREEN_WIDTH + x]);
            uint16_t color = ((rgb & 0xE0) << 8) | // R von 3 auf 5 Bit
                             ((rgb & 0x1C) << 6) | // G von 3 auf 6 Bit
                             ((rgb & 0x03) << 3);  // B von 2 auf 5 Bit
            buffer[x] = color;
        }
        _tft.pushImage(0, y, SCREEN_WIDTH, 1, buffer);
    }

    for (int x = 0; x <= PWM_POWER; x++) {         // Logarithmischer Lichtanstieg besser für menschliches auge sichtbar
        int pwmValue = (int)(pow(2, (double)x / PWM_POWER * 8) - 1);
        if (pwmValue > PWM_POWER) pwmValue = PWM_POWER;
        ledcWrite(0, pwmValue);
        delay(10);
    }

    if(dbglvl == 0){                               // führe zeitintensives intro nur bei debuglevel 0 aus. spart zeit beim entwickeln
        delay(100);
        _tft.setCursor(70, 112);
        _tft.setTextColor(TFT_RED);
        _tft.setTextSize(4);
        _tft.print("2025");
        delay(1000);
        _tft.setCursor(50, 208);
        _tft.setTextColor(TFT_WHITE);
        _tft.setTextSize(2);
        _tft.print("VR-addicted");
        delay(2000);

        _tft.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, TFT_WHITE);

        int kugelRadius = 40;
        int kugelX = SCREEN_WIDTH / 2;
        int kugelY = 170;
        uint16_t graustufen[] = {
            bgColor, 0x3186, 0x4208, 0x528A,
            0x630C, 0x738E, 0x8410, 0x9492,
            0xA514, 0xB596, 0xC618, 0xD69A,
            0xE71C, 0xF79E, 0xFFDF, 0xFFFF
        };  // 0x2104
        int anzahlGraustufen = sizeof(graustufen) / sizeof(graustufen[0]);

        for (int i = 0; i < anzahlGraustufen; i++) {
            int radius = kugelRadius * (anzahlGraustufen - i) / anzahlGraustufen;
            int verschiebungX = map(i, 0, anzahlGraustufen - 1, -3, 3);
            int verschiebungY = map(i, 0, anzahlGraustufen - 1, -3, 3);
            _tft.fillCircle(kugelX + verschiebungX, kugelY + verschiebungY, radius, graustufen[i]);
        }
        delay(1000);

        int startRadius = 40;
        int maxRadius = max(SCREEN_WIDTH, SCREEN_HEIGHT);
        uint16_t kreisFarbe = graustufen[0];

        for (int radius = startRadius; radius <= maxRadius; radius += 2) {
            _tft.drawCircle(kugelX, kugelY, radius, kreisFarbe);
            delay(20);
        }

        startRadius = 40;
        maxRadius = max(SCREEN_WIDTH, SCREEN_HEIGHT);

        for (int radius = startRadius; radius <= maxRadius; radius += 2) {
            _tft.drawCircle(kugelX, kugelY, radius, kreisFarbe);
            _tft.drawCircle(kugelX+1, kugelY, radius, kreisFarbe);
            _tft.drawCircle(kugelX, kugelY+1, radius, kreisFarbe);
            _tft.drawCircle(kugelX+1, kugelY+1, radius, kreisFarbe);
            delay(2);
        }

        for (int radius = startRadius; radius > 0; radius--) {
            _tft.drawCircle(kugelX, kugelY, radius, kreisFarbe);
            _tft.drawCircle(kugelX+1, kugelY, radius, kreisFarbe);
            _tft.drawCircle(kugelX, kugelY+1, radius, kreisFarbe);
            _tft.drawCircle(kugelX+1, kugelY+1, radius, kreisFarbe);
            delay(40);
        }
    }
}


// ESP-Button for menu1 top middle position





// mode=0 checks touch field area and returns bool 1/0 if touched.
// mode=1 darkgrey base button, mode=2 green gedrückt, mode=3 lightgrey released, mode=4 red error connection
bool GUI::espnowButton(int8_t mode){
    if(mode == 0)      // 0 = check touch, gibt 0 oder 1 zurück
            {
                if(_lastTouchX > 98 && _lastTouchX < 142 && _lastTouchY > 1 && _lastTouchY < 51) return 1;  // bischen über die ränder der box, damit man sie leichter antippen kann
                else return 0;
            }
    if(UImenu == 1){  // zeige icon nur in menu 1
            if(mode == 1){     // 1 = base object menu 1 top center
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_DARKGREY);    // button oben links
                    _tft.setTextFont(2);         
                    _tft.setTextSize(2);
                    _tft.setTextColor(bgColor);    // 0x3186  // 0x4208  // todo: orange leuchten lassen
                    _tft.setCursor(113, 9);
                    _tft.print("X");    
                    return 0;
            }
            if(mode == 2){     //  2 = gedrückt
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_GREEN);       // button oben links
                    _tft.fillTriangle(110, 10, 130, 10, 120, 22, bgColor);                   // Schwarzes Dreieck nach oban nach unten
                    //_tft.fillTriangle(110, 45, 130, 45, 120, 36, bgColor);                 // Schwarzes Dreieck unten nach oben
                    return 0;
            }
            if(mode == 3){ // 3 = released, mit time out watchdog, der nach einer zeit die farbe erst auf mode2, dann auf mode 1 per timer stellt
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_LIGHTGREY);   // button oben links
                    //_tft.fillTriangle(110, 15, 130, 15, 120, 35, bgColor);                   // Schwarzes Dreieck nach oban nach unten
                    _tft.fillTriangle(110, 40, 130, 40, 120, 24, bgColor);                 // Schwarzes Dreieck unten nach oben
                    return 0;
            }
            if(mode == 4){ // 4 = esp connection loss
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_RED);         // button oben links
                    _tft.fillTriangle(110, 15, 130, 15, 120, 22, bgColor);                   // Schwarzes Dreieck nach oban nach unten
                    _tft.fillTriangle(110, 45, 130, 45, 120, 22, bgColor);                   // Schwarzes Dreieck unten nach oben
                    return 0;
            }
    }

    return 0;

    }









// mode=0 checks touch field area and returns 1 if touched. mode=1 draws the button
bool GUI::buttonBack(bool mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(10, 5, 40, 40, 5, TFT_WHITE, bgColor);    //button oben links
        _tft.fillTriangle(15, 25, 30, 14, 30, 35, bgColor);                // Schwarzes Dreieck nach links
        _tft.fillRect(30, 20, 10, 10, bgColor);                            // Kleines schwarzes Rechteck daneben
        return 0;
    }
    else{
         if(_lastTouchX > 5 && _lastTouchX < 55 && _lastTouchY > 1 && _lastTouchY < 51) return 1;
         else return 0;
    }
}



// Save Button mitte rot
bool GUI::saveButton(bool mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(210, 5, 40, 40, 5, TFT_WHITE, bgColor);    // button oben links
        _tft.fillTriangle(215, 25, 30, 15, 30, 35, bgColor);                // Schwarzes Dreieck nach links
        _tft.fillRect(230, 20, 10, 10, bgColor);                            // Kleines schwarzes Rechteck daneben
        return 0;
    }
    else{
         if(_lastTouchX > 205 && _lastTouchX < 241 && _lastTouchY > 1 && _lastTouchY < 51) return 1;
         else return 0;
    }
}



// [C] button für cheat mode oben rechts.
bool GUI::cheatButton(int8_t mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(190, 5, 40, 40, 5, TFT_WHITE, bgColor);     // button oben links
        _tft.setTextFont(2);         
        _tft.setTextSize(2);
        _tft.setTextColor(bgColor);    // 0x3186  // 0x4208  // todo: orange leuchten lassen
        _tft.setCursor(202, 9);
        _tft.print("C");        
         
        return 0;
    }
    else{
         if(_lastTouchX > 190 && _lastTouchX < 241 && _lastTouchY > 1 && _lastTouchY < 51) return 1;

            else return 0;
    }
}

// 0 bzw leer() gibt true false touch zone zurück, wenn touch in zone
// 1 zeichnet button mit text, 2 grüner rand, 3 grauer rand (clean the green, geht schneller als kompletten button neu zu rendern)
bool GUI::drawSkillShotButton(int8_t mode){

    if(mode == 0){  // 0 checkt touch coordinaten innerhalb button und dann direkt wieder raus
            if(_lastTouchX > 0 && _lastTouchX < 241 && _lastTouchY > 199 && _lastTouchY < 321) return 1;
            else return 0;
        }
    else if(mode == 1){
                    _tft.fillRoundRect(4 , 200 , 232 , 116, 10, TFT_WHITE);    // SKILL SHOT BUTTON
                    _tft.setTextFont(1);
                    _tft.setTextSize(3);
                    _tft.setTextColor(bgColor); 
                    _tft.setCursor(32, 210);     
                    _tft.print("SKILL SHOT");  
                    return 0;
        }
    else if(mode == 2 && UImenu == 4){  // && verhindert das das skillshot aktivitäts rechteck (rot/bgcolor) in anderen menus dargestellt wird.
                    _tft.drawRect(26,205,190,36, TFT_RED);  // Zeige grünen rahmen während der xxx ms sendezeit
                    _tft.drawRect(27,206,188,34, TFT_RED);
                    return 0;
                }
    else if(mode == 3 && UImenu == 4){  // && verhindert das das skillshot aktivitäts rechteck (rot/bgcolor) in anderen menus dargestellt wird.
                    _tft.drawRect(26,205,190,36, TFT_WHITE); // lösche den grünen rahmen wieder
                    _tft.drawRect(27,206,188,34, TFT_WHITE);
                    return 0;
    }
    else return 0;
















}








void GUI::topMenu() {  // 1 bis 3 weiße buttons oben  // TODO: TODO: löschen

            _tft.fillRect(0, 0, 240, 46, bgColor);                            // fill top background 
            _tft.fillSmoothRoundRect(10, 5, 40, 40, 5, TFT_WHITE, bgColor);   // button oben links
            _tft.fillTriangle(15, 25, 30, 15, 30, 35, bgColor);               // Schwarzes Dreieck nach links
            _tft.fillRect(30, 20, 10, 10, bgColor);                           // Kleines schwarzes Rechteck daneben
            _tft.fillRect(190, 5, 40, 40, TFT_WHITE);
}



// Hauptmenü mit 7 Balken
void GUI::menu0() {   

    if(drawOnce){
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);
        _tft.setRotation(0);
        int cellWidth = 220;
        int cellHeight = 42;
        int startX = 10;
        int startY = 54;
        _tft.setTextSize(1);
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
                
        for (int i = 0; i < numEntries; i++) {
            _tft.drawRect(startX, startY + i * cellHeight, cellWidth, cellHeight, TFT_WHITE);
            int textX = startX + 10;
            int textY = startY + i * cellHeight + (cellHeight / 2) + 14; 
            _tft.setCursor(textX, textY);
            _tft.setTextColor(0x2000);                     // 0x3186  // 0x4208  // 
            _tft.print(88888888);                          // geiler effekt
            _tft.setCursor(textX, textY);
            _tft.setTextColor(0xFC00);                     // TFT_WHITE
            _tft.print(texts[i]);

            Ycells[i] = startY + i * cellHeight ;          // trennlinien für toucherkennung speichern
            Ycells[i+1] = startY + ((i+1) * cellHeight) ;  // berechne ende der letzten zelle
        }
       
    drawOnce = 0;    
    }
    
// touch erkennung und reaktionen
    if (_touchDetected) {        // menu abhängiges reagieren auf touches . wenn zentrales processTouch() true zurück gab, liegen die coordinaten in touchX, touchY
        _touchDetected=0 ;       // zurücksetzen. der nächste touch kommt von alleine

    if(dbglvl>1) _tft.fillCircle(_lastTouchX, _lastTouchY , 4, TFT_RED); // roter touch diagnose punkt auf display 
    
    for (int i = 0; i < numEntries; i++) { 
        if (_lastTouchY  > Ycells[i] && _lastTouchY  < Ycells[i+1]) {
            
            UImenu = i + 1;      // hier wird global das menu geändert und beim nächsten durchlaufkommt das neue menu
            UIclearScreen = 1;   
            drawOnce = 1;
// delay(200);    
            break;
            }
    }
    }
}


extern int8_t batteryESP32Status;
// "Play" – Flipperball mit Button-Anzeige und Klick-Counter
void GUI::menu1() {    
    
    static uint8_t batteryESP32StatusOld = 0;
    if(drawOnce){  // reihenfolge ist wichtig!!!!!!!!! dieses flag darf erst zum schluss auf 0 gesetzt werden!!
        
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);
       
        if(flagUImenu1SpriteDown){
            for (int spriteY = -80; spriteY < 153; spriteY += 4) {
                //_tft.fillCircle(120, spriteY, 55, bgColor);
                //_tft.fillCircle(120, spriteY, 50, TFT_WHITE);
                _tft.fillRect(70, spriteY -20, 102, 20, bgColor);
                spr.pushSprite(70, spriteY);   // Sprite auf Display zeichnen
                delay(20);
            }
            flagUImenu1SpriteDown = false;
            spr.deleteSprite();                // Speicher freigeben, falls nicht dauerhaft benötigt
        }
        else fillSpriteBackground();           // zeichnet den kreis, wenn ins menu rein gegangen wird und keine animation angezeigt wird.

        buttonBack(1);                         // draw back button
        cheatButton(1);                        // cheatmode button
        espnowButton(1);                       // espnow button
        drawPhysicalVirtualKeys(8,1);          // draw white flipper once
        drawPhysicalVirtualKeys(7,1);          // draw white flipper once
        drawPhysicalVirtualKeys(2,0);          // draw gray background once
        drawPhysicalVirtualKeys(1,0);          // draw gray background once
        
        draw7SegmentNumberSmall(ButtonFlipperLeftCounterAlltime, 10, 98, 1);    // refresh loop
        draw7SegmentNumberSmall(ButtonFlipperLeftCounterToday, 10, 121, 1);     // refresh loop  
        draw7SegmentNumberSmall(ButtonFlipperRightCounterAlltime, 130, 98, 1);  // refresh loop
        draw7SegmentNumberSmall(ButtonFlipperRightCounterToday, 130, 121, 1);   // refresh loop
    
        // interaktive bildschirmelemente ausgeben
        draw7SegmentNumberBig();               // Zeige Summe Alltime Left+Right. einmalig und dann automatisch bei änderung von a+b
        drawBatteryLocal(2);                   // init empty
        drawBatteryPedal(2);                   // init empty 

        if(dbglvl > 9){                        // show all display elements for debug purpose
                drawPhysicalVirtualKeys(6,1);  // draw all buttons
                drawPhysicalVirtualKeys(5,1);  // draw all buttons
                drawPhysicalVirtualKeys(4,1);  // draw all buttons
                drawPhysicalVirtualKeys(3,1);  // draw all buttons
                
                drawVirtualTiltingJoystickKeys(1,1);
                drawVirtualTiltingJoystickKeys(2,1);
                drawVirtualTiltingJoystickKeys(3,1);
                drawVirtualTiltingJoystickKeys(4,1);
       }
       drawOnce = 0;
    }  // ------------------------
    








    // static uint32_t uiButtonESPnowResetviewTimeFlag      = 0;                // loop session übergreifend deklarieren
    // if(espnowAirButtonCurrentState != espnowAirButtonCurrentStateSend)
    // {
    //     espnowAirButtonCurrentStateSend =  espnowAirButtonCurrentState;
    //     if(espnowAirButtonCurrentStateSend == 0) { 
    //             sendBTcommandActionKey(0);
    //             espnowButton(3); 
    //             uiButtonESPnowResetviewTimeFlag = milliTimeCopy + 1000;      // set flag in future time mark  
    //         }
    //     if(espnowAirButtonCurrentStateSend == 1) { // send bt command and update ui
    //             sendBTcommandActionKey(1);
    //             espnowButton(2);
    //             uiButtonESPnowResetviewTimeFlag=0;
    //         } 
    // }
  
    // if(uiButtonESPnowResetviewTimeFlag){  // muss größer als 0 sein
    //   if(uiButtonESPnowResetviewTimeFlag < milliTimeCopy){  // wenn es jetzt wieder kleiner als die future mark ist, aktion und flag cleanen
    //         espnowButton(1);                       // update ui
    //         uiButtonESPnowResetviewTimeFlag = 0;   // clean flag
    //     }
    // }















    
    bool refresh7SegmentBig = 0;
    if(_oldButtonFlipperLeftCounterAlltime < ButtonFlipperLeftCounterAlltime)
        {        // erneuere die zahlen linke hälfte vom display
                draw7SegmentNumberSmall(ButtonFlipperLeftCounterAlltime, 10, 98, 1);  
                draw7SegmentNumberSmall(ButtonFlipperLeftCounterToday, 10, 121, 1);  
                _oldButtonFlipperLeftCounterAlltime = ButtonFlipperLeftCounterAlltime;
                refresh7SegmentBig = 1;   // draw die große 7 segment anzeige 1x, auch wenn beide werte sich ändern
            }
   
    if(_oldButtonFlipperRightCounterAlltime  < ButtonFlipperRightCounterAlltime)
        {       // erneuere die zahlen rechte hälfte vom display
                draw7SegmentNumberSmall(ButtonFlipperRightCounterAlltime, 130, 98, 1);// 0 = refresh call, zeige Flipper R ingesamt
                draw7SegmentNumberSmall(ButtonFlipperRightCounterToday, 130, 121, 1); // 0 = refresh call, zeige Flipper R today
                _oldButtonFlipperRightCounterAlltime  = ButtonFlipperRightCounterAlltime;
                 refresh7SegmentBig = 1;  // draw die große 7 segment anzeige 1x, auch wenn beide werte sich ändern
            }
    
    if(refresh7SegmentBig) draw7SegmentNumberBig(); // zeige die große 7 segment anzeige 1x, auch wenn beide werte sich ändern. spart taktzyklen
       
    
    if (_touchDetected) {                 // touch auswerten // wenn processTouch() true zurück gibt, liegen die coordinaten in touchX, touchY
        _touchDetected=0 ;                // zurücksetzen. der nächste touch kommt von alleine                    
                if(buttonBack()){         // check back button
                    UImenu = 0;           // setze neues menu  
                    UIclearScreen = 1;
                    flagUImenu0DrawRowCells = true;
                    flagUImenu1SpriteDown   = false; 
                }
    
                if(cheatButton()){        // check cheatButton area
                    UImenu = 4;           // setze neues menu  
                    UIclearScreen = 1;
                    drawOnce = 1;
                }
                else{
                    }
               // _touchDetected=0 ;         // zurücksetzen. der nächste touch kommt von alleine
                }
                else drawOnce = 0;         // fix

  
    if(secondKeyButtonFlag){
        drawWaterBubble();  // wenn B lange gedrückt ist
        }
    else{
        drawPeakMeterNudgeDirection();  // wenn b angetippt wird.
    }
    
    drawButtonTilt(tiltCounterGlob);

    if(batteryESP32Status != batteryESP32StatusOld) {
        batteryESP32StatusOld = batteryESP32Status;
        drawBatteryLocal(batteryESP32Status);
    }



}



//  direction 1 up,2 down,3 left,4 right, (0 walk throuhg not required)
//  draw indicator on screen. mode 0 set bgColor, 1 set White
void GUI::drawVirtualTiltingJoystickKeys(int8_t direction, int8_t mode){

         uint32_t drawColor = bgColor;                                  // default color
    if(mode == 1) drawColor = TFT_WHITE;                                // set white color
    switch (direction) {                                                // switch case für die 4 richtungen
        case 1: _tft.fillRect ( 116,  143,  8, 5, drawColor); break;    // UP
        case 2: _tft.fillRect ( 116,  260,  8, 5, drawColor); break;    // DOWN
        case 3: _tft.fillRect (  60,  198,  5, 8, drawColor); break;    // LEFT
        case 4: _tft.fillRect ( 177,  198,  5, 8, drawColor); break;    // RIGHT
    default: break;
    }
}



// TODO: eine box mit pfeilen und vorher jeweils den kleinen background löschen
// 3 directions, 1 up, 2 left, 3 right. mode set color/arrow direction
void GUI::drawTiltDirection(int8_t direction, int8_t mode){

         uint32_t drawColor = bgColor;                                  // default color
    if(mode == 1) drawColor = TFT_RED;                                  // set white color
    switch (direction) {                                                // switch case für die 4 richtungen
        case 1: _tft.fillRect ( 116,  142,  8, 5, drawColor); break;    // UP
        case 2: _tft.fillRect (  54,  196,  8, 5, drawColor); break;    // LEFT
        case 3: _tft.fillRect ( 184,  192,  5, 8, drawColor); break;    // RIGHT
    default: break;
    }
}



// (1) FrontL, (2) FrontR, (3) V, (4) V, (5) V, (6) V, (7) FlipperL, (8) FlipperR
// key = taste, mode 0,1,2 --> 0 -inaktiv, 1 - aktiv (andere farbe), 2 - spezial mode noch eine andere wunschfarbe
void GUI::drawPhysicalVirtualKeys(int8_t key, int8_t mode){

    uint32_t drawColor = bgColor;                                       // default color

    if(mode == 1){  // 1 - Active

        if(key == 1 || key == 2)  drawColor = TFT_YELLOW;               // change color
        if(key == 3 || key == 4)  drawColor = TFT_DARKCYAN;             // change color
        if(key == 5 || key == 6)  drawColor = TFT_GREEN;                // change color
        if(key == 7 || key == 8)  drawColor = TFT_WHITE;                // change color
    }
    else if(mode == 2){ // 2 - special  // wird derzeit eigentlich nur für key 1 um skill shot in rot anzuzeigen. könnte man auch für long press B nutzen. wär nicht schlecht
        if(key == 1 || key == 2)  drawColor = TFT_RED;                  // change color  skill shot visualisierung in UImenu1
        if(key == 3 || key == 4)  drawColor = TFT_DARKCYAN;             // change color
        if(key == 5 || key == 6)  drawColor = TFT_GREEN;                // change color
        if(key == 7 || key == 8)  drawColor = TFT_WHITE;                // change color
       }
    
    else{           // 0 - oder jede zahl Y2 Inactive darstellung
        if(key == 1 || key == 2)  drawColor = TFT_DARKGREY;             // change color
        //if(key == 3 || key == 4)  drawColor = bgColor;                // change color // könnte man ausklammern wegen default bgColor
        //if(key == 5 || key == 6)  drawColor = bgColor;                // change color // könnte man ausklammern wegen default bgColor
        if(key == 7 || key == 8)  drawColor = TFT_RED;                  // change color
    }

switch (key) {                                                          // switch case für die 4 richtungen
        case 1: _tft.fillRect ( 225,  310,  10, 10, drawColor); break;  // [1] Physical Front Right --> A (or any other key)
        case 2: _tft.fillRect (   5,  310,  10, 10, drawColor); break;  // [2] Physical Front LEFT  --> B (or any other key)

        case 3: _tft.fillRect (   5,  298,  10, 10, drawColor); break;  // [3] Virtual Key Display Seite Links
        case 4: _tft.fillRect ( 225,  298,  10, 10, drawColor); break;  // [4] Virtual Key Display Seite Rechts

        case 5: _tft.fillRect ( 213,  310,  10, 10, drawColor); break;  // [5] Virtual Key Display Front Rechts
        case 6: _tft.fillRect (  17,  310,  10, 10, drawColor); break;  // [6] Virtual Key Display Front Links

        case 7: _tft.fillRect (   5,  240,  20, 10, drawColor); break;  // [7] Physical Flipper LEFT
        case 8: _tft.fillRect ( 215,  240,  20, 10, drawColor); break;  // [8] Physical Flipper RIGHT
        default: break;
        }                                                               // switch case für die 4 richtungen
}



volatile uint32_t btLastSendTime = 0;  // Zeitpunkt des letzten gesendeten Pakets
volatile uint32_t btLatency = 0;       // Gemessene Latenz in ms
volatile uint32_t btPacketsSent = 0;   // Anzahl gesendeter Pakete
volatile uint32_t btPacketsLost = 0;   // Geschätzte verlorene Pakete


// Parset "AA:BB:CC:DD:EE:FF" (oder ohne Doppelpunkte) zu 6 Bytes.
// Gibt true zurück, wenn exakt 12 Hex-Zeichen gefunden wurden.
static bool macStringToBytes(const String& s, uint8_t out[6]) {
    auto hexVal = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        c = (char)tolower((unsigned char)c);
        if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
        return -1;
    };
    int nibbles = 0;
    uint8_t tmp[6] = {0};
    for (size_t i = 0; i < s.length(); ++i) {
        int v = hexVal(s[i]);
        if (v < 0) continue;                 // ignoriert ":" etc.
        if (nibbles >= 12) return false;     // zu lang
        if ((nibbles & 1) == 0) tmp[nibbles >> 1] = (uint8_t)(v << 4);
        else                    tmp[nibbles >> 1] |= (uint8_t)v;
        ++nibbles;
    }
    if (nibbles != 12) return false;         // exakt 6 Bytes
    memcpy(out, tmp, 6);
    return true;
}

//import functions from main()
extern void verifyBLEServices();                         // ein paar von den funktionen sind doppelt und können raus
extern void updateBLEStatus();    
extern void debugBLEStatus(const char* tag = "BLE");     // ein paar von den funktionen sind doppelt und können raus
// Bluetooth Einstellungen 
void GUI::menu2() { 
static String macOld = "00:01:02:03:04:05";              // 1st init irgend eine dummy MAC-Adresse damit eine differenz zur gemessenen erezugt wird
static String deviceNameOld = "scan ...";                // 1st init dummy Name des verbundenen Geräts
   

    if(drawOnce){
                _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);        
                _tft.pushImage(95, 50, 50, 73, BTlogo_50x73x16);          // show bt gfx
                flagUImenu0DrawRowCells = true;
                // flagUItopButtonsDraw = true;                           // blende oben 1 button und den top bar ein.
                buttonBack(1);
                _tft.setTextSize(1);
                _tft.setTextFont(2);
                _tft.setTextColor(TFT_BLACK, TFT_RED);
                _tft.fillSmoothRoundRect(190, 5, 40, 40, 5, TFT_RED, bgColor);  // oben rechts roter lösch button
                _tft.setCursor(199, 8);
                _tft.print("DEL");
                _tft.setCursor(199, 25);
                _tft.print("ALL");
                _tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_YELLOW, bgColor);  // oben mitte oranger lösch button, nur aktive verbindgung löschen
                _tft.setCursor(109, 8);
                _tft.print("DEL");
                _tft.setCursor(106, 25);
                _tft.print("THIS");
                
                _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                drawBTprofile(1);                                         // draw BT profile settings row
                drawOnce = 0;
                deviceNameOld = "refresh";                                // fix. setze irgend einen fake namen, damit er beim nächsten mal wieder refresht wird.
                updateBLEStatus();
                verifyBLEServices();
            }
    _tft.setTextFont(2);

    // >>>>>>>>>>>> Time Trap 1000ms >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    static uint32_t timeTrap = 0;                                          // einmalig variable initialisieren.
    if(timeTrap < milliTimeCopy){           // millis()                    // benchmark timer, alle 1000ms 10 tasten senden
       timeTrap = milliTimeCopy + 1000;     // millis()
        updateBLEStatus();
        _tft.setTextColor(TFT_WHITE, bgColor);
        _tft.setTextSize(1);
        _tft.setCursor(10, 300);
        if(dbglvl) debugBLEStatus();        // ein paar von den funktionen sind doppelt und können raus
        //if(dbglvl) verifyBLEServices();     // ein paar von den funktionen sind doppelt und können raus
        updateBLEStatus();                  // polling and refresh variables  
        if (isBleConnected()) {

                String macAddress = macAdress;
                String deviceName = getDeviceName(macAddress);

                if (macAddress != macOld || deviceName != deviceNameOld) {
                    _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                    _tft.setCursor(83, 144);
                    _tft.setTextColor(TFT_GREEN, TFT_BLACK);
                    _tft.setTextSize(1);
                    _tft.print("CONNECTED");

                    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
                    _tft.setCursor(15, 159);
                    _tft.setTextSize(2);
                    _tft.setTextDatum(MC_DATUM);
                    _tft.drawString(deviceName, 120, 176);
                    _tft.setTextDatum(TL_DATUM);
                    _tft.setCursor(15, 188);
                    _tft.setTextColor(TFT_DARKGREY);
                    _tft.print(macAddress);

                    macOld = macAddress;
                    deviceNameOld = deviceName;
                    drawBTprofile(1);
                }
            } else {
                _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                _tft.setCursor(20, 162);
                _tft.setTextColor(TFT_RED, TFT_BLACK);
                _tft.setTextSize(2);
                _tft.println("NO CONNECTION");
                deviceNameOld = "scan .";
            }

    }  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

if (_touchDetected) {   // wenn processTouch() true zurück gibt, liegen die coordinaten in touchX, touchY
    _touchDetected=0 ;  // zurücksetzen. der nächste touch kommt von alleine
        
        // Button oben rechts ROT dele pairings
        if(_lastTouchX > 190 && _lastTouchX < 240 && _lastTouchY > 5 && _lastTouchY < 50){
                    _tft.fillRect(0, 140, 240, 80, TFT_BLACK);      // Statusbereich leeren
                    _tft.setCursor(10, 170);
                    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                    _tft.setTextSize(2);
                    _tft.println("del all bonds");
                    clearBluetoothPairings();
                    // nottrick um nvs sicher zu formatieren, ohne großen aufwand.
                    formatNVS();
                    delay(1000);
                    _tft.setCursor(10, 150);
                    _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                    _tft.println("all bonds\n deleted");
                    delay(1000);
                    drawOnce = true;  // force redraw FUNZT NET!
        }

        // Button top center orange: nur aktives Pairing löschen
        if(_lastTouchX > 100 && _lastTouchX < 150 && _lastTouchY > 5 && _lastTouchY < 50){
            _tft.fillRect(0, 160, 240, 80, TFT_BLACK);
            _tft.setCursor(10, 170);
            _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            _tft.setTextSize(2);
            _tft.println("del active");

            if (_gamepad) {
                clearActiveBluetoothPairing();
                delay(500);
                _tft.println("deleted one");
                delay(1500);
            }

            _tft.fillRect(0, 160, 240, 80, TFT_BLACK);
            _tft.setCursor(10, 170);

            drawOnce = true;
        }

        if(_lastTouchX > 95 && _lastTouchX < 145 && _lastTouchY > 50 && _lastTouchY < 130){  // check bluetooth icon to pair
                
                if (_gamepad) {                                   // Sicherstellen, dass das Objekt existiert
                    _tft.fillRect(0, 160, 240, 60, TFT_BLACK);       // Statusbereich leeren
            
                    _tft.setCursor(10, 170);
                    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                    _tft.setTextSize(2);
                    _tft.println("Starte Pairing...");
                
                    startFreshPairing(); 
                    delay(100);                                      // Kurze Wartezeit für Debug-Zwecke
            
                    _tft.setCursor(10, 190);
                    if (isBleConnected()) {
                        _tft.setTextColor(TFT_GREEN, TFT_BLACK);
                        _tft.println("Erfolgreich verbunden!");
                    } else {
                        _tft.setTextColor(TFT_RED, TFT_BLACK);
                        _tft.println("Kopplung fehlgeschlagen.");
                    }
                }
        }  
      
        // abfrage ob touch auf menüb/back utton für home menu
        if(buttonBack())
            {
            UImenu = 0;                                              // setze neues menu  
            UIclearScreen = 1;
            flagUImenu0DrawRowCells = true;
            flagUImenu2drawBTsymbol = true;                          
        } 
        
        static uint32_t timeTrapBTmenu = 0;                          // einmalig in die funktion laden
        
        if(timeTrapBTmenu < milliTimeCopy){     // millis()
        timeTrapBTmenu = milliTimeCopy + 200;   // millis()          // alle 1000ms einmalig die touch abfrage machen
                if(drawBTprofile(2) != 0) drawBTprofile(1);          // (2) check touch fields, -1,0,+1 kommt zurück, und mit (1) stelle gegebenfalls dar 
        }
    }
}




// Menu Settings
void GUI::menu3() {  
    
    static MenuItem sleepTimerItem ("SLEEP TIMER",   5, 3000, sleepTimer);
    static MenuItem brightnessItem ("BRIGHTNESS",    6,  255, PWM_POWER);    // lade aktuellen stand einmalig ins objekt
    static MenuItem ledTimeOffItem ("LED TIMER",     1,  120, ledTimeOff);
    static MenuItem tiltGainItem   ("NUDGE GAIN",    1,   50, tiltGain);
    static MenuItem stdMenuItem    ("STD MENU",      0,    5, stdMenu);
    static MenuItem stdMenuTimeItem("MENU TIMEOUT", 10,  300, stdMenuTime);
    
    if(drawOnce){ // Zeichne die MenuItems (once, einmalig, danach nur noch zeilenweise update, je nachdem welche geändert wird)
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);    
     
        buttonBack(1);  // zeichne einen button oben links    
            // Setze die Y-Positionen
        sleepTimerItem.setButtonY(60);   // set Y positions for every menu row
        brightnessItem.setButtonY(100);  
        ledTimeOffItem.setButtonY(140);
        tiltGainItem.setButtonY(180);
        stdMenuItem.setButtonY(220);  
        stdMenuTimeItem.setButtonY(260);

        sleepTimerItem.draw(_tft);        // zeichne die menu items
        brightnessItem.draw(_tft);
        ledTimeOffItem.draw(_tft);
        tiltGainItem.draw(_tft);
        stdMenuItem.draw(_tft);
        stdMenuTimeItem.draw(_tft);
        drawOnce = 0;   // setze flag zurück
    }
   

    // Touch-Eingabe verarbeiten
    // TODO: miese lösung. das muss wie in den anderen menus entprellt werden.
    static int8_t countTrap = 0;  // einmalig in die funktion laden
    countTrap++; // hochzählen und in der if abfrage nur jede 3 bis 8te runde rein spart recourcen und entprellt touch zähler
    if (_touchDetected && countTrap > 1) {  //  // menu abhängiges reagieren auf touches . wenn zentrales processTouch() true zurück gab, liegen die coordinaten in touchX, touchY
        _touchDetected=0 ;  // zurücksetzen. 
        countTrap     =0 ;  
    
        int deltaBright      = brightnessItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaSleep       = sleepTimerItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaLedTime     = ledTimeOffItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaTiltGain    = tiltGainItem   .checkTouch(_lastTouchX, _lastTouchY);
        int deltastdMenu     = stdMenuItem    .checkTouch(_lastTouchX, _lastTouchY);
        int deltastdMenuTime = stdMenuTimeItem.checkTouch(_lastTouchX, _lastTouchY);

        // Aktualisiere die Werte der globlen/lokalen variablen 
        if (deltaSleep != 0) {
            sleepTimer = sleepTimerItem.updateValue(deltaSleep);  
            sleepTimerItem .draw(_tft);   // update UI once
        }
        if (deltaBright != 0) {
            PWM_POWER = brightnessItem.updateValue(deltaBright);  
            ledcWrite(0, PWM_POWER);
            brightnessItem .draw(_tft);
        }
      
        if (deltaLedTime != 0) {
            ledTimeOff = ledTimeOffItem.updateValue(deltaLedTime); 
            ledTimeOffItem .draw(_tft);
            
        }
  
        if(deltaTiltGain !=0){
            tiltGain = tiltGainItem.updateValue(deltaTiltGain);  
            tiltGainItem   .draw(_tft);
        }

        if(deltastdMenu !=0){
            stdMenu = stdMenuItem.updateValue(deltastdMenu);  
            stdMenuItem    .draw(_tft);
        }

        if(deltastdMenuTime !=0){
            stdMenuTime = stdMenuTimeItem.updateValue(deltastdMenuTime);  
            stdMenuTimeItem.draw(_tft);
        }
        
        // check home/back button
        if(buttonBack()){
            UImenu = 0;                           // setze neues menu  
            UIclearScreen = 1;
            flagUImenu0DrawRowCells = true;
            flagUImenu2drawBTsymbol = true;       // für zukunft wieder scharf schalten
        } 
    }

}



// Cheat MENU
void GUI::menu4() { 
    static int tempskillShotMillisSend = skillShotMillisSend;  // einmalig "ist" zustand übernehmen.
    if(drawOnce){  // check flag
       drawOnce = 0;  // clear flag
                _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor); 
                buttonBack(1);                    // zeige back button 
                drawButtonCheatLockRecord(1);     // 1=N button= normal mode, 2=C button cheat mode aktiv
                buttonCheatBackToPlay(1);
                drawButtonCheatStoreProfile(1);
                drawCheatProfileSelector();
                drawTextCheatProfileName(69);         
                drawSkillShotButton(1);
                draw7SegmentMillisecondBig(skillShotMillisSend);
                drawButtonCheatPlusMinus(1);
                }
    

    if( skillShotMillisSend != tempskillShotMillisSend) {
        draw7SegmentMillisecondBig(skillShotMillisSend);
        tempskillShotMillisSend = skillShotMillisSend;
    }



    if (_touchDetected) {                                       // wenn processTouch() true zurück gibt, liegen die coordinaten in touchX, touchY
        _touchDetected=0 ;                                      // zurücksetzen // menu abhängiges reagieren auf touches  
                              
            // int8_t value = drawButtonCheatPlusMinus();          // check touch area. ja nach area, also je nach button kommt -10,-1,0,1,10 zurück. umweg über variable kostet wenige cpu als 2x die funktion aufrufen
            if(int8_t value = drawButtonCheatPlusMinus()) skillShotMillisSend += value;             // check touch area 4 buttons. ja nach area, also je nach button kommt -10,-1,0,1,10 zurück

            if(drawSkillShotButton() && sendTimedPlungerButtonATimerReleaseFlag == 0) sendTimedPlungerButtonA = true;  // && verhindert zu langes drücken und das während der timer läuft wieder gedrückt wird
    
            if(drawButtonCheatLockRecord()){                    // check touch field für N/C toggle button
                CheatLockRecordMode = 3 - CheatLockRecordMode;  // fastest methode to toggle between 1 and 2
                drawButtonCheatLockRecord(CheatLockRecordMode); // refresh button auf display
            }

            if(buttonCheatBackToPlay()){
                UImenu = 1;
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;                  // für zukunft wieder scharf schalten
                batteryESP32StatusLastround = -1 ; 
            }
            if(buttonBack()){                                    // abfrage ob touch auf menübutton für back/home menu
                UImenu = 0;                                      // setze neues menu  
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;                  // für zukunft wieder scharf schalten
                batteryESP32StatusLastround = -1 ;
            } 
        }
}



// Menu Debug
void GUI::menu5() {  
    static MenuItem debugLvlOSDItem("DEBUG OSD", 0, 1, dbglvlOSD);                     // lade aktuellen stand einmalig ins objekt     
    static MenuItem dbglvlItem("Serial0 LEVEL", 0, 50, dbglvl);                        // lade aktuellen stand einmalig ins objekt
    static MenuItem uiInterval("UI INTERVAL", 10,100, UIinterval);                     // lade aus globaler var aus main.cpp
    static MenuItem dbgGamepadItem("DBG GAMEPAD", 0,32000, dbgGamePad);                // lade aus globaler var aus main.cpp
    static MenuItem dbgGamepadTresholdXItem("DEADZONE X", 1,200, tiltTresholdMenuX);   // lade aus globaler var aus main.cpp
    static MenuItem dbgGamepadTresholdYItem("DEADZONE Y", 1,200, tiltTresholdMenuY);   // lade aus globaler var aus main.cpp

    if(drawOnce){  // draw 1x
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);                     // clean background
        buttonBack(1);
        _tft.setTextFont(2);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE);   // TFT_WHITE
        _tft.setCursor(198, 4);         // x,y  10,10
        _tft.print("DEBUG");            // text
        _tft.setCursor(198, 24);        // x,y  10,30                           
        _tft.print("MODE");             // text
        
        
        debugLvlOSDItem.setButtonY(60);                                                // starting at display row 60 +40 +40 +40...
        dbglvlItem.setButtonY(100);                                                 
        uiInterval.setButtonY(140);
        dbgGamepadItem.setButtonY(180);
        dbgGamepadTresholdXItem.setButtonY(220);
        dbgGamepadTresholdYItem.setButtonY(260);
        
        // static MenuItem dbgGamepadNudgeLimiter("Nudge Limiter", 100,320, tiltLimiter);  // lade aus globaler var aus main.cpp
        // dbgGamepadNudgeLimiter.setButtonY(260);
        

        debugLvlOSDItem.draw(_tft);
        dbglvlItem.draw(_tft);
        uiInterval.draw(_tft);
        dbgGamepadItem.draw(_tft);
        dbgGamepadTresholdXItem.draw(_tft);
        dbgGamepadTresholdYItem.draw(_tft);
        
        drawOnce = 0;
    }

static unsigned long timeTrap = 0;              // 

        if (_touchDetected ) {
            _touchDetected = 0;                 // menu abhängiges reagieren auf touches . wenn zentrales processTouch() true zurück gab, liegen die coordinaten in touchX, touchY
            if(timeTrap < milliTimeCopy)        //_millis // nur + und - tasten eingaben langsam machen. return button muss sofort gehen.
                {
                timeTrap = milliTimeCopy + 100; // milliTimeCopy = cached _millis // 100ms, damit die zahleneingaben keine zahlen überspringen.
                // Überprüfe, welcher Button, welcher zeile gedrückt wurde
                int deltaDebugOSD    = debugLvlOSDItem.checkTouch(_lastTouchX, _lastTouchY);                   // return -1,0,1
                int deltadbglvl      = dbglvlItem.checkTouch(_lastTouchX, _lastTouchY);                        // return -1,0,1
                int deltaUIinterval  = uiInterval.checkTouch(_lastTouchX, _lastTouchY);                        // return -1,0,1
                int deltadbgGamePad  = dbgGamepadItem.checkTouch(_lastTouchX, _lastTouchY);                    // return -1,0,1
                int deltatiltTresholdMenuX = dbgGamepadTresholdXItem.checkTouch(_lastTouchX, _lastTouchY);     // return -1,0,1
                int deltatiltTresholdMenuY = dbgGamepadTresholdYItem.checkTouch(_lastTouchX, _lastTouchY);     // return -1,0,1
                //int deltaTiltLimiter = dbgGamepadNudgeLimiter.checkTouch(_lastTouchX, _lastTouchY);          // return -1,0,1    

                // Aktualisiere die Werte
                if (deltaDebugOSD != 0) {
                    dbglvlOSD = debugLvlOSDItem.updateValue(deltaDebugOSD);  // übernehme in globale variable
                    debugLvlOSDItem.draw(_tft);
                }
                
                if (deltadbglvl != 0) {    // Serial0 debug Level std
                    dbglvl = dbglvlItem.updateValue(deltadbglvl);  
                    dbglvlItem.draw(_tft);
                    if(dbglvl>0 && !Serial) Serial.begin(115200);
                }

                if (deltaUIinterval != 0) {
                    UIinterval = uiInterval.updateValue(deltaUIinterval);  // globale variable
                    uiInterval.draw(_tft); 
                }

                if (deltadbgGamePad != 0) {
                    dbgGamePad = dbgGamepadItem.updateValue(deltadbgGamePad);  // globale variable
                    dbgGamepadItem.draw(_tft);
                }
                    
                if (deltatiltTresholdMenuX != 0) {
                    tiltTresholdMenuX = dbgGamepadTresholdXItem.updateValue(deltatiltTresholdMenuX);  // globale variable
                    dbgGamepadTresholdXItem.draw(_tft);
                    THRESHOLD_X = tiltTresholdMenuX * 100; // setze TRESHOLDS global
                    }
                
                if (deltatiltTresholdMenuY != 0) {
                    tiltTresholdMenuY = dbgGamepadTresholdYItem.updateValue(deltatiltTresholdMenuY);  // globale variable
                    dbgGamepadTresholdYItem.draw(_tft);
                    THRESHOLD_Y = tiltTresholdMenuY * 100; // setze TRESHOLDS global
                    }    
                // tilt limiter 1-200 * (100 intern). größere zahlen sind wegen UI layout und 3 stelligen zahlen nicht gut. simple *100
            }
            // button oben links springe zu "haupt menu0()"", per UImenu variable beim nächsten durchlauf
            if(buttonBack()){  // check back button coordinates area
                UImenu = 0;                            // setze neues menu  
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;   // für zukunft wieder scharf schalten
            } 
        }
}



// Menu Gameover deepsleep/power off
void GUI::menu6() {               
    
        int startX = 0;
        int startY = 40;
   
        if(drawOnce){                 // draw once
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);   // clean background
        _tft.drawRect(startX, startY , 240, 170, TFT_WHITE);
        _tft.setTextFont(2);         
        _tft.setTextSize(6);
        _tft.setTextColor(0x2000);    // 0x3186  // 0x4208  // todo: orange leuchten lassen
        _tft.setCursor(startX + 20, startY );
        _tft.print("GAME");        
        _tft.setCursor(startX + 22, startY + 80);
        _tft.print  ("OVER");        
        _tft.setTextColor(0xFC00);    //TFT_WHITE
        _tft.setCursor(startX + 45, startY + 185);
        _tft.setTextSize(2);
        _tft.println("SHUT DOWN");
        _tft.setCursor(startX + 43, startY + 230);
        _tft.print("INSERT COIN");
                
        _touchDetected = 0;  // ausnahmsweise um eine runde auszusetzen
        drawOnce = 0;
        }

   
        static bool animationStarted = false;
        static int totalBlocks = (SCREEN_WIDTH / 8) * (SCREEN_HEIGHT / 8);
        static int16_t blockPositions[1200][2]; // 30x40 = 1200 Blöcke
        static int currentBlock = 0;
        static bool animationFlag = 0;
        // Initialisierung beim ersten Aufruf
        if (!animationStarted) {
            

            // Array mit allen Block-Positionen füllen
            int index = 0;
            for (int y = 0; y < SCREEN_HEIGHT; y += 8) {
                for (int x = 0; x < SCREEN_WIDTH; x += 8) {
                    blockPositions[index][0] = x;
                    blockPositions[index][1] = y;
                    index++;
                }
            }
    
            // Fisher-Yates Shuffle für zufällige Reihenfolge
            for (int i = totalBlocks - 1; i > 0; i--) {
                int j = random(i + 1);
                int16_t tempX = blockPositions[i][0];
                int16_t tempY = blockPositions[i][1];
                blockPositions[i][0] = blockPositions[j][0];
                blockPositions[i][1] = blockPositions[j][1];
                blockPositions[j][0] = tempX;
                blockPositions[j][1] = tempY;
            }
    
            animationStarted = true;   // calculate only once
            currentBlock = 0;
            dbglvlOSD = 0;             // macht das störende blaue fenster während abschluss animation weg. reine optik
            
        }
    
        if(_touchDetected){
           if(_lastTouchY < 240) animationFlag = true;
             else {
                    UImenu = 0;        // zurück ins hauptmenu und cleane einige flags
                    UIclearScreen = 1;
                    flagUImenu6drawMenuOnce = 1;
                    UIclearScreen = 1;
                    flagUImenu0DrawRowCells = true;
                    flagUImenu1SpriteDown = false; 
            }
            _touchDetected = 0;
        }
        
        if(animationFlag){

            for (int speedUp = 0; speedUp <16 ; speedUp++) // Zeige einen 16 Blocks pro Frame
            {
                if (currentBlock < totalBlocks) {
                _tft.fillRect(
                    blockPositions[currentBlock][0],
                    blockPositions[currentBlock][1],
                    8, 8, TFT_BLACK
                );
                currentBlock++;
                delay(1); // Kleine Verzögerung für sichtbare Animation
                } 
                else {
                        // Zeige Text nach abgeschlossener Animation
                        // Deep Sleep nach Animation
                        if (currentBlock >= totalBlocks) {
                            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
                            _tft.setTextSize(3);
                            _tft.setCursor(92, 145);  // 30
                            delay(500);
                            _tft.print("OFF");
                            if(dbglvl>1) Serial.println("esp_deep_sleep_start()");
                            delay(2000);
                            esp_deep_sleep_start(); 
                            }
                        }
                    }
        }

}



void GUI::drawText(int x, int y, const char* text) {
        _tft.setCursor(x, y);
        _tft.setTextColor(TFT_WHITE);
        _tft.setTextSize(2);
        _tft.print(text);
}



void GUI::drawRectangle(int x, int y, int w, int h, uint32_t color) {
        _tft.fillRect(x, y, w, h, color);
}



// Neuer Setter für die Touch-Instanz
void GUI::setTouch(GT911* touch) {
        _touch = touch;
}


extern uint16_t processTouchInterval;
extern unsigned long processTouchTimeFlag;
void GUI::UIupdate(int loopsPerSecond, int loopTimeMs) {

        // ESP-NOW
        if (wasConnected && (milliTimeCopy - lastPacketTime > TIMEOUT_MS)) {  // 1000ms check
                if(dbglvl) Serial.println("Verbindung verloren!");
                wasConnected = false;
                espnowButton(4);
            }


        static uint8_t oldUImenu = 0;
        // Touch-Abfrage nur hier und nur einmal pro Zyklus

// >>>>> Time Trap 20-1000 ms  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>        
        if(processTouchTimeFlag < milliTimeCopy){
           processTouchTimeFlag = milliTimeCopy + processTouchInterval;    
          _touchDetected = processTouch(&_lastTouchX, &_lastTouchY);  // methode gibt true/false in die var _touchDetected zurück
          // die Variablen _lastTouchX/Y sind in der kompletten klasse verfügbar. -touchDetected Flag löschen, nach touch benutzung.
        }
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




        // kleine logik, damit nicht bei jedem frame unnötig einen ledcwrite auslöst
        static uint8_t PWM_POWER_OUT = PWM_POWER ;                  // einmalig übernehmen.    // um veränderungen einmalig zu machen. 
        static uint8_t PWM_POWER_OLD  = 55 ;                        // einmalig fakewert
        
        if(ledTimeOffMillis  < milliTimeCopy)  PWM_POWER_OUT = 5;   // Mindest power, damit man sieht das das display an ist
          else                                 PWM_POWER_OUT = PWM_POWER;  
                   
         if(PWM_POWER_OLD != PWM_POWER_OUT){
            PWM_POWER_OLD =  PWM_POWER_OUT;
            ledcWrite(0, PWM_POWER_OUT);
            }


        if(UIclearScreen)   // clear screen
        {
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);
        UIclearScreen = 0;
        drawOnce = 1;       // set flag für einmalige initialisierung des neuen menus
        }


        //if(ledTimeOffMillis > milliTimeCopy )   // wenn display gedimmt ist, keine UI updates
        //    {  
                switch (UImenu) {
                    case 0: menu0(); break;
                    case 1: menu1(); break;
                    case 2: menu2(); break;
                    case 3: menu3(); break;
                    case 4: menu4(); break;
                    case 5: menu5(); break;
                    case 6: menu6(); break;
                   default: menu1(); break;                 // wird aufgerufen falls kein case getroffen wurde
                }
        //    }
  

    // food pedal integration
    static uint32_t uiButtonESPnowResetviewTimeFlag  = 0;   // loop session übergreifend deklarieren
    if(espnowAirButtonCurrentState != espnowAirButtonCurrentStateSend)
    {
        espnowAirButtonCurrentStateSend =  espnowAirButtonCurrentState;
        if(espnowAirButtonCurrentStateSend == 0) { 
                sendBTcommandActionKey(0);
                if(UImenu == 1) espnowButton(3);            // icon nur updaten in menu 1 // TODO nur flag setzen und menu 1 da drauf triggenr lassen.
                uiButtonESPnowResetviewTimeFlag = milliTimeCopy + 1000;      // set flag in future time mark  
            }
        if(espnowAirButtonCurrentStateSend == 1) {          // send bt command and update ui
                sendBTcommandActionKey(1);
                if(UImenu == 1) espnowButton(2);            // icon nur updaten in menu 1
                uiButtonESPnowResetviewTimeFlag=0;
            } 
    }
  
    if(uiButtonESPnowResetviewTimeFlag){                    // muss größer als 0 sein
      if(uiButtonESPnowResetviewTimeFlag < milliTimeCopy){  // wenn es jetzt wieder kleiner als die future mark ist, aktion und flag cleanen
            if(UImenu == 1) espnowButton(1);                // update ui
            uiButtonESPnowResetviewTimeFlag = 0;            // clean flag
        }
    }











    // Timetrap 2:  0-1000  die auch schneller oder langsamer als 30fps (wie der screen) laufen kann.
    if(timeMarkUIdrawDebug <= milliTimeCopy){                        
       timeMarkUIdrawDebug  = milliTimeCopy + UIintervalDBG;         // setze nächste trap

            if(dbglvlOSD){ 
                drawDebug(loopsPerSecond, loopTimeMs);               // UI dbg overlay layer
            // hier kann noch etwas rein das sich sekündlich updated

            }
        }
}



bool GUI::processTouch(int* x, int* y) {                              // war bis jetzt gut

    if (_touch != nullptr) {
        // Aktualisiere die Touch-Daten mit der öffentlichen Methode `touched()`
        uint8_t contacts = _touch->touched(GT911_MODE_POLLING);       // Polling-Modus 

        if (contacts > 0) {
            GTPoint point = _touch->getPoint(0);                      // Ersten Touch-Punkt holen
            *x = point.x;     // global verfügbar machen
            *y = point.y;     // global verfügbar machen
            // bei tastendruck alle sleep timer wieder neu setzen
            ledTimeOffMillis  = milliTimeCopy + ledTimeOff  * 1000;      
            stdMenuTimeMillis = milliTimeCopy + stdMenuTime * 1000;          
            sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; 

            if (dbglvl > 4) {
                if(dbglvl>1) Serial.printf("Touch detected: X=%d, Y=%d\n", point.x, point.y);
                _tft.fillCircle(point.x, point.y, 3, TFT_RED);
                
            }
            return true;
        }
    }
    return false;



}



void GUI::draw7SegmentNumberSmall(uint32_t number, int segmentPosX, int segmentPosY, bool trigger) {
    if(trigger){
        char buffer[9];                                                      // 8 Zeichen + Nullterminator
        snprintf(buffer, sizeof(buffer), "%08lu", number);                   // Führende Nullen
        _tft.setFreeFont(&G7_Segment7_S510pt7b);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.drawString(buffer, segmentPosX, segmentPosY);                   // Zentriert in der Breite (240px → X=120)
    }
}


// 0-25 px its pretty fast because we do not use cpu expensive calculations
// 0 draws a clean empty battery, 1-25 fills the battery
void GUI::drawBatteryLocal(uint8_t pixel){
    // es kommen werte von 0-100 (prozent quasi)
    //int frameColor = TFT_WHITE;
    //if(pixel > 90) frameColor = TFT_RED;                                     // hier haben wir die noch höchsze quantiserungsstufen.daher  können wir genauer den ladebereich markieren.
    
    int frameColor = (pixel > 90) ? TFT_RED : TFT_WHITE;
    
    pixel = pixel / 3;                                                        // etwas kleiner machen, weil beim laden 100 direkt auf ca 80% abfallen, wenn nur noch akku da ist. int division ist nicht so teuer wie float  
    if(pixel > 24) pixel = 25;                                                // limit to 25 for correct visual bar
    _tft.drawRect(70,  9, 10, 27, frameColor);                                // draw outer frame
    _tft.fillRect(71, 10, 8 , 25, bgColor);                                   // clean inner area
   if(pixel) _tft.fillRect(71 , 35 - pixel, 8, pixel, TFT_LIGHTGREY);         // draw value as bar
}


// 0-25 px its pretty fast because we do not use cpu expensive calculations
// 0 draws a clean empty battery, 1-25 fills the battery
void GUI::drawBatteryPedal(uint8_t pixel){
    _tft.drawRect(160,  9, 10, 27, TFT_WHITE);
    _tft.fillRect(161, 10, 8 , 25, bgColor);
    // int framedPercent = levelPercent >> 2;                                // ultra fast division :4 
    if(pixel) _tft.fillRect(161 , 35 - pixel, 8, pixel, TFT_DARKGREY);   
}



// only on change show new BIG LCD number
void GUI::draw7SegmentMillisecondBig(uint32_t output) {  
     
        // uint32_t summaryAlltime = ButtonFlipperLeftCounterAlltime + ButtonFlipperRightCounterAlltime;
        _tft.fillRect(61, 250, 170, 60, TFT_WHITE);
        char buffer[5];                                                      // 4 Zeichen + Nullterminator
        snprintf(buffer, sizeof(buffer), "%04" PRIu32, output);              // Führende Nullen
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextSize(2);
        _tft.setTextColor(0xD69A);                                           // ultra light grey
        _tft.drawString("8888", 25, 242);                                    
        _tft.setTextColor(bgColor );                                         // TFT_WHITE
        _tft.drawString(buffer, 25, 242);                                     
}



bool GUI::drawButtonCheatStoreProfile(int8_t mode){
    if(mode == 0){  // check touch
        return 0;
        };  
    if(mode == 1){
        _tft.fillSmoothRoundRect(70, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.setTextSize(2);
        _tft.setTextFont(2);
        _tft.setTextColor(TFT_RED);   
        _tft.drawString("S", 82, 9); // show "S" as symbol for: save ms on display to actual user(!) save plave. only in the 10 user places!
    }
    return 0;
}


bool GUI::drawButtonCheatLockRecord(int8_t mode){
    if(mode == 0) if(_lastTouchX > 129 && _lastTouchX < 171 && _lastTouchY > 3 && _lastTouchY < 50){return 1;} 
    if(mode == 1){
        _tft.fillSmoothRoundRect(130, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.setTextFont(2);
        _tft.setTextSize(2);
        _tft.setTextColor(bgColor);     
        _tft.drawString("N", 143, 9);  //show "N" symbol for: record aka normal mode, aber benchmark zwischen set und release wird im display angezeigt.
    }
    if(mode == 2){
        _tft.fillSmoothRoundRect(130, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.setTextFont(2);
        _tft.setTextSize(2);
        _tft.setTextColor(TFT_RED);     
        _tft.drawString("C", 143, 9);   //show "C" synbol for: play skill/cheat shot ms vom display wird sowohl bei tft touch als auch front A ausgelöst.
    }
    return 0;
    }


bool GUI::buttonCheatBackToPlay(int8_t mode){
    if(mode == 0) if(_lastTouchX > 189 && _lastTouchX < 240 && _lastTouchY > 5 && _lastTouchY < 50){return 1;}    
    if(mode == 1){
        _tft.fillSmoothRoundRect(190, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.fillTriangle(209, 14, 209, 35, 224, 25, bgColor);  
        _tft.fillRect(199, 20, 10, 10, bgColor); 
    }
    return 0;
}


// only on change show new BIG LCD number
void GUI::draw7SegmentNumberBig() {  
     
        uint32_t summaryAlltime = ButtonFlipperLeftCounterAlltime + ButtonFlipperRightCounterAlltime;
        char buffer[9];  // 8 Zeichen + Nullterminator
        snprintf(buffer, sizeof(buffer), "%08" PRIu32, summaryAlltime);        // Führende Nullen
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.drawString(buffer, 23, 51);  //
    
}


// 0 checkt touch und gibt wert für addition +-10 zurück
int8_t GUI::drawButtonCheatPlusMinus(int8_t mode){
       
    if(mode == 0) {
        if(_lastTouchY > 131 && _lastTouchY < 181)
        {
         if(_lastTouchX >   0 && _lastTouchX <  61){return -10;}
         if(_lastTouchX >  62 && _lastTouchX < 121){return  -1;}
         if(_lastTouchX > 121 && _lastTouchX < 181){return   1;}
         if(_lastTouchX > 180 && _lastTouchX < 240){return  10;}
         }
        return 0;
    }
    if(mode == 1) {

        _tft.fillRoundRect(4,   140, 55, 55, 12, TFT_WHITE);
        _tft.fillRoundRect(63,  140, 55, 55, 12, TFT_WHITE);
        _tft.fillRoundRect(122, 140, 55, 55, 12, TFT_WHITE);
        _tft.fillRoundRect(181, 140, 55, 55, 12, TFT_WHITE);
        _tft.setTextColor(0x0);
        _tft.setTextSize(4);
        _tft.setFreeFont();
        _tft.drawString("<<",  5, 154);
        _tft.drawString("<",  78, 154);
        _tft.drawString(">", 143, 154);
        _tft.drawString(">>",187, 154);
    }
    return 0;
}

void GUI::drawTextCheatProfileName(int8_t profileNumber){
        _tft.drawRect(5, 110, 234, 26, 0xFFFF);
        _tft.fillRect(6, 111, 198, 24, bgColor);
      if(profileNumber == 0) return;           // breakout, somit 0 cleanes frame
        _tft.setTextColor(0xFFFF);
        _tft.setTextSize(1);
        _tft.setFreeFont(&FreeMono12pt7b);
        _tft.drawString("The Adams Family", 8, 113);
        
}

void GUI::drawCheatProfileSelector(){
        _tft.fillRoundRect(3, 55, 233, 50, 10, 0xFFFF);
        _tft.fillRect(86, 60, 71, 43, 0x0);  // zahlen sichtfenster
        _tft.fillEllipse( 35, 80, 15, 15, 0xBDF7);
        _tft.fillEllipse(200, 80, 15, 15, 0xBDF7);
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextColor(0x3000);           // 0xBDF7
        _tft.setTextSize(1);
        _tft.drawString("88", 99, 66);
        _tft.setTextColor(TFT_RED);

        int platzhalter = 2;
        char buffer[3];                      // 4 Zeichen + Nullterminator
        snprintf(buffer, sizeof(buffer), "%02" PRIu32, platzhalter);        // Führende Nullen
        // _tft.drawString(" 1", 99, 66);
        _tft.drawString(buffer, 99, 66);     
        _tft.drawString("-",  29, 61);
        _tft.drawString("+", 191, 64);

}





void GUI::drawButtonTilt(int value){
    
static int oldValue = 0;                                                   // lade einmalig den stand

value=value%100;                                                           // max 99, min 0    
if(value != oldValue && value > 0){                                        // baue icon nur auf, wenn unterschied besteht und min 1 oder größer
        oldValue = value;
    
            _tft.fillRect(35, 270, 170, 45, TFT_BLACK);                    // clear field with background color
            
            for (int i = 0; i < 3; i++) {                                  // Rahmen mit 3 px Dicke
                _tft.drawRect(35 - i, 270 - i, 170 + 2 * i, 45 + 2 * i, TFT_RED);
            }
            
            // Schrift setzen
            _tft.setFreeFont(&G7_Segment7_S520pt7b);
            _tft.setTextSize(1);
            _tft.setTextColor(0x40e3);
            _tft.drawString("8888 88", 42, 276);
            _tft.setTextColor(TFT_RED);
            // Text "TILT" + Zahl (immer 2-stellig)
            String text = "TILT ";
            text += (value < 10 ? "0" : "");  // Führende 0 wenn nötig
            text += String(value);
            // Text ausgeben
            _tft.drawString(text, 42, 276);  
    }
}



// Peak meter input power 0-32767
void GUI::drawPeakMeterNudgeDirection(int8_t direction, int inputPower){
static int timerLeft, timerRight, timerUp = 0;  // timer für fade out loop übergreifend
//if(dbglvl >9) Serial.printf("timerLeft: %d | timerRight: %d | timerUp: %d\n", timerLeft, timerRight, timerUp);  // debug
//if(dbglvl >9) Serial.printf("inputPower: %d | direction: %d\n", inputPower, direction);  // debug

    int posX = 120;     // mitte des peakmeters
    int posY = 202;     // mitte des peakmeters

    if(direction > 0){  // set dir timer peak to...
       
            if (direction == 1){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_X, 32767);
                inputPower = map(safeInput, THRESHOLD_X, 32767, 0, 50);
                timerLeft  = inputPower; 
                _tft.fillRect(posX - 50, posY , 50, 3, TFT_WHITE);                  // clean complete line! wichtig
                _tft.fillRect(posX - inputPower, posY , inputPower, 3, TFT_RED);    // draw line
            }

            if (direction == 2){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_X, 32767);
                inputPower = map(safeInput, THRESHOLD_X, 32767, 0, 50);
                timerRight = inputPower;
                _tft.fillRect(posX, posY, 50, 3, TFT_WHITE);                       // draw line
                _tft.fillRect(posX, posY , inputPower, 3, TFT_RED);                // draw line
            }   
            
            if (direction == 3){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_Y, 32767);
                inputPower = map(safeInput, THRESHOLD_Y, 32767, 0, 50);
                timerUp    = inputPower;
                _tft.fillRect(posX -1, (posY +1) - 50, 3, 50, TFT_WHITE);                  // draw line
                _tft.fillRect(posX -1, (posY +1) - inputPower, 3, inputPower, TFT_RED);    // draw line
            }
        }
        else{
                // fade out / clean lines back to white, pixel wise to safe perfomance
                if(timerLeft){ // draw white line 50-timer or pixel au 50-timer+1 
                _tft.fillRect(posX -timerLeft , posY , 2, 3, TFT_WHITE);            // draw line _tft.fillRect(posX, posY -1, -inputPower, 3, TFT_RED);    // draw line
            }
        
            if(timerRight){ // draw white line 50-timer or pixel au 50-timer+1 
                _tft.fillRect(posX +timerRight , posY , 2, 3, TFT_WHITE);           // draw line
            }
        
            if(timerUp){ // draw white line 50-timer or pixel au 50-timer+1 
                _tft.fillRect(posX -1 , (posY) -timerUp, 3, 2, TFT_WHITE);          // draw line     
            }

            if(timerLeft > 0)  timerLeft--;
            if(timerRight > 0) timerRight--;
            if(timerUp > 0)    timerUp--;
 
        }
         // if(dbglvl >7) Serial.printf("-timerLeft: %d | timerRight: %d | timerUp: %d\n", timerLeft, timerRight, timerUp);  // debug   
}



void GUI::drawWaterBubble() {
    
    // Mittelpunkt des Kreises
    const int centerX = 120;                           // Mittelpunkt X
    const int centerY = 200;                           // Mittelpunkt Y
    
    // Initialisiere die Position der Blase
    int posX = centerX;
    int posY = centerY;
    
    static int lastX = posX;
    static int lastY = posY;

    // Verstärkungsfaktor (Gain) für die Empfindlichkeit
        float gain = 2.0;                           // Erhöhe diesen Wert, um die Empfindlichkeit zu steigern

        // Debug-Ausgabe der Rohdaten
        // if(dbglvl > 8) Serial.printf("pitch: %f, roll: %f\n", pitch, roll);  // NUR AUF LEVEL 9!

        // Skaliere die Beschleunigungswerte mit dem Verstärkungsfaktor
        int xOffset = pitch * gain;                 // Skaliere X
        int yOffset = roll  * gain;                 // Skaliere Y

        // Berechne die neue Position
        posX = centerX - xOffset;
        posY = centerY + yOffset;

        // Begrenze die Position kreisförmig auf einen Radius von 20 Pixeln
        float distance = sqrt((posX - centerX) * (posX - centerX) + (posY - centerY) * (posY - centerY));
        if (distance > 20) {
            // Skaliere die Position auf den Kreis mit Radius 20
            posX = centerX + (posX - centerX) * 20 / distance;
            posY = centerY + (posY - centerY) * 20 / distance;
        }


    if(lastX != posX || lastY != posY || dbglvl > 9) {  // nur zeichnen wenn sich was geändert hat
    
        lastX = posX;
        lastY = posY;

        _tft.fillCircle(120, 203, 50, TFT_WHITE);       // Lösche den äußeren Kreis
        //// Zeichne die Wasserblase an der neuen Position
        _tft.fillCircle(posX, posY, 26, TFT_WHITE);     // Äußere Kreisfläche
        
        _tft.fillCircle(posX, posY, 24, 0xBBD6);        // Innere Kreisfläche (grau)

    }

}



void  GUI::fillSpriteBackground(){  // male nur den kreis, wenn man nach ersten startanimationen menus wechselt. spart die sprite animation

    _tft.fillCircle(120, 203, 50, TFT_WHITE);
}



void GUI::drawDebug(int loopsPerSecond, int loopTimeMs) {  // debug benchmark
    
    if(flagUImenuDebugBackground){
        _tft.fillRect( 60, 0, 120, 31, TFT_BLUE);
        flagUImenuDebugBackground = 1;   // optionaler blauer hintergrund des debug info fensters
    }
     
    _tft.setCursor(64, 1);
    _tft.setTextColor(TFT_WHITE,TFT_BLUE);
    _tft.setTextFont(1); 
    _tft.setTextSize(1); 
   // _tft.printf("DBGLVL:%d V1.%03d P%d", dbglvl, BUILD_VERSION, emulationMode);
    _tft.printf("DBGLVL:%d V1.%03d P%d", dbglvl, BUILD_NUMBER, emulationMode);
 
    _tft.setCursor(64, 11);
    _tft.setTextColor(TFT_WHITE,TFT_BLUE);
    _tft.setTextSize(1);
    _tft.printf("Loops: %d/s", loopsPerSecond);
    _tft.setCursor(64, 22);
    _tft.printf("Time:%d ms %d,%d  ", loopTimeMs, _lastTouchX, _lastTouchY); 
}



// 0 drive thru, 1 refresh, 2 check touch fields, set redraw flag,  and returns -1/0/+1
int8_t GUI::drawBTprofile(int8_t mode){   // zeichnet die einstellungs zeile für auto/man profile 0-6
static int16_t posY = 220;                // später in klasse und dort per übergabe

    if(mode == 0){}                       // straight thru, do nothing

        else if(mode == 1){               // refresh the complete gfx elements of the line object
                    
                _tft.fillSmoothRoundRect(3,   posY    , 233, 37, 5, TFT_LIGHTGREY);
                _tft.fillSmoothRoundRect(10,  posY+3, 30 , 30,  4, TFT_RED, TFT_LIGHTGREY);
                _tft.fillSmoothRoundRect(200, posY+3, 30,  30,  4, TFT_DARKGREEN, TFT_LIGHTGREY);
                _tft.setTextFont(2); 
                _tft.setTextSize(3);
                _tft.setTextColor(TFT_WHITE);
                _tft.setCursor(17, posY - 7);
                _tft.print("-");
                _tft.setCursor(208, posY - 8);
                _tft.print("+");
                
                // show key mapping profile
                _tft.setTextFont(2); 
                _tft.setTextSize(1);
                _tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);   // TFT_WHITE
                _tft.setCursor(64, posY + 11);   // 65     

                if(emulationModeOverride > 0){
                    _tft.print("[MAN] PROFILE ");
                    _tft.print(emulationModeOverride); 
                }
                else{  // if override is 0
                    _tft.print("[AUTO] PROFILE ");
                    _tft.print(emulationMode);         
                }
            }
        else if(mode == 2)// check touch fields, set redraw flag,  and return 0/1/2
                {
                        if(_lastTouchX > 1 && _lastTouchX < 50 && _lastTouchY > posY && _lastTouchY < posY + 40){         // minus button
                            emulationModeOverride--  ;
                            if(emulationModeOverride < 1) emulationModeOverride = 0;
                            if(emulationModeOverride > 0) emulationMode = emulationModeOverride;

                            return -1; 
                         }
                        else if(_lastTouchX > 199 && _lastTouchX < 241 && _lastTouchY > posY && _lastTouchY < posY + 40){  // plus button
                            emulationModeOverride++;
                            if(emulationModeOverride > 5) emulationModeOverride = 6;                                       // limit der profile im menu
                            if(emulationModeOverride > 0) emulationMode = emulationModeOverride;
                            
                            return 1;
                        }
                    }  
return 0;
}



// =====================================================================================
// BT methoden



// extern void setupAdvertising();


void GUI::clearActiveBluetoothPairing() {
  btPairingActive = true;

  NimBLEServer* srv = NimBLEDevice::getServer();
  if (srv && srv->getConnectedCount() > 0) {
    for (auto h : srv->getPeerDevices()) srv->disconnect(h);
    unsigned long t = millis() + 1500;
    while (srv->getConnectedCount() > 0 && millis() < t) delay(50);
  }

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  if (adv && adv->isAdvertising()) adv->stop();

  NimBLEDevice::deleteAllBonds();
  macAdress = "00:00:00:00:00:00";
  g_lastConnHandle = 0xFFFF;

  delay(200);
  if (adv) adv->start(0);

  btPairingActive = false;
}








void GUI::clearBluetoothPairings() {

    if (dbglvl > 1) Serial.println("Clearing all BLE bonds...");
    
    // Nie während aktiver Verbindung!
    NimBLEServer* srv = NimBLEDevice::getServer();
    if (srv && srv->getConnectedCount() > 0) {
        if (dbglvl > 0) Serial.println("[clearBluetoothPairings()] Cannot clear bonds while connected!");
        return;
    }
    
    btPairingActive = true;
    
    // Advertising stoppen
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    if (adv) adv->stop();
    
    // Bonds löschen
    bool success = NimBLEDevice::deleteAllBonds();
    
    if (dbglvl > 0) Serial.printf("[deleteAllBonds()] Delete all bonds: %s\n", success ? "OK" : "FAILED");
        
    // Advertising neu starten
    if (adv) {
        delay(100);
        adv->start(0);
    }
    
    btPairingActive = false;
    macAdress = "00:00:00:00:00:00";


}



// Wandelt "AA:BB:CC:DD:EE:FF" in 6 Bytes um.
// Gibt true zurück, wenn genau 6 Hex-Bytes gelesen wurden.
static bool parseMac(const String& macStr, uint8_t out[6]) {
    int v[6];
    if (macStr.length() < 11) return false; // minimaler Schutz; eigentlich sind es 17 Zeichen
    if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x",
               &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != 6) {
        return false;
    }
    for (int i = 0; i < 6; ++i) out[i] = static_cast<uint8_t>(v[i] & 0xFF);
    return true;
}
 



void GUI::startFreshPairing() {
  if (dbglvl) Serial.println("[BLE] startFreshPairing (soft)");

  btPairingActive = true;
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  if (adv && adv->isAdvertising()) adv->stop();

  // Optional: Bonds nicht generell löschen, nur falls nötig.
  // NimBLEDevice::deleteAllBonds();

  delay(200);
  if (adv) adv->start(0);

  // Optional: 10s warten und UI Punkte malen
  unsigned long t0 = millis();
  while (millis() - t0 < 20000 && !isBleConnected()) {
    delay(1400);
    _tft.print(".");
  }

  btPairingActive = false;
}




// translate mac address to device name and set emulation mode if not set by user
// Verbesserte Device-Erkennung
String GUI::getDeviceName(const String& macAddress) {
    String oui = macAddress.substring(0, 8);
    
    if (dbglvl > 1) {
        Serial.print("MAC OUI: ");
        Serial.println(oui);
    }
    
    if (emulationModeOverride == 0) {
        // Quest-Varianten (verschiedene OUIs möglich)
        if (oui == "c0:dd:8a" || oui == "2c:26:17" || oui == "ac:37:43") {
            emulationMode = 1;
            return "META QUEST";
        }
        else if (oui == "c4:18:e9" || oui == "28:11:a5") {
            emulationMode = 2;
            return "Samsung";
        }
        else if (oui == "64:5d:86") {
            emulationMode = 5;    // this profile with keyboard strokes für pinballFX2025
            return "ASUS PC";
        }
    }
    
    return "GENERIC";
}

     
