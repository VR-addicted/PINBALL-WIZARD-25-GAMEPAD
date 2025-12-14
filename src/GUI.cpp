#include "GUI.h"
#include "GFX_ARRAYS.h"                               
#include <math.h>
#include <cstring>                                    
#include <String.h>                                   
#include <NimBLEDevice.h>








/*  kommandos um aus ui zu senden
ui->setGamepad(hid->gamepad);
ui->setKeyboard(hid->keyboard);
*/



extern int8_t emulationMode; 
extern int8_t emulationModeOverride;                  
extern int dbglvl;                                    
extern uint8_t dbglvlOSD; 
extern int PWM_POWER;                                 
extern uint8_t sleepTimer;
extern uint8_t ledTimeOff;
extern unsigned long ledTimeOffMillis;
extern int tiltGain;
extern int tiltTresholdMenuX;
extern int tiltTresholdMenuY;
extern int tiltLimiter;
extern uint8_t stdMenu;                               
extern unsigned long stdMenuTimeMillis;
extern bool drawOnce;                                 

extern unsigned long milliTimeCopy;                   
extern uint8_t stdMenuTime;                           
extern unsigned long sleepTimerMillis; 
extern unsigned long keyAbenchmarkTimeMark;
extern int8_t CheatLockRecordMode;
extern int skillShotMillisSend;
extern bool sendTimedPlungerButtonA;
extern unsigned long sendTimedPlungerButtonATimerReleaseFlag;

extern int tiltCounterGlob;
extern int dbgGamePad;
extern int flipFlopFlagSideLeft;
extern int THRESHOLD_X, THRESHOLD_Y, THRESHOLD_Z;     
extern bool secondKeyButtonFlag;

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

extern volatile uint16_t g_lastConnHandle;  




uint32_t _oldButtonFlipperLeftCounterToday    = 0 ;   
uint32_t _oldButtonFlipperRightCounterToday   = 0 ;   
uint32_t _oldButtonFlipperLeftCounterAlltime  = 0 ;   
uint32_t _oldButtonFlipperRightCounterAlltime = 0 ;   

struct HostDevice {
    const char* mac;
    const char* name;
};



MenuItem::MenuItem(const char* text, int min, int max, int initial, bool mod) {
    strncpy(label, text, sizeof(label) - 1);           
    label[sizeof(label) - 1] = '\0';                   

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

    
    tft.setFreeFont(&G7_Segment7_S510pt7b);
    tft.setTextSize(1);
        
    char buffer[4];  
    snprintf(buffer, sizeof(buffer), "%03" PRIu32, currentValue);   
    tft.setTextColor(TFT_RED);
    tft.drawString(buffer, 194, btnMinus.yStart + 7);  

    
    tft.setTextFont(2);  
    tft.setTextSize(2);  
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
        return -1;  
    }
    if (touchX >= 50 && touchX <= 100 && touchY >= btnPlus.yStart && touchY <= btnPlus.yEnd) {
        return 1;   
    }
    return 0;       
}



int MenuItem::updateValue(int delta) { 
    currentValue += delta;   
    if (currentValue < minValue) currentValue = minValue;
    if (currentValue > maxValue) currentValue = maxValue;
    return currentValue;     
}



int MenuItem::getValue() const {         
    return currentValue;
}



const char* MenuItem::getLabel() const {  
    return label;
}





extern uint8_t  UIinterval;
unsigned long UIintervalDBG    = 1000;             

extern uint8_t UImenu ;                            
int touchX, touchY;                                






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
    initDisplay();                                 
  
}



void GUI::initDisplay() {
    _tft.begin();
    _tft.writecommand(0xB3);                       
    _tft.writedata(0x08);                          
    _tft.writedata(0x0F);                          
    _tft.writedata(0x0F);                          

    _tft.setSwapBytes(true);                       
    _tft.setRotation(0);                           
    _tft.fillScreen(TFT_BLACK);                    

    
    spr.createSprite(102, 102);
    spr.fillSprite(bgColor);
    spr.fillCircle(51, 51, 50, TFT_WHITE);
}



void GUI::intro() {
    
    _tft.fillScreen(TFT_BLACK);
    uint16_t buffer[SCREEN_WIDTH];                 

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t rgb = pgm_read_byte(&fake_display[y * SCREEN_WIDTH + x]);
            uint16_t color = ((rgb & 0xE0) << 8) | 
                             ((rgb & 0x1C) << 6) | 
                             ((rgb & 0x03) << 3);  
            buffer[x] = color;
        }
        _tft.pushImage(0, y, SCREEN_WIDTH, 1, buffer);
    }

    for (int x = 0; x <= PWM_POWER; x++) {         
        int pwmValue = (int)(pow(2, (double)x / PWM_POWER * 8) - 1);
        if (pwmValue > PWM_POWER) pwmValue = PWM_POWER;
        ledcWrite(0, pwmValue);
        delay(10);
    }

    if(dbglvl == 0){                               
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
        };  
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










bool GUI::espnowButton(int8_t mode){
    if(mode == 0)      
            {
                if(_lastTouchX > 98 && _lastTouchX < 142 && _lastTouchY > 1 && _lastTouchY < 51) return 1;  
                else return 0;
            }
    if(UImenu == 1){  
            if(mode == 1){     
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_DARKGREY);    
                    _tft.setTextFont(2);         
                    _tft.setTextSize(2);
                    _tft.setTextColor(bgColor);    
                    _tft.setCursor(113, 9);
                    _tft.print("X");    
                    return 0;
            }
            if(mode == 2){     
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_GREEN);       
                    _tft.fillTriangle(110, 10, 130, 10, 120, 22, bgColor);                   
                    
                    return 0;
            }
            if(mode == 3){ 
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_LIGHTGREY);   
                    
                    _tft.fillTriangle(110, 40, 130, 40, 120, 24, bgColor);                 
                    return 0;
            }
            if(mode == 4){ 
                    _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_WHITE, TFT_RED);         
                    _tft.fillTriangle(110, 15, 130, 15, 120, 22, bgColor);                   
                    _tft.fillTriangle(110, 45, 130, 45, 120, 22, bgColor);                   
                    return 0;
            }
    }

    return 0;

    }










bool GUI::buttonBack(bool mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(10, 5, 40, 40, 5, TFT_WHITE, bgColor);    
        _tft.fillTriangle(15, 25, 30, 14, 30, 35, bgColor);                
        _tft.fillRect(30, 20, 10, 10, bgColor);                            
        return 0;
    }
    else{
         if(_lastTouchX > 5 && _lastTouchX < 55 && _lastTouchY > 1 && _lastTouchY < 51) return 1;
         else return 0;
    }
}




bool GUI::saveButton(bool mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(210, 5, 40, 40, 5, TFT_WHITE, bgColor);    
        _tft.fillTriangle(215, 25, 30, 15, 30, 35, bgColor);                
        _tft.fillRect(230, 20, 10, 10, bgColor);                            
        return 0;
    }
    else{
         if(_lastTouchX > 205 && _lastTouchX < 241 && _lastTouchY > 1 && _lastTouchY < 51) return 1;
         else return 0;
    }
}




bool GUI::cheatButton(int8_t mode){
    
    if(mode){
        _tft.fillSmoothRoundRect(190, 5, 40, 40, 5, TFT_WHITE, bgColor);     
        _tft.setTextFont(2);         
        _tft.setTextSize(2);
        _tft.setTextColor(bgColor);    
        _tft.setCursor(202, 9);
        _tft.print("C");        
         
        return 0;
    }
    else{
         if(_lastTouchX > 190 && _lastTouchX < 241 && _lastTouchY > 1 && _lastTouchY < 51) return 1;

            else return 0;
    }
}



bool GUI::drawSkillShotButton(int8_t mode){

    if(mode == 0){  
            if(_lastTouchX > 0 && _lastTouchX < 241 && _lastTouchY > 199 && _lastTouchY < 321) return 1;
            else return 0;
        }
    else if(mode == 1){
                    _tft.fillRoundRect(4 , 200 , 232 , 116, 10, TFT_WHITE);    
                    _tft.setTextFont(1);
                    _tft.setTextSize(3);
                    _tft.setTextColor(bgColor); 
                    _tft.setCursor(32, 210);     
                    _tft.print("SKILL SHOT");  
                    return 0;
        }
    else if(mode == 2 && UImenu == 4){  
                    _tft.drawRect(26,205,190,36, TFT_RED);  
                    _tft.drawRect(27,206,188,34, TFT_RED);
                    return 0;
                }
    else if(mode == 3 && UImenu == 4){  
                    _tft.drawRect(26,205,190,36, TFT_WHITE); 
                    _tft.drawRect(27,206,188,34, TFT_WHITE);
                    return 0;
    }
    else return 0;
















}








void GUI::topMenu() {  

            _tft.fillRect(0, 0, 240, 46, bgColor);                            
            _tft.fillSmoothRoundRect(10, 5, 40, 40, 5, TFT_WHITE, bgColor);   
            _tft.fillTriangle(15, 25, 30, 15, 30, 35, bgColor);               
            _tft.fillRect(30, 20, 10, 10, bgColor);                           
            _tft.fillRect(190, 5, 40, 40, TFT_WHITE);
}




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
            _tft.setTextColor(0x2000);                     
            _tft.print(88888888);                          
            _tft.setCursor(textX, textY);
            _tft.setTextColor(0xFC00);                     
            _tft.print(texts[i]);

            Ycells[i] = startY + i * cellHeight ;          
            Ycells[i+1] = startY + ((i+1) * cellHeight) ;  
        }
       
    drawOnce = 0;    
    }
    

    if (_touchDetected) {        
        _touchDetected=0 ;       

    if(dbglvl>1) _tft.fillCircle(_lastTouchX, _lastTouchY , 4, TFT_RED); 
    
    for (int i = 0; i < numEntries; i++) { 
        if (_lastTouchY  > Ycells[i] && _lastTouchY  < Ycells[i+1]) {
            
            UImenu = i + 1;      
            UIclearScreen = 1;   
            drawOnce = 1;

            break;
            }
    }
    }
}




void GUI::menu1() {    
   
    if(drawOnce){  
        
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);
       
        if(flagUImenu1SpriteDown){
            for (int spriteY = -80; spriteY < 153; spriteY += 4) {
                
                
                _tft.fillRect(70, spriteY -20, 102, 20, bgColor);
                spr.pushSprite(70, spriteY);   
                delay(20);
            }
            flagUImenu1SpriteDown = false;
            spr.deleteSprite();                
        }
        else fillSpriteBackground();           

        buttonBack(1);                         
        cheatButton(1);                        
        espnowButton(1);                       
        drawPhysicalVirtualKeys(8,1);          
        drawPhysicalVirtualKeys(7,1);          
        drawPhysicalVirtualKeys(2,0);          
        drawPhysicalVirtualKeys(1,0);          
        
        draw7SegmentNumberSmall(ButtonFlipperLeftCounterAlltime, 10, 98, 1);    
        draw7SegmentNumberSmall(ButtonFlipperLeftCounterToday, 10, 121, 1);     
        draw7SegmentNumberSmall(ButtonFlipperRightCounterAlltime, 130, 98, 1);  
        draw7SegmentNumberSmall(ButtonFlipperRightCounterToday, 130, 121, 1);   
    
        
        draw7SegmentNumberBig();               
        drawBatteryLocal(2);                   
        drawBatteryPedal(2);                   

        if(dbglvl > 9){                        
                drawPhysicalVirtualKeys(6,1);  
                drawPhysicalVirtualKeys(5,1);  
                drawPhysicalVirtualKeys(4,1);  
                drawPhysicalVirtualKeys(3,1);  
                
                drawVirtualTiltingJoystickKeys(1,1);
                drawVirtualTiltingJoystickKeys(2,1);
                drawVirtualTiltingJoystickKeys(3,1);
                drawVirtualTiltingJoystickKeys(4,1);
       }
       drawOnce = 0;
    }  
    








    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
  
    
    
    
    
    
    
















    bool refresh7SegmentBig = 0;
    if(_oldButtonFlipperLeftCounterAlltime < ButtonFlipperLeftCounterAlltime)
        {        
                draw7SegmentNumberSmall(ButtonFlipperLeftCounterAlltime, 10, 98, 1);  
                draw7SegmentNumberSmall(ButtonFlipperLeftCounterToday, 10, 121, 1);  
                _oldButtonFlipperLeftCounterAlltime = ButtonFlipperLeftCounterAlltime;
                refresh7SegmentBig = 1;   
            }
   
    if(_oldButtonFlipperRightCounterAlltime  < ButtonFlipperRightCounterAlltime)
        {       
                draw7SegmentNumberSmall(ButtonFlipperRightCounterAlltime, 130, 98, 1);
                draw7SegmentNumberSmall(ButtonFlipperRightCounterToday, 130, 121, 1); 
                _oldButtonFlipperRightCounterAlltime  = ButtonFlipperRightCounterAlltime;
                 refresh7SegmentBig = 1;  
            }
    
    if(refresh7SegmentBig) draw7SegmentNumberBig(); 
       
    
    if (_touchDetected) {                 
        _touchDetected=0 ;                
                if(buttonBack()){         
                    UImenu = 0;           
                    UIclearScreen = 1;
                    flagUImenu0DrawRowCells = true;
                    flagUImenu1SpriteDown   = false; 
                }
    
                if(cheatButton()){        
                    UImenu = 4;           
                    UIclearScreen = 1;
                    drawOnce = 1;
                }
                else{
                    }
               
                }
                else drawOnce = 0;         

  
    if(secondKeyButtonFlag){
        drawWaterBubble();  
        }
    else{
        drawPeakMeterNudgeDirection();  
    }
    
    drawButtonTilt(tiltCounterGlob);
}





void GUI::drawVirtualTiltingJoystickKeys(int8_t direction, int8_t mode){

         uint32_t drawColor = bgColor;                                  
    if(mode == 1) drawColor = TFT_WHITE;                                
    switch (direction) {                                                
        case 1: _tft.fillRect ( 116,  143,  8, 5, drawColor); break;    
        case 2: _tft.fillRect ( 116,  260,  8, 5, drawColor); break;    
        case 3: _tft.fillRect (  60,  198,  5, 8, drawColor); break;    
        case 4: _tft.fillRect ( 177,  198,  5, 8, drawColor); break;    
    default: break;
    }
}





void GUI::drawTiltDirection(int8_t direction, int8_t mode){

         uint32_t drawColor = bgColor;                                  
    if(mode == 1) drawColor = TFT_RED;                                  
    switch (direction) {                                                
        case 1: _tft.fillRect ( 116,  142,  8, 5, drawColor); break;    
        case 2: _tft.fillRect (  54,  196,  8, 5, drawColor); break;    
        case 3: _tft.fillRect ( 184,  192,  5, 8, drawColor); break;    
    default: break;
    }
}





void GUI::drawPhysicalVirtualKeys(int8_t key, int8_t mode){

    uint32_t drawColor = bgColor;                                       

    if(mode == 1){  

        if(key == 1 || key == 2)  drawColor = TFT_YELLOW;               
        if(key == 3 || key == 4)  drawColor = TFT_DARKCYAN;             
        if(key == 5 || key == 6)  drawColor = TFT_GREEN;                
        if(key == 7 || key == 8)  drawColor = TFT_WHITE;                
    }
    else if(mode == 2){ 
        if(key == 1 || key == 2)  drawColor = TFT_RED;                  
        if(key == 3 || key == 4)  drawColor = TFT_DARKCYAN;             
        if(key == 5 || key == 6)  drawColor = TFT_GREEN;                
        if(key == 7 || key == 8)  drawColor = TFT_WHITE;                
       }
    
    else{           
        if(key == 1 || key == 2)  drawColor = TFT_DARKGREY;             
        
        
        if(key == 7 || key == 8)  drawColor = TFT_RED;                  
    }

switch (key) {                                                          
        case 1: _tft.fillRect ( 225,  310,  10, 10, drawColor); break;  
        case 2: _tft.fillRect (   5,  310,  10, 10, drawColor); break;  

        case 3: _tft.fillRect (   5,  298,  10, 10, drawColor); break;  
        case 4: _tft.fillRect ( 225,  298,  10, 10, drawColor); break;  

        case 5: _tft.fillRect ( 213,  310,  10, 10, drawColor); break;  
        case 6: _tft.fillRect (  17,  310,  10, 10, drawColor); break;  

        case 7: _tft.fillRect (   5,  240,  20, 10, drawColor); break;  
        case 8: _tft.fillRect ( 215,  240,  20, 10, drawColor); break;  
        default: break;
        }                                                               
}



volatile uint32_t btLastSendTime = 0;  
volatile uint32_t btLatency = 0;       
volatile uint32_t btPacketsSent = 0;   
volatile uint32_t btPacketsLost = 0;   




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
        if (v < 0) continue;                 
        if (nibbles >= 12) return false;     
        if ((nibbles & 1) == 0) tmp[nibbles >> 1] = (uint8_t)(v << 4);
        else                    tmp[nibbles >> 1] |= (uint8_t)v;
        ++nibbles;
    }
    if (nibbles != 12) return false;         
    memcpy(out, tmp, 6);
    return true;
}


extern void verifyBLEServices();                         
extern void updateBLEStatus();    
extern void debugBLEStatus(const char* tag = "BLE");     

void GUI::menu2() { 
static String macOld = "00:01:02:03:04:05";              
static String deviceNameOld = "scan ...";                
   

    if(drawOnce){
                _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);        
                _tft.pushImage(95, 50, 50, 73, BTlogo_50x73x16);          
                flagUImenu0DrawRowCells = true;
                
                buttonBack(1);
                _tft.setTextSize(1);
                _tft.setTextFont(2);
                _tft.setTextColor(TFT_BLACK, TFT_RED);
                _tft.fillSmoothRoundRect(190, 5, 40, 40, 5, TFT_RED, bgColor);  
                _tft.setCursor(199, 8);
                _tft.print("DEL");
                _tft.setCursor(199, 25);
                _tft.print("ALL");
                _tft.setTextColor(TFT_BLACK, TFT_YELLOW);
                _tft.fillSmoothRoundRect(100, 5, 40, 40, 5, TFT_YELLOW, bgColor);  
                _tft.setCursor(109, 8);
                _tft.print("DEL");
                _tft.setCursor(106, 25);
                _tft.print("THIS");
                
                _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                drawBTprofile(1);                                         
                drawOnce = 0;
                deviceNameOld = "refresh";                                
                updateBLEStatus();
                verifyBLEServices();
            }
    _tft.setTextFont(2);

    
    static uint32_t timeTrap = 0;                                          
    if(timeTrap < milliTimeCopy){           
       timeTrap = milliTimeCopy + 1000;     
        updateBLEStatus();
        _tft.setTextColor(TFT_WHITE, bgColor);
        _tft.setTextSize(1);
        _tft.setCursor(10, 300);
        if(dbglvl) debugBLEStatus();        
        
        updateBLEStatus();                  
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

    }  

if (_touchDetected) {   
    _touchDetected=0 ;  
        
        
        if(_lastTouchX > 190 && _lastTouchX < 240 && _lastTouchY > 5 && _lastTouchY < 50){
                    _tft.fillRect(0, 140, 240, 80, TFT_BLACK);      
                    _tft.setCursor(10, 170);
                    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                    _tft.setTextSize(2);
                    _tft.println("del all bonds");
                    clearBluetoothPairings();
                    
                    formatNVS();
                    delay(1000);
                    _tft.setCursor(10, 150);
                    _tft.fillRect(0, 140, 240, 80, TFT_BLACK);
                    _tft.println("all bonds\n deleted");
                    delay(1000);
                    drawOnce = true;  
        }

        
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

        if(_lastTouchX > 95 && _lastTouchX < 145 && _lastTouchY > 50 && _lastTouchY < 130){  
                
                if (_gamepad) {                                   
                    _tft.fillRect(0, 160, 240, 60, TFT_BLACK);       
            
                    _tft.setCursor(10, 170);
                    _tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                    _tft.setTextSize(2);
                    _tft.println("Starte Pairing...");
                
                    startFreshPairing(); 
                    delay(100);                                      
            
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
      
        
        if(buttonBack())
            {
            UImenu = 0;                                              
            UIclearScreen = 1;
            flagUImenu0DrawRowCells = true;
            flagUImenu2drawBTsymbol = true;                          
        } 
        
        static uint32_t timeTrapBTmenu = 0;                          
        
        if(timeTrapBTmenu < milliTimeCopy){     
        timeTrapBTmenu = milliTimeCopy + 200;   
                if(drawBTprofile(2) != 0) drawBTprofile(1);          
        }
    }
}





void GUI::menu3() {  
    
    static MenuItem sleepTimerItem ("SLEEP TIMER",   5, 3000, sleepTimer);
    static MenuItem brightnessItem ("BRIGHTNESS",    6,  255, PWM_POWER);    
    static MenuItem ledTimeOffItem ("LED TIMER",     1,  120, ledTimeOff);
    static MenuItem tiltGainItem   ("NUDGE GAIN",    1,   50, tiltGain);
    static MenuItem stdMenuItem    ("STD MENU",      0,    5, stdMenu);
    static MenuItem stdMenuTimeItem("MENU TIMEOUT", 10,  300, stdMenuTime);
    
    if(drawOnce){ 
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);    
     
        buttonBack(1);  
            
        sleepTimerItem.setButtonY(60);   
        brightnessItem.setButtonY(100);  
        ledTimeOffItem.setButtonY(140);
        tiltGainItem.setButtonY(180);
        stdMenuItem.setButtonY(220);  
        stdMenuTimeItem.setButtonY(260);

        sleepTimerItem.draw(_tft);        
        brightnessItem.draw(_tft);
        ledTimeOffItem.draw(_tft);
        tiltGainItem.draw(_tft);
        stdMenuItem.draw(_tft);
        stdMenuTimeItem.draw(_tft);
        drawOnce = 0;   
    }
   

    
    
    static int8_t countTrap = 0;  
    countTrap++; 
    if (_touchDetected && countTrap > 3) {  
        _touchDetected=0 ;  
        countTrap     =0 ;  
    
        int deltaBright      = brightnessItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaSleep       = sleepTimerItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaLedTime     = ledTimeOffItem .checkTouch(_lastTouchX, _lastTouchY);
        int deltaTiltGain    = tiltGainItem   .checkTouch(_lastTouchX, _lastTouchY);
        int deltastdMenu     = stdMenuItem    .checkTouch(_lastTouchX, _lastTouchY);
        int deltastdMenuTime = stdMenuTimeItem.checkTouch(_lastTouchX, _lastTouchY);

        
        if (deltaSleep != 0) {
            sleepTimer = sleepTimerItem.updateValue(deltaSleep);  
            sleepTimerItem .draw(_tft);   
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
        
        
        if(buttonBack()){
            UImenu = 0;                           
            UIclearScreen = 1;
            flagUImenu0DrawRowCells = true;
            flagUImenu2drawBTsymbol = true;       
        } 
    }

}




void GUI::menu4() { 
    static int tempskillShotMillisSend = skillShotMillisSend;  
    if(drawOnce){  
       drawOnce = 0;  
                _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor); 
                buttonBack(1);                    
                drawButtonCheatLockRecord(1);     
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



    if (_touchDetected) {                                       
        _touchDetected=0 ;                                      
                              
            
            if(int8_t value = drawButtonCheatPlusMinus()) skillShotMillisSend += value;             

            if(drawSkillShotButton() && sendTimedPlungerButtonATimerReleaseFlag == 0) sendTimedPlungerButtonA = true;  
    
            if(drawButtonCheatLockRecord()){                    
                CheatLockRecordMode = 3 - CheatLockRecordMode;  
                drawButtonCheatLockRecord(CheatLockRecordMode); 
            }

            if(buttonCheatBackToPlay()){
                UImenu = 1;
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;                  
                
            }
            if(buttonBack()){                                    
                UImenu = 0;                                      
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;                  
            } 
        }
}




void GUI::menu5() {  
    static MenuItem debugLvlOSDItem("DEBUG OSD", 0, 1, dbglvlOSD);                     
    static MenuItem dbglvlItem("Serial0 LEVEL", 0, 50, dbglvl);                        
    static MenuItem uiInterval("UI INTERVAL", 10,100, UIinterval);                     
    static MenuItem dbgGamepadItem("DBG GAMEPAD", 0,32000, dbgGamePad);                
    static MenuItem dbgGamepadTresholdXItem("DEADZONE X", 1,200, tiltTresholdMenuX);   
    static MenuItem dbgGamepadTresholdYItem("DEADZONE Y", 1,200, tiltTresholdMenuY);   

    if(drawOnce){  
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);                     
        buttonBack(1);
        _tft.setTextFont(2);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE);   
        _tft.setCursor(198, 4);         
        _tft.print("DEBUG");            
        _tft.setCursor(198, 24);        
        _tft.print("MODE");             
        
        
        debugLvlOSDItem.setButtonY(60);                                                
        dbglvlItem.setButtonY(100);                                                 
        uiInterval.setButtonY(140);
        dbgGamepadItem.setButtonY(180);
        dbgGamepadTresholdXItem.setButtonY(220);
        dbgGamepadTresholdYItem.setButtonY(260);
        
        
        
        

        debugLvlOSDItem.draw(_tft);
        dbglvlItem.draw(_tft);
        uiInterval.draw(_tft);
        dbgGamepadItem.draw(_tft);
        dbgGamepadTresholdXItem.draw(_tft);
        dbgGamepadTresholdYItem.draw(_tft);
        
        drawOnce = 0;
    }

static unsigned long timeTrap = 0;              

        if (_touchDetected ) {
            _touchDetected = 0;                 
            if(timeTrap < milliTimeCopy)        
                {
                timeTrap = milliTimeCopy + 100; 
                
                int deltaDebugOSD    = debugLvlOSDItem.checkTouch(_lastTouchX, _lastTouchY);                   
                int deltadbglvl      = dbglvlItem.checkTouch(_lastTouchX, _lastTouchY);                        
                int deltaUIinterval  = uiInterval.checkTouch(_lastTouchX, _lastTouchY);                        
                int deltadbgGamePad  = dbgGamepadItem.checkTouch(_lastTouchX, _lastTouchY);                    
                int deltatiltTresholdMenuX = dbgGamepadTresholdXItem.checkTouch(_lastTouchX, _lastTouchY);     
                int deltatiltTresholdMenuY = dbgGamepadTresholdYItem.checkTouch(_lastTouchX, _lastTouchY);     
                

                
                if (deltaDebugOSD != 0) {
                    dbglvlOSD = debugLvlOSDItem.updateValue(deltaDebugOSD);  
                    debugLvlOSDItem.draw(_tft);
                }
                
                if (deltadbglvl != 0) {    
                    dbglvl = dbglvlItem.updateValue(deltadbglvl);  
                    dbglvlItem.draw(_tft);
                    if(dbglvl>0 && !Serial) Serial.begin(115200);
                }

                if (deltaUIinterval != 0) {
                    UIinterval = uiInterval.updateValue(deltaUIinterval);  
                    uiInterval.draw(_tft); 
                }

                if (deltadbgGamePad != 0) {
                    dbgGamePad = dbgGamepadItem.updateValue(deltadbgGamePad);  
                    dbgGamepadItem.draw(_tft);
                }
                    
                if (deltatiltTresholdMenuX != 0) {
                    tiltTresholdMenuX = dbgGamepadTresholdXItem.updateValue(deltatiltTresholdMenuX);  
                    dbgGamepadTresholdXItem.draw(_tft);
                    THRESHOLD_X = tiltTresholdMenuX * 100; 
                    }
                
                if (deltatiltTresholdMenuY != 0) {
                    tiltTresholdMenuY = dbgGamepadTresholdYItem.updateValue(deltatiltTresholdMenuY);  
                    dbgGamepadTresholdYItem.draw(_tft);
                    THRESHOLD_Y = tiltTresholdMenuY * 100; 
                    }    
                
            }
            
            if(buttonBack()){  
                UImenu = 0;                            
                UIclearScreen = 1;
                flagUImenu0DrawRowCells = true;
                flagUImenu2drawBTsymbol = true;   
            } 
        }
}




void GUI::menu6() {               
    
        int startX = 0;
        int startY = 40;
   
        if(drawOnce){                 
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);   
        _tft.drawRect(startX, startY , 240, 170, TFT_WHITE);
        _tft.setTextFont(2);         
        _tft.setTextSize(6);
        _tft.setTextColor(0x2000);    
        _tft.setCursor(startX + 20, startY );
        _tft.print("GAME");        
        _tft.setCursor(startX + 22, startY + 80);
        _tft.print  ("OVER");        
        _tft.setTextColor(0xFC00);    
        _tft.setCursor(startX + 45, startY + 185);
        _tft.setTextSize(2);
        _tft.println("SHUT DOWN");
        _tft.setCursor(startX + 43, startY + 230);
        _tft.print("INSERT COIN");
                
        _touchDetected = 0;  
        drawOnce = 0;
        }

   
        static bool animationStarted = false;
        static int totalBlocks = (SCREEN_WIDTH / 8) * (SCREEN_HEIGHT / 8);
        static int16_t blockPositions[1200][2]; 
        static int currentBlock = 0;
        static bool animationFlag = 0;
        
        if (!animationStarted) {
            

            
            int index = 0;
            for (int y = 0; y < SCREEN_HEIGHT; y += 8) {
                for (int x = 0; x < SCREEN_WIDTH; x += 8) {
                    blockPositions[index][0] = x;
                    blockPositions[index][1] = y;
                    index++;
                }
            }
    
            
            for (int i = totalBlocks - 1; i > 0; i--) {
                int j = random(i + 1);
                int16_t tempX = blockPositions[i][0];
                int16_t tempY = blockPositions[i][1];
                blockPositions[i][0] = blockPositions[j][0];
                blockPositions[i][1] = blockPositions[j][1];
                blockPositions[j][0] = tempX;
                blockPositions[j][1] = tempY;
            }
    
            animationStarted = true;   
            currentBlock = 0;
            dbglvlOSD = 0;             
            
        }
    
        if(_touchDetected){
           if(_lastTouchY < 240) animationFlag = true;
             else {
                    UImenu = 0;        
                    UIclearScreen = 1;
                    flagUImenu6drawMenuOnce = 1;
                    UIclearScreen = 1;
                    flagUImenu0DrawRowCells = true;
                    flagUImenu1SpriteDown = false; 
            }
            _touchDetected = 0;
        }
        
        if(animationFlag){

            for (int speedUp = 0; speedUp <16 ; speedUp++) 
            {
                if (currentBlock < totalBlocks) {
                _tft.fillRect(
                    blockPositions[currentBlock][0],
                    blockPositions[currentBlock][1],
                    8, 8, TFT_BLACK
                );
                currentBlock++;
                delay(1); 
                } 
                else {
                        
                        
                        if (currentBlock >= totalBlocks) {
                            _tft.setTextColor(TFT_WHITE, TFT_BLACK);
                            _tft.setTextSize(3);
                            _tft.setCursor(92, 145);  
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




void GUI::setTouch(GT911* touch) {
        _touch = touch;
}


extern uint16_t processTouchInterval;
extern unsigned long processTouchTimeFlag;
void GUI::UIupdate(int loopsPerSecond, int loopTimeMs) {

        
        if (wasConnected && (milliTimeCopy - lastPacketTime > TIMEOUT_MS)) {  
                if(dbglvl) Serial.println("Verbindung verloren!");
                wasConnected = false;
                espnowButton(4);
            }


        static uint8_t oldUImenu = 0;
        


        if(processTouchTimeFlag < milliTimeCopy){
           processTouchTimeFlag = milliTimeCopy + processTouchInterval;    
          _touchDetected = processTouch(&_lastTouchX, &_lastTouchY);  
          
        }





        
        static uint8_t PWM_POWER_OUT = PWM_POWER ;                  
        static uint8_t PWM_POWER_OLD  = 55 ;                        
        
        if(ledTimeOffMillis  < milliTimeCopy)  PWM_POWER_OUT = 5;   
          else                                 PWM_POWER_OUT = PWM_POWER;  
                   
         if(PWM_POWER_OLD != PWM_POWER_OUT){
            PWM_POWER_OLD =  PWM_POWER_OUT;
            ledcWrite(0, PWM_POWER_OUT);
            }


        if(UIclearScreen)   
        {
        _tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgColor);
        UIclearScreen = 0;
        drawOnce = 1;       
        }


        
        
                switch (UImenu) {
                    case 0: menu0(); break;
                    case 1: menu1(); break;
                    case 2: menu2(); break;
                    case 3: menu3(); break;
                    case 4: menu4(); break;
                    case 5: menu5(); break;
                    case 6: menu6(); break;
                   default: menu1(); break;                 
                }
        
  

    
    static uint32_t uiButtonESPnowResetviewTimeFlag  = 0;   
    if(espnowAirButtonCurrentState != espnowAirButtonCurrentStateSend)
    {
        espnowAirButtonCurrentStateSend =  espnowAirButtonCurrentState;
        if(espnowAirButtonCurrentStateSend == 0) { 
                sendBTcommandActionKey(0);
                if(UImenu == 1) espnowButton(3);            
                uiButtonESPnowResetviewTimeFlag = milliTimeCopy + 1000;      
            }
        if(espnowAirButtonCurrentStateSend == 1) {          
                sendBTcommandActionKey(1);
                if(UImenu == 1) espnowButton(2);            
                uiButtonESPnowResetviewTimeFlag=0;
            } 
    }
  
    if(uiButtonESPnowResetviewTimeFlag){                    
      if(uiButtonESPnowResetviewTimeFlag < milliTimeCopy){  
            if(UImenu == 1) espnowButton(1);                
            uiButtonESPnowResetviewTimeFlag = 0;            
        }
    }











    
    if(timeMarkUIdrawDebug <= milliTimeCopy){                        
       timeMarkUIdrawDebug  = milliTimeCopy + UIintervalDBG;         

            if(dbglvlOSD){ 
                drawDebug(loopsPerSecond, loopTimeMs);               
            

            }
        }
}



bool GUI::processTouch(int* x, int* y) {                              

    if (_touch != nullptr) {
        
        uint8_t contacts = _touch->touched(GT911_MODE_POLLING);       

        if (contacts > 0) {
            GTPoint point = _touch->getPoint(0);                      
            *x = point.x;     
            *y = point.y;     
            
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
        char buffer[9];                                                      
        snprintf(buffer, sizeof(buffer), "%08lu", number);                   
        _tft.setFreeFont(&G7_Segment7_S510pt7b);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.drawString(buffer, segmentPosX, segmentPosY);                   
    }
}




void GUI::drawBatteryLocal(uint8_t pixel){    
    _tft.drawRect(70,  9, 10, 27, TFT_WHITE);                                
    _tft.fillRect(71, 10, 8 , 25, bgColor);                                  
    
    if(pixel) _tft.fillRect(71 , 35 - pixel, 8, pixel, TFT_DARKGREY);        
}




void GUI::drawBatteryPedal(uint8_t pixel){
    _tft.drawRect(160,  9, 10, 27, TFT_WHITE);
    _tft.fillRect(161, 10, 8 , 25, bgColor);
    
    if(pixel) _tft.fillRect(161 , 35 - pixel, 8, pixel, TFT_DARKGREY);   
}




void GUI::draw7SegmentMillisecondBig(uint32_t output) {  
     
        
        _tft.fillRect(61, 250, 170, 60, TFT_WHITE);
        char buffer[5];                                                      
        snprintf(buffer, sizeof(buffer), "%04" PRIu32, output);              
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextSize(2);
        _tft.setTextColor(0xD69A);                                           
        _tft.drawString("8888", 25, 242);                                    
        _tft.setTextColor(bgColor );                                         
        _tft.drawString(buffer, 25, 242);                                     
}



bool GUI::drawButtonCheatStoreProfile(int8_t mode){
    if(mode == 0){  
        return 0;
        };  
    if(mode == 1){
        _tft.fillSmoothRoundRect(70, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.setTextSize(2);
        _tft.setTextFont(2);
        _tft.setTextColor(TFT_RED);   
        _tft.drawString("S", 82, 9); 
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
        _tft.drawString("N", 143, 9);  
    }
    if(mode == 2){
        _tft.fillSmoothRoundRect(130, 5, 40, 40, 5, TFT_WHITE, bgColor);
        _tft.setTextFont(2);
        _tft.setTextSize(2);
        _tft.setTextColor(TFT_RED);     
        _tft.drawString("C", 143, 9);   
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



void GUI::draw7SegmentNumberBig() {  
     
        uint32_t summaryAlltime = ButtonFlipperLeftCounterAlltime + ButtonFlipperRightCounterAlltime;
        char buffer[9];  
        snprintf(buffer, sizeof(buffer), "%08" PRIu32, summaryAlltime);        
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextSize(1);
        _tft.setTextColor(TFT_WHITE, TFT_BLACK);
        _tft.drawString(buffer, 23, 51);  
    
}



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
    if(profileNumber == 0) return;           
        _tft.setTextColor(0xFFFF);
        _tft.setTextSize(1);
        _tft.setFreeFont(&FreeMono12pt7b);
        _tft.drawString("The Adams Family", 8, 113);
        
}

void GUI::drawCheatProfileSelector(){
        _tft.fillRoundRect(3, 55, 233, 50, 10, 0xFFFF);
        _tft.fillRect(86, 60, 71, 43, 0x0);  
        _tft.fillEllipse( 35, 80, 15, 15, 0xBDF7);
        _tft.fillEllipse(200, 80, 15, 15, 0xBDF7);
        _tft.setFreeFont(&G7_Segment7_S520pt7b);
        _tft.setTextColor(0x3000);           
        _tft.setTextSize(1);
        _tft.drawString("88", 99, 66);
        _tft.setTextColor(TFT_RED);

        int platzhalter = 2;
        char buffer[3];                      
        snprintf(buffer, sizeof(buffer), "%02" PRIu32, platzhalter);        
        
        _tft.drawString(buffer, 99, 66);     
        _tft.drawString("-",  29, 61);
        _tft.drawString("+", 191, 64);

}





void GUI::drawButtonTilt(int value){
    
static int oldValue = 0;                                                   

value=value%100;                                                           
if(value != oldValue && value > 0){                                        
        oldValue = value;
    
            _tft.fillRect(35, 270, 170, 45, TFT_BLACK);                    
            
            for (int i = 0; i < 3; i++) {                                  
                _tft.drawRect(35 - i, 270 - i, 170 + 2 * i, 45 + 2 * i, TFT_RED);
            }
            
            
            _tft.setFreeFont(&G7_Segment7_S520pt7b);
            _tft.setTextSize(1);
            _tft.setTextColor(0x40e3);
            _tft.drawString("8888 88", 42, 276);
            _tft.setTextColor(TFT_RED);
            
            String text = "TILT ";
            text += (value < 10 ? "0" : "");  
            text += String(value);
            
            _tft.drawString(text, 42, 276);  
    }
}




void GUI::drawPeakMeterNudgeDirection(int8_t direction, int inputPower){
static int timerLeft, timerRight, timerUp = 0;  



    int posX = 120;     
    int posY = 202;     

    if(direction > 0){  
       
            if (direction == 1){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_X, 32767);
                inputPower = map(safeInput, THRESHOLD_X, 32767, 0, 50);
                timerLeft  = inputPower; 
                _tft.fillRect(posX - 50, posY , 50, 3, TFT_WHITE);                  
                _tft.fillRect(posX - inputPower, posY , inputPower, 3, TFT_RED);    
            }

            if (direction == 2){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_X, 32767);
                inputPower = map(safeInput, THRESHOLD_X, 32767, 0, 50);
                timerRight = inputPower;
                _tft.fillRect(posX, posY, 50, 3, TFT_WHITE);                       
                _tft.fillRect(posX, posY , inputPower, 3, TFT_RED);                
            }   
            
            if (direction == 3){
                int safeInput = abs(inputPower);
                safeInput = constrain(safeInput, THRESHOLD_Y, 32767);
                inputPower = map(safeInput, THRESHOLD_Y, 32767, 0, 50);
                timerUp    = inputPower;
                _tft.fillRect(posX -1, (posY +1) - 50, 3, 50, TFT_WHITE);                  
                _tft.fillRect(posX -1, (posY +1) - inputPower, 3, inputPower, TFT_RED);    
            }
        }
        else{
                
                if(timerLeft){ 
                _tft.fillRect(posX -timerLeft , posY , 2, 3, TFT_WHITE);            
            }
        
            if(timerRight){ 
                _tft.fillRect(posX +timerRight , posY , 2, 3, TFT_WHITE);           
            }
        
            if(timerUp){ 
                _tft.fillRect(posX -1 , (posY) -timerUp, 3, 2, TFT_WHITE);          
            }

            if(timerLeft > 0)  timerLeft--;
            if(timerRight > 0) timerRight--;
            if(timerUp > 0)    timerUp--;
 
        }
         
}



void GUI::drawWaterBubble() {
    
    
    const int centerX = 120;                           
    const int centerY = 200;                           
    
    
    int posX = centerX;
    int posY = centerY;
    
    static int lastX = posX;
    static int lastY = posY;

    
        float gain = 2.0;                           

        
        

        
        int xOffset = pitch * gain;                 
        int yOffset = roll  * gain;                 

        
        posX = centerX - xOffset;
        posY = centerY + yOffset;

        
        float distance = sqrt((posX - centerX) * (posX - centerX) + (posY - centerY) * (posY - centerY));
        if (distance > 20) {
            
            posX = centerX + (posX - centerX) * 20 / distance;
            posY = centerY + (posY - centerY) * 20 / distance;
        }


    if(lastX != posX || lastY != posY || dbglvl > 9) {  
    
        lastX = posX;
        lastY = posY;

        _tft.fillCircle(120, 203, 50, TFT_WHITE);       
        
        _tft.fillCircle(posX, posY, 26, TFT_WHITE);     
        
        _tft.fillCircle(posX, posY, 24, 0xBBD6);        

    }

}



void  GUI::fillSpriteBackground(){  

    _tft.fillCircle(120, 203, 50, TFT_WHITE);
}



void GUI::drawDebug(int loopsPerSecond, int loopTimeMs) {  
    
    if(flagUImenuDebugBackground){
        _tft.fillRect( 60, 0, 120, 31, TFT_BLUE);
        flagUImenuDebugBackground = 1;   
    }
     
    _tft.setCursor(64, 1);
    _tft.setTextColor(TFT_WHITE,TFT_BLUE);
    _tft.setTextFont(1); 
    _tft.setTextSize(1); 
   
    _tft.printf("DBGLVL:%d V1.%03d P%d", dbglvl, BUILD_NUMBER, emulationMode);
 
    _tft.setCursor(64, 11);
    _tft.setTextColor(TFT_WHITE,TFT_BLUE);
    _tft.setTextSize(1);
    _tft.printf("Loops: %d/s", loopsPerSecond);
    _tft.setCursor(64, 22);
    _tft.printf("Time:%d ms %d,%d  ", loopTimeMs, _lastTouchX, _lastTouchY); 
}




int8_t GUI::drawBTprofile(int8_t mode){   
static int16_t posY = 220;                

    if(mode == 0){}                       

        else if(mode == 1){               
                    
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
                
                
                _tft.setTextFont(2); 
                _tft.setTextSize(1);
                _tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);   
                _tft.setCursor(64, posY + 11);   

                if(emulationModeOverride > 0){
                    _tft.print("[MAN] PROFILE ");
                    _tft.print(emulationModeOverride); 
                }
                else{  
                    _tft.print("[AUTO] PROFILE ");
                    _tft.print(emulationMode);         
                }
            }
        else if(mode == 2)
                {
                        if(_lastTouchX > 1 && _lastTouchX < 50 && _lastTouchY > posY && _lastTouchY < posY + 40){         
                            emulationModeOverride--  ;
                            if(emulationModeOverride < 1) emulationModeOverride = 0;
                            if(emulationModeOverride > 0) emulationMode = emulationModeOverride;

                            return -1; 
                         }
                        else if(_lastTouchX > 199 && _lastTouchX < 241 && _lastTouchY > posY && _lastTouchY < posY + 40){  
                            emulationModeOverride++;
                            if(emulationModeOverride > 5) emulationModeOverride = 6;                                       
                            if(emulationModeOverride > 0) emulationMode = emulationModeOverride;
                            
                            return 1;
                        }
                    }  
return 0;
}











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
    
    
    NimBLEServer* srv = NimBLEDevice::getServer();
    if (srv && srv->getConnectedCount() > 0) {
        if (dbglvl > 0) Serial.println("[clearBluetoothPairings()] Cannot clear bonds while connected!");
        return;
    }
    
    btPairingActive = true;
    
    
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    if (adv) adv->stop();
    
    
    bool success = NimBLEDevice::deleteAllBonds();
    
    if (dbglvl > 0) Serial.printf("[deleteAllBonds()] Delete all bonds: %s\n", success ? "OK" : "FAILED");
        
    
    if (adv) {
        delay(100);
        adv->start(0);
    }
    
    btPairingActive = false;
    macAdress = "00:00:00:00:00:00";


}





static bool parseMac(const String& macStr, uint8_t out[6]) {
    int v[6];
    if (macStr.length() < 11) return false; 
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

  
  

  delay(200);
  if (adv) adv->start(0);

  
  unsigned long t0 = millis();
  while (millis() - t0 < 20000 && !isBleConnected()) {
    delay(1400);
    _tft.print(".");
  }

  btPairingActive = false;
}






String GUI::getDeviceName(const String& macAddress) {
    String oui = macAddress.substring(0, 8);
    
    if (dbglvl > 1) {
        Serial.print("MAC OUI: ");
        Serial.println(oui);
    }
    
    if (emulationModeOverride == 0) {
        
        if (oui == "c0:dd:8a" || oui == "2c:26:17" || oui == "ac:37:43") {
            emulationMode = 1;
            return "META QUEST";
        }
        else if (oui == "c4:18:e9" || oui == "28:11:a5") {
            emulationMode = 2;
            return "Samsung";
        }
        else if (oui == "64:5d:86") {
            emulationMode = 5;    
            return "ASUS PC";
        }
    }
    
    return "GENERIC";
}

     
