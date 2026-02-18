
//!   THIS IS A STABLE NIGHTLY BUILD
//!   MAYBE SOME COMPONETS ARE NOT FULLY INTEGRATED
//!   

// 31.1.2026
// VALENTINES UPDATE. RGB ANIMATONS, NEW KEY SAMPLE METHODE WITH EMV SHIELD AGAINST RGB LED INTEREFERENCES.
// 
// SET serial debug level allways to 0 if you dont want to debug, that increases speed to 295000 rounds per secound
//
// I CHANGED THE IO PINS FOR THE BUTTONS. YOU CAN USE THE OLD BUTTON PINS, BUT THE YOU HAVE TO CHANGE IT IN THE SOURCE
// Now i solder the FLIPPER L+R, RGB LED, FRONT RIGHT to the removed TF CARD READER PINS. PICTURE WILL FOLLOW.
// That minimises the traces to cut to only 1, and we can freely use the onboard pull up resistors for for the card 


// 1.1.2026 the developement of the PBWZ26 has begun.
// Unfortunely the Cheap Yellow Display esp32 from China has changing PCB. Thats annoying.
// And no interrupt line for the touch panel. For that reason i switch to an other Display.
// I think a good Candidate is a Waveshare esp32 S3 with integrated gyro/acc.
// Some Displays are on the way and we will see what display wins.
// The new MCU+display will have newer and faster Bluetooth and USB-HOST as well, 
// for extra extra low latency and some other goodies.
// And its easier to install. Maybe no soldering on the pcb. Only the switch buttons must be soldered.




//!   PINBALL WIZARD 
//!   (C) 21.01.2019 - 2025 by DOMin8or aka VR-addicted aka ORGATHM TECH
//!   18.12.2025 stable release. 
//!   In stable releases debug is "off" by default to safe cpu for max performance.
//!   If you need debug stuff, activate it in the menu.  or install a dbg release.
//!   Keep in mind: dbg releases are a bit behind. try nightly instead. thats the newest version) 
//!   Actual Benchmark speed: 1/288000 of a second reaction time to any physical button press.
//!   In reality its faster without the OSD debug menu. that costs a bit. >300000 is more realistic.
//!   
//!   If you fork my souce, allways have an eye on that OSD benchmark. dont let it go down.
//!   Try to optimize your code, and mine as well. :-)
//!   Do not wonder why i use so much globals & flags. that safes a lot of time, because api calls are expensive.
//!   Thats the reason why i implemnted some functions and methodes straight into the code.
//!   And thats the reason of extensivly using flags. And there is potential to set more flags (marked with TODO:)
//!   Some mini UI elements are not consequently controlled by flags and they are called directly.
//!   That was lazyness while coding and could be fixed. But only if you implement so much stuff, 
//!   and you want to get the last nano second. The code is in most parts highly optimzed for the
//!   cpu and this means, try to use INT calculations or bitshifting, instead of float calcs.
//!   And some other compiler performance optimizations.
//!   Shrinked or removed standart libraries to have maximum controll over the code.  

//!   This Bluetooth Gamepad is a hybrid gamepad and keyboard HID controller at the same time.
//!   Maybe UART over bluetooth is an option for the future, as well.
//!   That could opens the door to many config helpers in VR as an app. and for rumble, free game solenoid click, and much more. 
//!   In the future, we will see what the git hub and the pinball coder community will implement, or not. :-)


#ifndef BUILD_NUMBER                      // sicherung falls in der platformIO das python skript die .build_number datei nicht lesen kann
 #define BUILD_NUMBER 0 
#endif

#ifndef DBGLVL_DEFAULT                    // sicherung, falls in platformIO.ini der passeende eintrag im enviroment fehlt.
 #define DBGLVL_DEFAULT 0 
#endif

#ifndef DBGLVLOSD_DEFAULT                 // sicherung, falls in platformIO.ini der passeende eintrag im enviroment fehlt.
 #define DBGLVLOSD_DEFAULT 0 
#endif

int     dbglvl      = DBGLVL_DEFAULT    ; // Globale Debug-Variable over Serial0 UART, zentral in main.cpp 1 only benchmark on screen, >1 to 10 goes to serial // später über filesystem oder in rtc speichern
int     dbglvlOSD   = DBGLVLOSD_DEFAULT ; // only a small blue sprite with minimal info + benchmark results onscreen
int     dbglvOSDldState = dbglvlOSD;
/*!



 #################################################################################################################################
 Todo: Buzzer einbauen. Einschalt/ausschalt Melodie. Tilt Trigger Alarm optional
 
 Todo: Checken on NimBLE irgend welche Nachrichten received, z.b. Force Feedback/Rumpble, falls ja --> serial.print() it

 Todo: Spiffs filesystem um Tastenclicks und Gameprofile zu speichern.

 Todo: limiter als grauen kreis im kreis visualisieren

 Todo: Battery Voltage Meter from foot pedal synchen!

 Todo: Tiefpassfilter via software auf dem acc/gyro aktivieren. ich denke 50-200 hz max oder so müssten eigentlich reichen.
 
 Todo: akku low level <10% automatic shutdown.


 // Action Key for pinballFX arcade mode, via food padel? ALMOST DONE
 #################################################################################################################################
 PCB HACKS:
 cut trace between IO16 and IO17, to solder to gpio side
 scratch gpio0 0 to solder on the trace. no need to cut the trace.
 
 ONLY WHEN YOU NEED A BATTERY MONITOR:
 desolder the light sensor on the front if you want a battery monitor. 
 when the sensor is desoldered, you can slightly scratch the isolation on the trace to gpio34. (look into the manual)
 now connect a simple voltage divider 100k/56k.
 100k from GND to IO34, and from IO34 a 56k resistor to [battery +] pin.
 If you have a mosfet spare on your shelf, it makes sense to install the mosfet, to prevent
 power consumption from the voltage divider in battery standby mode...
 
 ###################################################################################################################################
 -----------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen PinBall FX Oculus Quest. PROFILE: 1   
 In Game Keys                   |  Game function                | bleGamepad commands
 A                             ->  Button "A",Confirm, Plunger -> BUTTON_1                          [real key, Pin IO16, 10ms debounce]
 B                             ->  Button "B" back             -> BUTTON_2                          [real key, Pin  IO0, 10ms debounce]
 Flipper Left                  ->                              -> BUTTON_7                          [real key, Pin IO17, 10ms debounce]
 Flipper Right                 ->                              -> BUTTON_8                          [real key, Pin  IO4, 10ms debounce]
 MENU                          ->  Menu                        -> BUTTON_11                         [real (B)+(A)] menu am tisch, ansonsten BACK. alternativ: pressStart()  (aus menu mit a oder b raus)         
 RT                            ->  Select Table right          -> setZ(32767)                       [virtual] tilt20degrees right         
 LT                            ->  Select Table left           -> setRZ(32767)                      [virtual] tilt20degrees left          
 Y                             ->                              ->
 TILTleft  NUDGE left to right ->  NUDGE LEFT TO RIGHT         -> setX(-32767); release: setX(0);
 TILTright NUDGE right to left ->  NUDGE RIGHT TO LEFT         -> setX(-32767); release: setX(0);
 TILTup    NUDGE front         ->  NUDGE FRONT TO BACK         -> setY(-32767); release: setY(0);
 2 Button Kombo1 B + A         ->                              -> vielleicht X machen? passt dann für starwars PB als "recenter view"
 2 Button Kombo2 B + FL L      ->                              -> vielleicht Y machen? oder = LT = Select Table scroll to left
 2 Button Kombo3 B + FL R      ->                              -> vielleicht "menu"?   oder = RT = Select Table scroll to right 
 select table <--              ->
 select table -->              ->
 ------------------------------------------------------------------------------------------------------------------------------------
 INFO: alle tests hier im block beziehen sich auf das obige profil.
 INFO: bleGamepad.pressBack(); ruft oculus menu auf. (toggle)
 INFO: setHat(0x08) keine reaktion in back to future? 0up,1upR,2R,3DR,4D,5DL,6L,7upL,8 neutral(standart)
 INFO: pressHome()   oculus desktop kommt, zurück ins spiel derzeit nur mit rechtem oculus controller, oculus taste
 INFO: pressStart()  ruft am tisch stehend das MENU auf. raus gehts mit A oder B besser BUTTON_11
 INFO: pressSelect() scheint nichts zu machen
 INFO: pressMenu()  
 INFO: pressBack()
 ------------------------------------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen Profile 2 Android Phones
 A                         ->  Bestätigen, Plunger         -> BUTTON_0
 B                         ->                              ->
 Flipper Left              ->                              ->
 Flipper Right             ->                              ->
 MENU                      ->  Menu                        ->
 RT                        ->  Select Table right          ->
 LT                        ->  Select Table left           ->
 Y                         ->                              ->
 TILTleft                  ->                              ->
 TILTright                 ->                              ->
 TILTup                    ->                              ->
 RightJoyHatDown           -> recenter view starwars pinb. -> set: gamepad.setHat1(4); release: gamepad.setHat1(0); 
 2 Button + FLIPPER  LEFT  ->                              ->
 2 Button + FLIPPER RIGHT  ->                              ->
 2 Button + FRONT RIGHT    ->                              ->
 2 Button Gyro tilt front  ->                              ->
 2 Button Gyro tilt down   ->                              ->
 2 Button Gyro tilt leftt  ->                              ->
 2 Button Gyro tilt right  ->                              ->
 ----------------------------------------------------------------------------------------------------------------------------------

 ---------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen Profile 3 PC
 A                         ->  Bestätigen, Plunger         -> BUTTON_0
 B                         ->                              ->
 Flipper Left              ->                              ->
 Flipper Right             ->                              ->
 MENU                      ->  Menu                        ->
 RT                        ->  Select Table right          ->
 LT                        ->  Select Table left           ->
 Y                         ->                              ->
 TILTleft                  ->                              ->
 TILTright                 ->                              ->
 TILTup                    ->                              ->
 RightJoyHatDown           -> recenter view starwars pinb. -> set: gamepad.setHat1(4); release: gamepad.setHat1(0); 
 2 Button + FLIPPER  LEFT  ->                              ->
 2 Button + FLIPPER RIGHT  ->                              ->
 2 Button + FRONT RIGHT    ->                              ->
 2 Button Gyro tilt front  ->                              ->
 2 Button Gyro tilt down   ->                              ->
 2 Button Gyro tilt leftt  ->                              ->
 2 Button Gyro tilt right  ->                              ->
 ----------------------------------------------------------------------------------------------------------------------------------

 ---------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen Profile 4 IPHONE
 A                         ->  Bestätigen, Plunger         -> BUTTON_0
 B                         ->                              ->
 Flipper Left              ->                              ->
 Flipper Right             ->                              ->
 MENU                      ->  Menu                        ->
 RT                        ->  Select Table right          ->
 LT                        ->  Select Table left           ->
 Y                         ->                              ->
 TILTleft                  ->                              ->
 TILTright                 ->                              ->
 TILTup                    ->                              ->
 RightJoyHatDown           -> recenter view starwars pinb. -> set: gamepad.setHat1(4); release: gamepad.setHat1(0); 
 2 Button + FLIPPER  LEFT  ->                              ->
 2 Button + FLIPPER RIGHT  ->                              ->
 2 Button + FRONT RIGHT    ->                              ->
 2 Button Gyro tilt front  ->                              ->
 2 Button Gyro tilt down   ->                              ->
 2 Button Gyro tilt leftt  ->                              ->
 2 Button Gyro tilt right  ->                              ->
---------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen Profile 5 Keyboard emulation for Pinball FX 2025
 A                         ->  Bestätigen, Plunger         -> keyboard 8
 B                         ->                              -> keyboard I
 Flipper Left              ->                              -> keyboard F
 Flipper Right             ->                              -> keyboard 6
 MENU                      ->  Menu                        -> keyboard I
 ARCADE MODE               ->                              -> keyboard 5
 RT                        ->                              ->
 LT                        ->                              ->
 Y                         ->                              ->
 TILTleft                  ->                              -> keyboard F
 TILTright                 ->                              -> keyboard D
 TILTup                    ->                              -> keyboard A
 TILTdown                  ->                              -> keyboard S
 2 Button + FLIPPER  LEFT  ->                              ->
 2 Button + FLIPPER RIGHT  ->                              ->
 2 Button + FRONT RIGHT    ->                              ->
 2 Button Gyro tilt front  ->                              ->
 2 Button Gyro tilt down   ->                              ->
 2 Button Gyro tilt leftt  ->                              ->
 2 Button Gyro tilt right  ->                              ->
 ----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen Starwars PinBall Oculus Quest  PROFILE 6
 A                         ->  Bestätigen, Plunger         -> BUTTON_0
 B                         ->                              ->
 Flipper Left              ->                              ->
 Flipper Right             ->                              ->
 MENU                      ->  Menu                        ->
 RT                        ->  Select Table right          ->
 LT                        ->  Select Table left           ->
 Y                         ->                              ->
 TILTleft                  ->                              ->
 TILTright                 ->                              ->
 TILTup                    ->                              ->
 RightJoyHatDown           -> recenter view starwars pinb. -> set: gamepad.setHat1(4); release: gamepad.setHat1(0); 
 2 Button + FLIPPER  LEFT  ->                              ->
 2 Button + FLIPPER RIGHT  ->                              ->
 2 Button + FRONT RIGHT    -> recenter view                -> RightJoyHatDown, set: gamepad.setHat1(4); release: gamepad.setHat1(0);
 2 Button Gyro tilt front  ->                              ->
 2 Button Gyro tilt down   ->                              ->
 2 Button Gyro tilt leftt  ->                              ->
 2 Button Gyro tilt right  ->                              ->
 ----------------------------------------------------------------------------------------------------------------------------------

 */


#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <nvs_flash.h>                          // for nvs_flash_init()
#include <esp_err.h>                            // for esp_err_to_name
#include <esp_system.h>                         // for esp_restart()
#include <esp_bt.h>                             // für btStop()
#include <math.h>
#include <GT911.h>
#include <TFT_eSPI.h>
#include "GUI.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <driver/adc.h>                         // for reading voltage
#include <NeoPixelBus.h>                        // rgb led driver rmt based

// alte pinbelegung
// const uint8_t ioPinSideLeft   =   4 ;                 // cut pcb trace
// const uint8_t ioPinSideRight  =  17 ;                 // cut pcb trace
// const uint8_t ioPinFrontLeft  =   0 ;                 // only scratch pcb trace GPIO0 shared with boot mode, easy to flash with (FRONT LEFT) pullup resistor
// const uint8_t ioPinFrontRight =  16 ;                 // cut pcb trace
// const uint8_t ioPinSideX      =  35 ;                 // solder 10k pullup to 3.3v. button left+right shares 1 line. (same function). liegt auf anderer speicherbank. pin35 - 32 = 3. das gleiche geht mit mod
// const uint8_t ioPinSideXbit   = ioPinSideX %32;

// IO PINS
// neue pinbelegung test 1
const uint8_t ioPinSideLeft   =  23 ;                 // hardware pullup onboard
const uint8_t ioPinSideRight  =  19 ;                 // hardware pullup onboard
const uint8_t ioPinFrontLeft  =   0 ;                 // only scratch pcb trace GPIO0 shared with boot mode, easy to flash with (FRONT LEFT) pullup resistor
const uint8_t ioPinFrontRight =   5 ;                 // hardware pullup onboard 
const uint8_t ioPinSideX      =  18 ; //-->18  pullup dran löten       // solder 10k pullup to 3.3v. button left+right shares 1 line. (same function). liegt auf anderer speicherbank. pin35 - 32 = 3. das gleiche geht mit mod
const uint8_t ioPinRGBleds    =  16 ; //-->16  100-330ohm serien widerstand und ein kondensator vom pin zu gnd 4.7 bis 10nf

const uint16_t PixelCount     =   6 ;                  
int16_t PixelReadyToSend      =   0 ;

// GRB + RMT
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> strip(PixelCount, ioPinRGBleds);    // RGBW, 3 byte pakete. Mischung RGB + RGBW möglich, aber nicht gut. eigene array füllfunktion nutzen

// base colors for my LED chain
// 0 - NEOPIXEL-R im gedruckten flipper oben auf spielfläche
// 1 - FLipper-R
// 2 - FRont-R
// 3 - FLipper-L
// 4 - FRont-L
// 5 - NEOPIXEL-L im gedruckten flipper oben auf spielfläche

struct RGB {
    u_int8_t R,G,B;
};

RGB LED_FrontLbase       = {255, 255,  20};   // leerlauf standart farbe wenn keine animation läuft
RGB LED_FrontLpressed    = {255, 255, 255};
RGB LED_FrontLflipped    = {255,   0,   0};   // leuchtet mit den flipper tasten mit
RGB LED_FrontLshifted    = {  0,   0, 255};   // long pressed shift mode
RGB LED_FrontLXbutton    = { 40,  40, 200};   // (Hell Blau) X-Button indicator color
RGB LED_FrontLSleepMode  = { 40,  10,  10};   // deepsleep reactivation signal led

RGB LED_FrontRbase       = {255,  40,  40};   // leerlauf standart farbe wenn keine animation läuft
RGB LED_FrontRpressed    = {255, 255, 255};   // wenn taste direkt gedrückt wird
RGB LED_FrontRflipped    = {255,   0,   0};   // leuchtet mit den flipper tasten mit
RGB LED_FrontRshifted    = { 20,  20, 255};
RGB LED_FrontRXbutton    = { 40,  40, 200};   // (Hell Blau) X-Button indicator color


RGB LED_NeopxLbase       = { 40,  40,  40};   // leerlauf standart farbe wenn keine animation läuft
RGB LED_NeopxLflipped    = {255,   0,   0};   // läuft parallel zum flipper L
RGB LED_NeopxLshifted    = {  5,   5, 255};

RGB LED_NeopxRbase       = { 40,  40,  40};   // leerlauf standart farbe wenn keine animation läuft
RGB LED_NeopxRflipped    = {255,   0,   0};
RGB LED_NeopxRshifted    = {  5,   5, 255};


RGB LED_FlipperLbase     = { 40,  40,  40};   // leerlauf standart farbe wenn keine animation läuft
RGB LED_FlipperLpressed  = {255,   0,   0};   // normal pressed
RGB LED_FlipperLshifted  = {  0,   0, 255};   // (Hellblau) mit aktiver "shift" taste

RGB LED_FlipperRbase     = { 40,  40,  40};   // (weiß 20%)  leerlauf standart farbe wenn keine animation läuft
RGB LED_FlipperRpressed  = {255,   0,   0};   // (max red)   ohne "shift" taste
RGB LED_FlipperRshifted  = {  0,   0, 255};   // (Hell Blau) bei aktiver "shift" taste + flipper rechts

unsigned long RGBanimationFutureTimeFlag = 0;

void RGBall(uint8_t input = 0){
    for (uint16_t i = 0; i < PixelCount ; i++) {  
        strip.SetPixelColor(i, RgbColor(input, input, input));
    }
    strip.Show();
}



void RGBbaseLight(){
    // base light config
    strip.SetPixelColor(0, RgbColor( LED_NeopxRbase.R,    LED_NeopxRbase.G,   LED_NeopxRbase.B  ));  // NEOPIXEL-R
    strip.SetPixelColor(1, RgbColor( LED_FlipperRbase.R,  LED_FlipperRbase.G, LED_FlipperRbase.B));  // FL-R
    strip.SetPixelColor(2, RgbColor( LED_FrontRbase.R,    LED_FrontRbase.G,   LED_FrontRbase.B  ));  // Front-R
    strip.SetPixelColor(3, RgbColor( LED_FlipperLbase.R,  LED_FlipperLbase.G, LED_FlipperLbase.B));  // FL-L
    strip.SetPixelColor(4, RgbColor( LED_FrontLbase.R,    LED_FrontLbase.G,   LED_FrontLbase.B  ));  // FRONT-L
    strip.SetPixelColor(5, RgbColor( LED_NeopxLbase.R,    LED_NeopxLbase.G,   LED_NeopxLbase.B  ));  // NEOPIXEL RGBW-L
    strip.Show();
}



void RGBshutDownSequence(){  // funktion darf ohne time trap in sich geschlossen laufen, da der controller so oder so danach aus geht

for (int i = 0; i < 60; i++) {
    float t = (float)i / 60.0f;       // 0.0 → 1.0
    float speed = 1.0f - powf(t, 2);  // Quadratische Kurve (langsam → schnell)

    int d1 = 10 + speed * 80;         // 10–90 ms
    int d2 = 5  + speed * 40;         // 5–45 ms

    RGBall(0);
    strip.Show();
    delay(d1);

    strip.SetPixelColor(4, RgbColor( 255, 10, 10));  // FRONT-L
    //setPinballLed(4, 255, 10, 10);
    strip.Show();
    delay(d2);
}
//setPinballLed(4, 40, 10, 10);
strip.SetPixelColor(4, RgbColor( 40, 10, 10));       // FRONT-L
strip.Show();
}



// setter um aus gui pixel zu setzen, ohne das komplette objekt in der ui zu nutzen.
void RGBFrontBasecolorsSetter(int8_t mode = 0){
    if( mode == 0 ){
                strip.SetPixelColor(4, RgbColor( LED_FrontLbase.R, LED_FrontLbase.G, LED_FrontLbase.B));             // FR-L
                strip.SetPixelColor(2, RgbColor( LED_FrontRbase.R, LED_FrontRbase.G, LED_FrontRbase.B));             // FR-R
                PixelReadyToSend++;
    }
    else{
                strip.SetPixelColor(4, RgbColor( LED_FrontLXbutton.R, LED_FrontLXbutton.G, LED_FrontLXbutton.B));    // FR-L
                strip.SetPixelColor(2, RgbColor( LED_FrontLXbutton.R, LED_FrontLXbutton.G, LED_FrontLXbutton.B));    // FR-R
                PixelReadyToSend++;
    }
}

// einfache Rainbow-Funktion (HSV → RGB)
RgbColor wheel(uint8_t pos, uint8_t brightness)
{
    pos = 255 - pos;

    if (pos < 85) {
        return RgbColor(
            (255 - pos * 3) * brightness / 255,
            0,
            (pos * 3) * brightness / 255
        );
    } else if (pos < 170) {
        pos -= 85;
        return RgbColor(
            0,
            (pos * 3) * brightness / 255,
            (255 - pos * 3) * brightness / 255
        );
    } else {
        pos -= 170;
        return RgbColor(
            (pos * 3) * brightness / 255,
            (255 - pos * 3) * brightness / 255,
            0
        );
    }
}  





// end RGB


unsigned long ms = 0;                           // decorative for millis to 22.493 format
int8_t batteryESP32Status  = 100;               // nach 8 runden ist wert stabil.this esp32  0-100%  3.3-4.2 V
int8_t batteryESPNOWstatus = 0;                 // external esp foot controller
int8_t batteryESP32StatusLastround = 100;       // 

// local battery check without MOSFET. TODO: mosfet einbauen um strom zu sparen. benutze einen pin, der so oder so auf high liegt. spart io pins

// --- 8-Bytes-Ringbuffer für geglättete ADC-Werte (bei 1000ms time trap 8 sekunden)---

static uint16_t buf[8] = {0};       // Jeder Eintrag ist ein 12-Bit-ADC-Rohwert (0..4095)
static uint8_t idx = 0;             // Index für das nächste Sample (0..7)


static bool filled = false;         // Flag: wurde der Buffer schon einmal komplett gefüllt?


// --- Hauptfunktion: liefert direkt Batterie-Prozent (0..100) ---
// Vollständig integerbasiert, keine Floats, keine Schleifen, keine Divisionen.
// mega optimierte lese routine mit bit shift division. 
int readBatteryPercent() {

  
  (void)adc1_get_raw(ADC1_CHANNEL_6);                     // Erstes Sample verwerfen (ADC braucht 1 Lesezyklus zum Stabilisieren)
  uint16_t raw = (uint16_t)adc1_get_raw(ADC1_CHANNEL_6);  // Neues ADC-Sample holen (12 Bit, 0..4095)
  buf[idx] = raw;                                         // Sample in den Ringbuffer schreiben
  if(dbglvl) {Serial.print("Read analog raw: ");Serial.println(raw);}
  
  idx++;                                                  // Index weiterschalten (0..7)
  if (idx >= 8) {
    idx = 0;
    filled = true;                                        // Buffer ist ab jetzt voll
  }

  // --- Summe der 8 Werte (manuell, ohne Schleife) ---
  uint32_t s =  buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6] + buf[7];

  // Anzahl gültiger Samples (während der Startphase <8)
  uint8_t count = filled ? 8 : idx;

  // Falls noch kein einziges Sample vorhanden ist → 0% // besser mit 100 starten, dann gehts von oben nach unten, verhindert zu frühes auslösen des deepsleep an battery minimum
  if (count == 0) return 100;

  // Durchschnitt der Samples:
  // Bei 8 Samples → avg = sum >> 3 (Bitshift statt Division)
uint32_t avg = (count == 8) ? (s >> 3) : (s / count); 
if (dbglvl) { Serial.print("avg: "); Serial.println(avg); }



// --- Kalibrierte Prozentberechnung im ADC-Raum --- 
// Diese beiden Werte musst du später mit echten Messungen setzen: 
// ADC-Wert bei "0 % Akku" (z.B. 3.30 V) const int RAW_0PCT = 2600; 


const int RAW_0PCT = 2600;     // Platzhalter (raw adc!! wert messen)
// ADC-Wert bei "100 % Akku" (z.B. 4.20 V)
const int RAW_100PCT = 3200;   // Platzhalter (raw adc!! wert messen)
int pct;
if ((int32_t)avg <= RAW_0PCT) { pct = 0; } 
  else if ((int32_t)avg >= RAW_100PCT) { pct = 100; } 
  else { int32_t num = ((int32_t)avg - RAW_0PCT) * 100;
int32_t denom = (RAW_100PCT - RAW_0PCT);
pct = (int)(num / denom); // integer, aber völlig ausreichend }
if (dbglvl) { Serial.print("ungefiltert. Prozent: "); Serial.println(pct);}
return pct;
  }
}




// ESP-NOW
#include <esp_now.h>                                  // ESP-NOW Bibliothek

uint8_t senderMac[] = {0x78, 0xE3, 0x6D, 0x1A, 0x8C, 0xC8}; // MAC des Senders eintragen
int TIMEOUT_MS = 1000;                                // Zeit ohne esp-now Empfang, bis "Verbindung verloren" (1s)

// Variablen für Verbindungsüberwachung
unsigned long lastPacketTime = 0;
bool wasConnected = false;
// Zustandsvariable (vom Sender empfangen)
volatile uint8_t espnowAirButtonCurrentState = 0;

// Callback wenn Daten empfangen wurden

struct __attribute__((packed)) KeyEvent {
  uint8_t state;                                      // 0=release, 1=press
};


const char* DEVICE_NAME = "X-Arcade";                    //! device name
const char* DEVICE_MANUFACTURER = "X-Arcade";           //! manufacturer
bool _isBleConnected = 0;

  
String macAdress = "00:00:00:00:00:00";               // dummy mac bis connected

#include "HIDKeycodes.h"
#include "HIDKeyboardTypes.h"
#include "GamepadKeyboardHID.h"

GamepadKeyboardHID* hid;

//volatile uint16_t g_lastConnHandle = 0xFFFF;  // 0xFFFF = none
volatile bool btPairingActive = false;          // blockiert Eingaben nur während Koppeln/Löschen im BT-Menü
volatile uint16_t g_lastConnHandle = 0xFFFF;


// Hilfsfunktion zur CCCD-Wiederherstellung
void restoreHIDNotifications(NimBLEServer* srv) {
    NimBLEService* hidSvc = srv->getServiceByUUID(NimBLEUUID((uint16_t)0x1812));
    if (!hidSvc) return;
    
    auto chars = hidSvc->getCharacteristics();
    for (auto c : chars) {
        // Report Characteristics haben normalerweise Notify-Property
        if (c->getProperties() & NIMBLE_PROPERTY::NOTIFY) {
            NimBLEDescriptor* cccd = c->getDescriptorByUUID(NimBLEUUID((uint16_t)0x2902));
            if (cccd) {
                uint8_t notifyOn[2] = {0x01, 0x00}; // Enable notifications
                cccd->setValue(notifyOn, 2);
                
                if (dbglvl > 2) {
                    Serial.printf("[BLE] Restored CCCD for char %s\n", 
                                  c->getUUID().toString().c_str());
                }
            }
        }
    }
}

void updateBLEStatus() {
    static uint8_t lastConnCount = 0;
    static unsigned long lastCheck = 0;
    
    // // Nur alle 500ms prüfen (spart CPU)
    // unsigned long now = millis();
    // if (now - lastCheck < 500) return;
    // lastCheck = now;
    
    NimBLEServer* srv = NimBLEDevice::getServer();
    if (!srv) return;
    
    uint8_t connCount = srv->getConnectedCount();
    
    // Verbindungsänderung erkannt
    if (connCount != lastConnCount) {
        if (connCount > 0) {
            // Neue Verbindung
            auto connectedPeers = srv->getPeerDevices();
            if (!connectedPeers.empty()) {
                g_lastConnHandle = connectedPeers[0];
                
                // MAC-Adresse abrufen
                NimBLEConnInfo connInfo = srv->getPeerInfo(g_lastConnHandle);
                macAdress = String(connInfo.getAddress().toString().c_str());
                
                if (dbglvl) {
                    Serial.printf("[BLE] Connected: %s (handle: 0x%04X)\n", 
                                  macAdress.c_str(), g_lastConnHandle);
                }
                
                // Optional: CCCD-Wiederherstellung versuchen
                //restoreHIDNotifications(srv);
            }
        } else {
            // Verbindung getrennt
            if (dbglvl) {
                Serial.printf("[BLE] Disconnected (prev MAC: %s)\n", macAdress.c_str());
            }
            g_lastConnHandle = 0xFFFF;
            macAdress = "00:00:00:00:00:00";
        }
        
        lastConnCount = connCount;
    }
}



// ======= ZUSÄTZLICHE BLE DEBUG-FUNKTION, kann auch raus, sobald es läuft =======
void verifyBLEServices() {
    if (dbglvl == 0) return;

    NimBLEServer* srv = NimBLEDevice::getServer();
    if (!srv) {
        if(dbglvl) Serial.println("[BLE] ERROR: No server instance!");
        return;
    }

    auto probeService = [&](const NimBLEUUID& svcUuid,
                            std::initializer_list<NimBLEUUID> chars,
                            const char* name) {
        NimBLEService* s = srv->getServiceByUUID(svcUuid);
        if (!s) {
            if(dbglvl) Serial.printf("[BLE] MISSING service %s (%s)\n",
                          name, svcUuid.toString().c_str());
            return;
        }
        if(dbglvl) Serial.printf("[BLE] OK service %s (%s)\n",
                      name, s->getUUID().toString().c_str());
        for (const auto& cu : chars) {
            NimBLECharacteristic* c = s->getCharacteristic(cu);
            if (c) {
                uint8_t props = c->getProperties();
                if(dbglvl) Serial.printf("  - char %s props=0x%02X\n",
                              c->getUUID().toString().c_str(), props);
            } else {
                if(dbglvl) Serial.printf("  - MISSING char %s\n", cu.toString().c_str());
            }
        }
    };

    // Standard-Services: GAP/GATT (werden vom Stack angelegt)
    probeService(NimBLEUUID((uint16_t)0x1800), { /* Device Name 0x2A00, Appearance 0x2A01 optional */ }, "GAP");
    probeService(NimBLEUUID((uint16_t)0x1801), { /* Service Changed 0x2A05 optional */ }, "GATT");


// Nachher: nur informative Ausgabe
    if(dbglvl) Serial.println("[BLE] GAP/GATT services are managed by the stack (not listed in app server)");
    if(dbglvl) Serial.println("[BLE] GAP (0x1800) / GATT (0x1801): handled by controller, not app-created -> expected not to list here");
    
    // Battery Service
    probeService(NimBLEUUID((uint16_t)0x180F), { NimBLEUUID((uint16_t)0x2A19) }, "Battery");

    // Device Information (typische Felder; je nach Lib optional)
    probeService(NimBLEUUID((uint16_t)0x180A), {
        NimBLEUUID((uint16_t)0x2A29), // Manufacturer Name
        NimBLEUUID((uint16_t)0x2A24), // Model Number
        NimBLEUUID((uint16_t)0x2A25), // Serial Number
        NimBLEUUID((uint16_t)0x2A27), // Hardware Revision
        NimBLEUUID((uint16_t)0x2A26)  // Firmware Revision
    }, "Device Information");

    // HID Service (CompositeHID / BleGamepad / Keyboard)
    probeService(NimBLEUUID((uint16_t)0x1812), {
        NimBLEUUID((uint16_t)0x2A4A), // HID Information
        NimBLEUUID((uint16_t)0x2A4B), // Report Map
        NimBLEUUID((uint16_t)0x2A4E), // Protocol Mode
        NimBLEUUID((uint16_t)0x2A4D), // Report (mehrfach möglich)
        NimBLEUUID((uint16_t)0x2A4C)  // HID Control Point
    }, "HID");
}



void debugBLEStatus(const char* tag = "BLE")
{
    if (dbglvl == 0) return;

    NimBLEServer*      srv = NimBLEDevice::getServer();
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();

    const uint8_t connCnt   = srv ? srv->getConnectedCount() : 0;
    const bool    connected = connCnt > 0;
    const bool    advOn     = (adv && adv->isAdvertising());

    if(dbglvl) Serial.printf("[%s] getAddress=%s, macAdress=%s, connected=%s, peers=%u, adv=%s, comp=%p, gp=%p, kb=%p\n",
        tag,
        NimBLEDevice::getAddress().toString().c_str(),
        macAdress.c_str(), // macAdress.c_str()
        connected ? "yes" : "no",
        connCnt,
        advOn ? "on" : "off");
        
    if(dbglvl) Serial.printf("[%s] Connected: %s, Peers: %d, Advertising: %s, Handle: 0x%04X\n",
        tag,
        srv && srv->getConnectedCount() > 0 ? "YES" : "NO",
        srv ? srv->getConnectedCount() : 0,
        adv && adv->isAdvertising() ? "ON" : "OFF",
        (unsigned)g_lastConnHandle);

    if (srv) {
        auto hasSvc = [&](uint16_t u){ return srv->getServiceByUUID(NimBLEUUID(u)) != nullptr; };
        if(dbglvl) Serial.printf("[%s] services: HID=%s, BAT=%s, DI=%s\n",
            tag,
            hasSvc(0x1812) ? "present" : "MISSING",
            hasSvc(0x180F) ? "present" : "MISSING",
            hasSvc(0x180A) ? "present" : "MISSING");
    }
}



void formatNVS(){
  btStop(); 
  if(dbglvl)Serial.println("btStop()");
  delay(500);
  NimBLEDevice::deinit(true);  // BLE-Stack sauber runterfahren
  if(dbglvl)Serial.println("deinit()");
  delay(1000);
  if(dbglvl)Serial.println("NVS Flash Erase and Init started...");
  // NVS komplett löschen + neu initialisieren
  esp_err_t err = nvs_flash_erase();   // **WIPT ALLES im NVS** (Bonds, WiFi-Creds, Preferences...)
  if (err != ESP_OK) {
    if(dbglvl)Serial.printf("nvs_flash_erase() failed: %s\n", esp_err_to_name(err));
  }
  err = nvs_flash_init();
  if (err != ESP_OK) {
    if(dbglvl)Serial.printf("nvs_flash_init() failed: %s\n", esp_err_to_name(err));
  }
  delay(1000);
  if(dbglvl)Serial.println("NVS Flash Erase and Init done. Restarting...");
  // Optional: sauberen Neustart, um alle Caches/Controller frisch zu haben
  esp_restart();
}



bool isBleConnected() {
    NimBLEServer* srv = NimBLEDevice::getServer();
    bool connected = srv && srv->getConnectedCount() > 0;
    
    // // Opportunistische MAC-Update bei jedem Check
    // if (connected && macAdress == "00:00:00:00:00:00") {
    //     auto peers = srv->getPeerDevices();
    //     if (!peers.empty()) {
    //         NimBLEConnInfo info = srv->getPeerInfo(peers[0]);
    //         macAdress = String(info.getAddress().toString().c_str());
    //     }
    // }


    // if(dbglvl > 1)Serial.printf("isBleConnected() %d\n", connected );
    return connected;
}// BT END ========================================================================================================



uint8_t UIinterval  =   40;               // sets every x ms screenrefresh. costs power. 30 to 50 is very good. 50 makes display minimal slower, but reaction is at 50 ms 4 times higher. 
bool _touchDetected = false;  
uint32_t UIintervalTimerFlag   = 0;
int _lastTouchX = 0;
int _lastTouchY = 0;
int _cachedTouchX = -1;
int _cachedTouchY = -1;
uint16_t processTouchIntervalSpeed    =  40; // touch scan speed user operates in menus via touch. 10ms-199ms timetrap für gute reactivität 10-40ms im NICHT throttle mode,
uint16_t processTouchIntervalThrottle = 500; // touch scan speed in throttle mode aka user is gaming no use of display
uint16_t processTouchNextKeyDelay     = 300; // repeat on button pressed+hold. geschwindigkeit zwischen den touch tasten ausgaben
uint32_t touchThrottleTimeout         =4000; // nach zeit x den highspeed mode scan mode verlassen und volle performance zurück in den main loop
uint16_t touchHysteresis              =   1; // Toleranz in Pixeln ([1-5] falls der Finger zittert. lustiges feature. wenn man den butten reibt, gehts schneller (touchHytseresisKeyRepeatTime))
uint16_t touchHytseresisKeyRepeatTime = 80;  // spielt nur im speed mode eine rolle, damit nicht zu viele touches/sec auf die moving finger methode raus gefeuert werden. es soll schneller, aber nicht max schnell sein.

uint32_t lastTouchActivityTimerFlag = 0;  // Speichert den Zeitpunkt der letzten Berührung
uint32_t processTouchNextKeyTimeFlag= 0; 
uint32_t processTouchTimeFlag       = 0;
uint32_t timeTrapOneSecond          = 0;

int processTouchRepeatBlockerPerMenu = 0; // kann auch mit initialisiert werden.
    

int8_t emulationMode = 1;                 // bluetooth HID profiles 1 = Quest, 2 = PC, 3 = Android , 4 = Iphone, 5 = Switch (per funktion und if/case rutsche) 
int8_t emulationModeOverride = 0;         // 0 automatic mode in emulationMode, 


#define debounceKey 10                    // 10 ms (machanical flipper keys, x-key, front left+right key)
uint32_t milliTimeCopy = 0;

uint8_t UImenu      =    0;               // Startmenü-Index (auch in klasse lese und schreibbar?) // später über filesystem oder in rtc speichern
int     sleepTimer  =   15;               // 10-300 Minuten nach letztem tastendruck deep sleep shutdown. display einbrennen verhindern. akku schonen. später über filesystem oder in rtc speichern
int     ledTimeOff  =   60;               // 60 Sekunden = 1 minuten bis die leds zum stromsparen ausgehen. jede taste/touch reaktiviert timer
bool    drawOnce    =    1; 
uint8_t stdMenu     =    4;               // fallback menu, next variable defines timeout time
int     stdMenuTime =   20;               // springt danach zurück in stdMenu
int     tiltCounterGlob= 0;
int     dbgGamePad     = 0;               // um die richtigen tasten codes mit rechter flipper taste raus zu bekommen.
unsigned long UIpreviousMillis  = 0;      // Letzter Zeitpunkt, zu dem der Code ausgeführt wurde
unsigned long ledTimeOffMillis  = millis() + ledTimeOff  * 1000;      // 90 seKunden bis die LEDs ausgehen
unsigned long sleepTimerMillis  = millis() + sleepTimer  * 60000;      // 10 Minuten deep sleep timer
unsigned long stdMenuTimeMillis = millis() + stdMenuTime * 1000;      // 20 Sekunden bis zum nächsten Menü

const uint8_t LED_Order[6] = {3, 4, 5, 0, 2, 1};// Optische Reihenfolge für K.I.T.T. effekt straight von links nach rechts: SideL, FrontL, NeoL, NeoR, FrontR, SideR
bool AnimationIsRunning = false;          // flag sorgt dafür das der AnimationRunningStep counter auch bei durchlauf über 0 nicht unterbrochen wird.
int AnimationIsRunningStep = 0;           // hoch bis animation step max, dann wieder ab 0. kein flag. timemmark als flag
uint16_t AnimationActivationTime = 15000; // 15 sekunden
unsigned long AnimationDurationEndTimerFlag = 0; // eigentlich internes paramter und gelocked durch AnimationIsRunning
uint8_t  animationNumber     = 0;
static uint8_t  animationSpeed  = 100;
unsigned long gyroUpdateTimeTrapTimerFlag = 0;
int8_t gyroTimeTrapTimerCycle = 30;       // z.b. 20 ms also 50x die sekunde mit dem per separtem parallel task gesampelten daten synchen
bool     gamepadSendReportFlag  = false;  // wenn true, wird am ende der schleife ein gamepad report gesendet. so können mehrere tasten in einem report gesendet werden.
bool     keyboardSendReportFlag = false;  // wenn true, wird am ende der schleife ein keyboard report gesendet. so können mehrere tasten in einem report gesendet werden.
uint32_t skillShotMillisSend    = 426 ;   // ms
unsigned long skillShotMillisStartTime = 0;
int8_t CheatLockRecordMode = 1;           // 1 standart mode alles wie immer aber samplet immer press-release zeitdifferenz, 2 skillshot->A taste bei release triggert sendTimedPlungerButtonA 
unsigned long keyAbenchmarkTimeMark = 0;  // speicher bei drücken von A den timestamp und bei release ziehen wir millis() ab. differenz = benchmark

/* global timer vars */                   // tastenabfrage variablen timemarks and flipflop flags



uint32_t keyTimerFlagSideLeft    = 0;     // Flipper links debounce Timemark
int flipFlopFlagSideLeft         = 0;

uint32_t keyTimerFlagSideRight   = 0;     // Flipper rechts debounce Timemark
int flipFlopFlagSideRight        = 0;

uint32_t keyTimerFlagFrontLeft   = 0;     // Front Taste links
int flipFlopFlagFrontLeft        = 0;

uint32_t keyTimerFlagFrontRight  = 0;     // Plunger
int flipFlopFlagFrontRight       = 0;

uint32_t keyTimerFlagTwoButton   = 0;     // spezial keys durch 2 front tasten kombo ausgelöst
int flipFlopFlagTwoButton        = 0;

uint32_t keyTimerFlagFourButton  = 0;     // spezial keys durch 4 tasten kombo ausgelöst
int flipFlopFlagFourButtons      = 0;

uint32_t keyTimerFlagTilt        = 0;     // Timer Flag, wenn sich dieser ändert in größer als aktuelle zeit, setze auch direction, dann wird "flipFlopFlagTilt" high, bis debounce zuende ist
int flipFlopFlagTilt             = 0;     // änderung eines neuen timers wird das hier high
int keyTimerFlagTiltDirection    = 0;     // 0 = nicht zeichnen (überspringen), 1 = links, 2 = rechts, 3 = hoch, 4 = reset hintergrund

uint32_t secondKeyButtonTimeMark = 0;     // Merke Timestamp sobald die taste antriggert, um später simple die differenz zu 400ms normal to multi function button schnell berechnen zu können.
bool secondKeyButtonFlag         = false; // wenn dieser true ist, dann wird die zweite taste gedrückt, und die erste taste wird nicht mehr abgefragt.
int  secondKeyActivationTime     = 800;   // 400 ms bis der zweite button erkannt wird.
bool secondKeySetLaterRelease    = 0;
unsigned long secondKeySetLaterReleaseTimerFlag = 0;
bool sendTimedPlungerButtonA     = false;
unsigned long sendTimedPlungerButtonATimerReleaseFlag = 0;
uint32_t keyTimerFlagActionKey   = 0;     // Flipper rechts debounce Timemark
int flipFlopFlagActionKey        = 0;
bool releaseTrickFlagActionKey;

// sendet nach debouncetilt ca 100 ms die padX (0)message
#define debounceTilt 100 

// clean display circle with arrow after 500ms     
int circleTimeToDisplay          = 500; 


uint32_t keyTimerFlagAngleLeft   = 0;
int flipFlopFlagAngleLeft        = 0;    

uint32_t keyTimerFlagAngleRight  = 0;
int flipFlopFlagAngleRight       = 0;    

int keyTimerFlagAngleUp          = 0;
int flipFlopFlagAngleUp          = 0;

int keyTimerFlagAngleDown        = 0;
int flipFlopFlagAngleDown        = 0;

int potiEinsTimerFlag            = 0;

//float accelerationTriggerG      = 1.9;    // 1.8g  G-Force value to trigger the Tilt Key  ACHTUNG, ES MUSS EINE KOMMA ZAHL SEIN. z.b. 1.8 , 2.0 oder 2.3 etc.

int angleTrigger                = 12;       // Bei Neigung mehr als .. Grad, aktiviere Spezial tasten. Z.b. rechter joystick, d-pad

int benchmarkTimerFlag          = 0;
int benchmarkRoundValue         = 0;

int counterKeysPressedOverall   = 1337;     // TODO: load/save to lokal storage for overall pressed and session pressed keys  
int counterKeysPressedToday     = 0;



// RGB

// 1. Rainbow: Nutzt deine vorhandene wheel() Funktion
void effect_rainbow(unsigned long t) {
    for (uint16_t i = 0; i < PixelCount; i++) {
        // i * 42 versetzt die Farben auf den 6 LEDs (255 / 6)
        uint8_t hue = (uint8_t)((t / 10) + (i * 42)) & 255; 
        strip.SetPixelColor(i, wheel(hue, 200));
    }
}

// 2. Heartbeat: Pulsierendes Rot (Sinus-Welle)
void effect_heartbeat(unsigned long t) {
    // Phase berechnen: t * 0.003f steuert die Frequenz
    float pulse = (sinf(t * 0.003f) + 1.0f) / 2.0f; 
    uint8_t r = (uint8_t)(255 * pulse);
    for (uint16_t i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, RgbColor(r, 0, 0));
    }
}

// 3. Glitter: Zufällige Pixel faden aus (Random Color per Pixel)
void effect_glitter(unsigned long t) {
    static unsigned long lastSparkle = 0;
    // Alle LEDs ein Stück abdunkeln für den Fade-Effekt
    for (uint16_t i = 0; i < PixelCount; i++) {
        RgbColor col = strip.GetPixelColor(i);
        col.Darken(5); 
        strip.SetPixelColor(i, col);
    }
    // Alle 60ms ein neues zufälliges Pixel zünden
    if (t - lastSparkle > 60) {
        strip.SetPixelColor(random(PixelCount), wheel(random(255), 255));
        lastSparkle = t;
    }
}

// 4. Scanner: Der klassische K.I.T.T. / Cylon Effekt
void effect_comet(unsigned long t) {
    // Wandelt Zeit in eine Position 0-10 um (für Hin- und Her-Lauf)
    int pos = (t / 120) % 10; 
    int activeLED = (pos > 5) ? (10 - pos) : pos; 

    for (uint16_t i = 0; i < PixelCount; i++) {
        if (i == activeLED) {
            strip.SetPixelColor(i, RgbColor(255, 255, 255)); // Kopf des Kometen
        } else {
            RgbColor col = strip.GetPixelColor(i);
            col.Darken(30); // Schweif-Effekt
            strip.SetPixelColor(i, col);
        }
    }
}







void RGB_animation(uint8_t IN_animationNumber = 0, uint16_t IN_animationDurationMS = 1000, uint8_t IN_animationSpeed = 100)
{
    static unsigned long animationStartTime = 0;
    static uint16_t effectiveDuration = 0; // Lokaler Puffer für die berechnete Zeit

    if(AnimationIsRunning){
        if(milliTimeCopy > AnimationDurationEndTimerFlag) {
            AnimationIsRunning = false;
            RGBbaseLight();
            return;
        }

        // Speed-Skalierung (128 = 1.0x)
        float speedFactor;
        if (animationSpeed <= 128) speedFactor = 0.2f + (animationSpeed / 128.0f) * 0.8f;
        else speedFactor = 1.0f + ((animationSpeed - 128) / 127.0f) * 9.0f;

        uint32_t elapsed = (uint32_t)((milliTimeCopy - animationStartTime) * speedFactor);

        switch(animationNumber) {
            case 1: effect_rainbow(elapsed); break;
            case 2: effect_heartbeat(elapsed); break; 
            case 3: effect_glitter(elapsed); break;
            case 4: effect_comet(elapsed); break;
        }
        
        PixelReadyToSend++;  // set trigger for //strip.Show();
    }
    else {  
        if(IN_animationNumber == 0) return;
        
        animationNumber = IN_animationNumber;
        animationSpeed = IN_animationSpeed;
        animationStartTime = milliTimeCopy;
        
        // MINDESTZEIT-LOGIK:
        uint16_t minTime = 1000; // Globales Minimum
        
        switch(animationNumber) {
            case 1: minTime = 3000; break; // Rainbow braucht Zeit zum Fließen
            case 2: minTime = 4000; break; // Heartbeat braucht mind. 2-3 Schläge
            case 3: minTime = 5000; break; // Glitter muss langsam ausglühen
            case 4: minTime = 2500; break; // Scanner sollte paar mal pendeln
        }
        
        // Wir nehmen den höheren Wert: Entweder API-Wunsch oder unser Minimum
        effectiveDuration = (IN_animationDurationMS > minTime) ? IN_animationDurationMS : minTime;
        AnimationDurationEndTimerFlag = milliTimeCopy + effectiveDuration;
        
        AnimationIsRunning = true;

    // --- KOMPLETTER LOGIK-RESET ---
    keyTimerFlagFrontLeft = 0;   // Timer auf 0 setzen, damit keine Zeitdifferenz berechnet wird
    flipFlopFlagFrontLeft = 0;   // Flag auf 0, damit der Release-Zweig blockiert ist
    secondKeyButtonTimeMark = 0; // Zeitmarke löschen
    secondKeySetLaterRelease = false; // Geplante Releases abbrechen
    //AnimationIsRunning = 0;
    }
}





// Einstellungen & Makros für den BMI160-Sensor
#define MAX_VOLATILE(a, b) ((a) > (b) ? (a) : (b))
#define BMI160_ADDR 0x69
#define I2C_SDA 21
#define I2C_SCL 22
#define TASK_PRIORITY 5
#define TASK_STACK 4096
#define WINDOW_SIZE  10
int THRESHOLD_X =  1000;                                      
int THRESHOLD_Y =   800;
int THRESHOLD_Z =  2000;
int tiltGain    =    12;                                     // NUDGE-Tilt sensibilitäts Multiplikator. um ca 10 is mittel empfindlich. 
int tiltTresholdMenuX = THRESHOLD_X / 100;                   // *100 intern rechnen, im menu haben wir so kleinere zahlen
int tiltTresholdMenuY = THRESHOLD_Y / 100;                   // *100 intern rechnen, im menu haben wir so kleinere zahlen
float pitch, roll;
// TODO: tiltLimiter
int tiltLimiter   = 10;                                      // Limits nudge power to a limit, to prevent tilt table // intern *100

#define RAW_TO_G(raw) ((float)raw / 2048.0f)                 // ±16G (32768/16=2048)
//#define GYRO_TO_DPS(raw) ((float)raw * (250.0f / 32768.0f))  // 250DPS
#define GYRO_TO_DPS(raw) ((float)raw * 0.0076293945f)        // 250.0f / 32768.0f


volatile struct {  
    // Accel (für Schlagerkennung)
    int16_t left, right, up;
    uint8_t cnt_left, cnt_right, cnt_up;
    
    // Rohdaten für Winkelberechnung
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
  } power = {0};
  
  SemaphoreHandle_t power_mutex;
  
  // Sensor-Task
  void sensorTask(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1) {
      // Accel-Daten lesen (Register 0x12)
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x12);
      Wire.endTransmission(false);
      Wire.requestFrom(BMI160_ADDR, 6);
      if(Wire.available() == 6) {
        int16_t ax = Wire.read() | (Wire.read() << 8);
        int16_t ay = Wire.read() | (Wire.read() << 8);
        int16_t az = Wire.read() | (Wire.read() << 8);
        
        xSemaphoreTake(power_mutex, portMAX_DELAY);
        
        // Schlagerkennung. Sobald Treshold überschritten wird, wird werden 10 samples gezogen und der peak wert genommen. somit 10ms latenz
        if(ax < -THRESHOLD_X) {
          if(power.cnt_left == 0) power.left = 0;
          power.left = MAX_VOLATILE(power.left, abs(ax));
          power.cnt_left = WINDOW_SIZE;
        }
        else if(ax > THRESHOLD_X) {
          if(power.cnt_right == 0) power.right = 0;
          power.right = MAX_VOLATILE(power.right, ax);
          power.cnt_right = WINDOW_SIZE;
        }
        if(ay > THRESHOLD_Y) {
          if(power.cnt_up == 0) power.up = 0;
          power.up = MAX_VOLATILE(power.up, abs(ay));
          power.cnt_up = WINDOW_SIZE;
        }
  
        // Rohdaten speichern (für Winkel)
        power.accel_x = ax;
        power.accel_y = ay;
        power.accel_z = az;
  
        // Counter aktualisieren, immer richtung 0
        if(power.cnt_left) power.cnt_left--;
        if(power.cnt_right) power.cnt_right--;
        if(power.cnt_up) power.cnt_up--;
  
        xSemaphoreGive(power_mutex);
      }
  
      // Gyro-Daten lesen (Register 0x0C)
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x0C);
      Wire.endTransmission(false);
      Wire.requestFrom(BMI160_ADDR, 6);
      if(Wire.available() == 6) {
        xSemaphoreTake(power_mutex, portMAX_DELAY);
        power.gyro_x = Wire.read() | (Wire.read() << 8);
        power.gyro_y = Wire.read() | (Wire.read() << 8);
        power.gyro_z = Wire.read() | (Wire.read() << 8);
        
        xSemaphoreGive(power_mutex);
      }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
  }   // END of BMI160


//Touch Sensor & display
#define TOUCH_SDA 33                                  // I2C SDA-Pin
#define TOUCH_SCL 32                                  // I2C SCL-Pin
#define TOUCH_INT -1                                  // Interrupt-Pin (falls nicht verwendet, auf -1 setzen)
#define TOUCH_RST 25                                  // Reset-Pin
#define GT911_I2C_ADDR 0x5D                           // Standard-Adresse des GT911
#define SCREEN_WIDTH  240                             // Korrekte Breite des Touchscreens
#define SCREEN_HEIGHT 320                             // Korrekte Höhe des Touchscreens

#define BACKLIGHT_PIN  27                             // PWM-Pin für Backlight
#define PWM_CHANNEL    0                              // LEDC-Kanal (0-15)
#define PWM_RESOLUTION 8                              // 8-Bit-Auflösung (0-255)
#define PWM_FREQUENCY  5000                           // Frequenz in Hz

#define IO_PIN_ADC_BATTERY 34                         // Cut PCB! Solder from, this pin a 100k to GND and a 56k resistor to Battery +

int PWM_POWER      = 100;                             // startwert und auch zielwert global gespeichert erstes einschalten. TODO: speicherbar machen 5-255. nicht zu dunkel....

// debug Benchmark variablen 
uint32_t loopCounter       = 0;
uint32_t loopsPerSecond    = 0;
uint32_t lastLoopTime      = 0;
uint32_t lastSecondTime    = 0;
unsigned long loopStartTime     = 0;
unsigned long lastLoopStartTime = 0; 

// counter. TODO: laden und speicher auf chip oder NVS
uint32_t ButtonFlipperLeftCounterToday    =   0;      // counter
uint32_t ButtonFlipperRightCounterToday   =   0;      // 0 = nicht gedrückt, 1 = gedrückt
uint32_t ButtonFlipperLeftCounterAlltime  = 100;      // 0 = nicht gedrückt, 1 = gedrückt 
uint32_t ButtonFlipperRightCounterAlltime = 100;      // 0 = nicht gedrückt, 1 = gedrückt




int  gamepadXfinal            =   0 ;
int  gamepadYfinal            =   0 ;


// bilde instanzen der display klassen
// ========================================================================== //
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
GUI* ui = nullptr;



// Zweiter I2C-Bus für GT911 Sensor
TwoWire I2C_1(1);
GT911 touch(&I2C_1);  // GT911 am zweiten Bus

bool isGT911Connected() {
    I2C_1.beginTransmission(GT911_I2C_ADDR);
    return (I2C_1.endTransmission() == 0);
}



// Plunger links (meta taste B)
// input Mode 0 = release, 1 set
void sendBTcommandPlungerLinks(bool inputMode){   // diese taste zuerst abfragen für multi button 500ms trick
    if(dbglvl) Serial.printf("[%lu.%03lu] sendBTcommandPlungerLinks(bool inputMode) called- set report flag =true\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->press(BUTTON_2);           gamepadSendReportFlag   = true;  break;  // [verified] quest 
            case 2: hid->gamepad->press(BUTTON_3);           gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->press(BUTTON_1);           gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_1);           gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_I);          keyboardSendReportFlag  = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_2);           gamepadSendReportFlag   = true;  break;  // [verified] quest star wars pinball different keymap
           default: break;                                                                            // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->release(BUTTON_2);         gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_3);         gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->release(BUTTON_1);         gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_1);         gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_I);        keyboardSendReportFlag  = true;  break;  // pinballFX 2025
            case 6: hid->gamepad->release(BUTTON_2);         gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                            // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// 0 = release, 1 set
void sendBTcommandPlungerRechts(bool inputMode){   
  
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
                  switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
                    //case 0: gamepad.press(dbgGamePad); break;// debug BUTTON_1-128 // wenn dbgGamePad, kann über debug menu taste zum testen gesetzt werden
                    case 1: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  // [verified][A] quest 
                    case 2: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  // android 4 = (X)
                    case 3: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  // pc
                    case 4: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  // iphone
                    case 5: hid->keyboard->keyPress(KEY_8);   keyboardSendReportFlag  = true;  break;  // switch
                    case 6: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
                   default: break;                                                                     // wird aufgerufen falls kein case getroffen wurde
                }
            }
        else{
                switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
                    //case 0: gamepad.release(dbgGamePad); break;
                    case 1: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  // [release] quest
                    case 2: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  // android 4 = (X)
                    case 3: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  // pc
                    case 4: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  // iphone
                    case 5: hid->keyboard->keyRelease(KEY_8); keyboardSendReportFlag  = true;  break;  // keyboard for pinballFX
                    case 6: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  // [release] quest star wars pinball different keymap
                   default: break;                                                                     // wird aufgerufen falls kein case getroffen wurde
                }
            }
}



// 0 = release, 1 set
void sendBTcommandPlungerRechtsSecondKey(bool inputMode){   
    if(dbglvl) Serial.printf("[%lu.%03lu] sendBTcommandPlungerRechtsSecondKey called- set report flag =true\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
                //if(dbglvl >6 )Serial.println("sendBTcommandPlungerRechtsSecondKey()"); // debug
                switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
                    case 1: hid->gamepad->press(BUTTON_11);    gamepadSendReportFlag   = true;  break;  // [verified] quest  back, at table toggle menu on off
                    case 2: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // android 4 = (X)
                    case 3: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // pc
                    case 4: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // iphone
                    case 5: hid->gamepad->press(BUTTON_2);     gamepadSendReportFlag   = true;  break;  // switch
                    case 6: hid->gamepad->press(BUTTON_11);    gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap oder im game key bindings ändern, wird dann HAT pressed
                   default: break;                                                                      // wird aufgerufen falls kein case getroffen wurde
                }
            }
        else{
                switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
                    case 1: hid->gamepad->release(BUTTON_11);  gamepadSendReportFlag   = true;  break;  // quest
                    case 2: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  // android 4 = (X)
                    case 3: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  // pc
                    case 4: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  // iphone
                    case 5: hid->gamepad->release(BUTTON_2);   gamepadSendReportFlag   = true;  break;  // switch
                    case 6: hid->gamepad->release(BUTTON_11);  gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
                   default: break;                                                                      // wird aufgerufen falls kein case getroffen wurde
                }
            }
}



// input Mode 0 = release, 1 set
void sendBTcommandFlipperLinks(bool inputMode){   
    
    // wenn emulationModeOverride >0 ist, soll der emulationsmode 1 bis 6 sein.
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){  // 1 set
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  // [verified] quest
            case 2: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_U);      keyboardSendReportFlag  = true;  break;  // keyboard "A"
            case 6: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  // 9 quest star wars pinball different keymap [verified]
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{           // 0 release
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_U);    keyboardSendReportFlag  = true;  break;  // switch
            case 6: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  // 9 quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// input Mode 0 = release, 1 set
void sendBTcommandFlipperLinksSecondKey(bool inputMode){   
    
    // wenn emulationModeOverride >0 ist, soll der emulationsmode 1 bis 6 sein.
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){  // 1 set
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setRZ(32767);           gamepadSendReportFlag   = true;  break;  // [verified] quest
            case 2: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_15);       gamepadSendReportFlag   = true;  break;  // [BUTTON_15]+[R-HAT-PRESS][recenter view] (muss in starwars pinball auf quest in optionen auf HAT umgestellt werden!)
           default: break;                                                                         // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{           // 0 release
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setRZ(0);               gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->release(BUTTON_15);     gamepadSendReportFlag   = true;  break;  // [BUTTON_15]+[R-HAT-release][quest star wars pinball different keymap
           default: break;                                                                         // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// 0 = release, 1 set
void sendBTcommandFlipperRechts(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 

    // if(dbgGamePad > 0) useMode = 0;  // wenn im debug menu die zahl größer 1 ist, überschreibe pin mit zahl aus UI // debug mode UM TASTEN DURCHZUPROBIEREN
    // if(dbglvl  > 0 )Serial.printf("sendBTcommandFlipperRechts(inputMode:%d) useMode:%d, dbgGamepad:%d\n",inputMode,useMode,dbgGamePad); // debug
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            //case 0: gamepad->press(dbgGamePad);  break;  // debug BUTTON_1-128 // wenn dbgGamePad, kann über debug menu rechte flipper taste zum testen gesetzt werden
            case 1: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  // [verified] quest
            case 2: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_6);       keyboardSendReportFlag  = true;  break;  // keyboard for pinballFX
            case 6: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  // [8][10] [verified] quest star wars pinball different keymap
           default: break;                                                                         // wird aufgerufen falls kein case getroffen wurde 
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            //case 0: gamepad->release(dbgGamePad);break;
            case 1: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_6);     keyboardSendReportFlag  = true;  break;  // keyboard for pinballFX
            case 6: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  // [8][10] [release]] star wars pinball different keymap
           default: break;                                                                         // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// 0 = release, 1 set
void sendBTcommandFlipperRechtsSecondKey(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 

    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            //case 1: gamepadXfinal = -32767;           gamepadSendReportFlag   = true;  break;   // [verified] quest setRZ(32767);
            case 1: hid->gamepad->setZ(32767);           gamepadSendReportFlag   = true;  break;  // [verified] quest setRZ(32767);
            case 2: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setZ(0);               gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



//////////////////////// NUDGE //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NUDGE UP , 1 = set, 0 release, optional analog input 0-32737 for analog joystick profiles 
void sendBTcommandTiltFront(bool inputMode, int analogValue ){ 
        //if (dbglvl > 0) {Serial.printf("Nudge TiltFront:\n");}
        
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        if(inputMode){
            switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
                // case 1: gamepad->setLeftThumb(0,-analogValue); gamepadSendReportFlag   = true;  break; // [verified] quest WICHTIG "-"
                // case 1: gamepadYfinal =-analogValue;                gamepadSendReportFlag   = true;  break;  // [verified] quest WICHTIG "-" (different syntax to set y axis only!)
                case 1: hid->gamepad->setLeftThumb(hid->gamepad->lx ,-analogValue);   gamepadSendReportFlag   = true;  break;  // [verified] quest WICHTIG "-" (different syntax to set y axis only!)
                case 2: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  // android
                case 3: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  // pc
                case 4: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  // iphone
                case 5: hid->keyboard->keyPress(KEY_A);                               keyboardSendReportFlag  = true;  break;  // Keyboard Nudge A-up oder S-down testen
                case 6: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
               default: break;                                                                                                 // wird aufgerufen falls kein case getroffen wurde
            }
        }
        else{
            switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
                // case 1: gamepad->setLeftThumb(0,0);            gamepadSendReportFlag   = true;  break;  // [release] quest
                case 1: hid->gamepad->setLeftThumb(hid->gamepad->lx , 0);             gamepadSendReportFlag   = true;  break;  // [release] quest (different syntax to set y axis only!)
                case 2: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  // android
                case 3: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  // pc
                case 4: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  // iphone
                case 5: hid->keyboard->keyRelease(KEY_A);                             keyboardSendReportFlag  = true;  break;  // Keyboard Nudge A-up oder S-down testen
                case 6: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
               default: break;                                                                                                 // wird aufgerufen falls kein case getroffen wurde 
            }
        }
    }
    


// NUDGE LEFT , 1 = set, 0 release, optional analog input 0-32737 for analog joystick profiles 
void sendBTcommandTiltLeft(bool inputMode, int analogValue){ 
   
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        //if(dbglvl)Serial.printf("sendBTcommandTiltLeft(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values.    
        if(inputMode){
            switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
                // case 1: gamepad->setLeftThumb(analogValue,0);   gamepadSendReportFlag   = true;  break;  // [verified] quest  gamepadSendReportFlag   = false; kann eigentlich raus
                case 1: hid->gamepad->setLeftThumb(analogValue, hid->gamepad->ly);    gamepadSendReportFlag   = true; break;  // [verified] quest  gamepadSendReportFlag (different syntax to set x axis only!)
                case 2: hid->gamepad->setLeftThumb(-analogValue,0);                   gamepadSendReportFlag   = true; break;  // android
                case 3: hid->gamepad->setLeftThumb(0,-analogValue);                   gamepadSendReportFlag   = true; break;  // pc
                case 4: hid->gamepad->setLeftThumb(0,-analogValue);                   gamepadSendReportFlag   = true; break;  // iphone
                case 5: hid->keyboard->keyPress(KEY_F);                               keyboardSendReportFlag  = true; break;  // PinballFX 2025
                case 6: hid->gamepad->setLeftThumb(analogValue,0);                    gamepadSendReportFlag   = true; break;  // quest star wars pinball different keymap
               default: break;                                                                                                // wird aufgerufen falls kein case getroffen wurde
            }
        }
        else{
            switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
                case 1: hid->gamepad->setLeftThumb(0 , hid->gamepad->ly);             gamepadSendReportFlag   = true; break;  // [release] quest (different syntax to set x axis only!)
                case 2: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  // android
                case 3: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  // pc
                case 4: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  // iphone
                case 5: hid->keyboard->keyRelease(KEY_F);                             keyboardSendReportFlag  = true; break;  // switch   keyboard->keyRelease(KEY_F);
                case 6: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  // quest star wars pinball different keymap
               default: break;                                                                                                // wird aufgerufen falls kein case getroffen wurde
            }
        }
}
  


// NUDGE RIGHT , 1 = set, 0 release, optional analog input 0-32737 for analog joystick profiles 
void sendBTcommandTiltRight(bool inputMode, int analogValue){ // nudge/side bump
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    //if(dbglvl)Serial.printf("sendBTcommandTiltRight(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setLeftThumb(-analogValue, hid->gamepad->ly);      gamepadSendReportFlag   = true; break;  // [verified] quest (different syntax to set x axis only!)
            case 2: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  // android
            case 3: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  // pc
            case 4: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_D);                                  keyboardSendReportFlag  = true; break;  // keyboard D for pinballFX
            case 6: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  // quest star wars pinball different keymap
           default: break;                                                                                                   // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setLeftThumb(0, hid->gamepad->ly);                 gamepadSendReportFlag   = true; break;  // [release] quest (different syntax to set x axis only!) 
            case 2: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  // android
            case 3: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  // pc
            case 4: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_D);                                keyboardSendReportFlag  = true; break;  // keyboard D for pinballFX
            case 6: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  // quest star wars pinball different keymap
           default: break;                                                                                                   // wird aufgerufen falls kein case getroffen wurde
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 40 degrees angle based virtual buttons to bluetooth//////////////////////////////////////////////////////////////////////////////////////////////////////
// 1 = set, 0 release , optional analogValue 0-32767 for joystick 
void sendBTcommandAngleTiltButtonLeft(bool inputMode, int analogValue = 0){ 
        
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        if(dbglvl  > 0 )Serial.printf("sendBTcommandAngleTiltButtonLeft(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
        
        if(inputMode){
            switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
                // setHat() 0 = up, 2 = Right, 4 = down, 6 = left, 8 = NEUTRAL!!  //diagonal: Up‑Right, 3 = Down‑Right, 5 = Down‑Left, 7 = Up‑Left
                case 1: hid->gamepad->setHat(6);                      gamepadSendReportFlag   = true;  break; // [verified] quest
                case 2: hid->gamepad->setLeftThumb(-analogValue,0);   gamepadSendReportFlag   = true;  break; // android
                case 3: hid->gamepad->press(BUTTON_1);                gamepadSendReportFlag   = true;  break; // pc
                case 4: hid->gamepad->press(BUTTON_1);                gamepadSendReportFlag   = true;  break; // iphone
                case 5: hid->gamepad->press(BUTTON_2);                gamepadSendReportFlag   = true;  break; // switch
                case 6: hid->gamepad->setLeftThumb(-analogValue,0);   gamepadSendReportFlag   = true;  break; // quest star wars pinball different keymap
               default: break;                                                                                // wird aufgerufen falls kein case getroffen wurde
            }
        }
        else{
            switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
                case 1: hid->gamepad->setHat(8);                      gamepadSendReportFlag   = true;  break;  // [release] quest 
                case 2: hid->gamepad->setLeftThumb(0,0);              gamepadSendReportFlag   = true;  break;  // android
                case 3: hid->gamepad->release(BUTTON_1);              gamepadSendReportFlag   = true;  break;  // pc
                case 4: hid->gamepad->release(BUTTON_1);              gamepadSendReportFlag   = true;  break;  // iphone
                case 5: hid->gamepad->release(BUTTON_2);              gamepadSendReportFlag   = true;  break;  // keyboard pinball fx 2025
                case 6: hid->gamepad->release(BUTTON_2);              gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
               default: break;                                                                                 // wird aufgerufen falls kein case getroffen wurde
            }
        }
    }



// 40 degrees angle based virtual buttons to bluetooth
// 1 = set, 0 release , optional analogValue 0-32767 for joystick
void sendBTcommandAngleTiltButtonRight(bool inputMode, int analogValue = 0){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(dbglvl  > 0 )Serial.printf("sendBTcommandAngleTiltButtonRight(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(2);                          gamepadSendReportFlag   = true;  break;  // [verified] android
            case 2: hid->gamepad->setLeftThumb(analogValue,0);        gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->press(BUTTON_1);                    gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_1);                    gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->press(BUTTON_2);                    gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_2);                    gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                     // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(8);                          gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->release(BUTTON_1);                  gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_1);                  gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->release(BUTTON_2);                  gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->release(BUTTON_2);                  gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                     // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// 40 degrees angle based virtual buttons to bluetooth
// 1 = set, 0 release , optional analogValue 0-32767 for joystick
void sendBTcommandAngleTiltButtonUp(bool inputMode, int analogValue = 0){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    //if(dbglvl  > 0 )Serial.printf("sendBTcommandAngleTiltButtonUp(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values.   
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(0);                          gamepadSendReportFlag   = true;  break;  // [verified] quest
            case 2: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                     // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(8);                          gamepadSendReportFlag   = true;  break;  // [release] quest  release, set d-pad to neutral (0x08)
            case 2: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                     // wird aufgerufen falls kein case getroffen wurde
        }
    }
}



// 40 degrees angle based virtual buttons to bluetooth
// 1 = set, 0 release , optional analogValue 0-32767 for joystick
void sendBTcommandAngleTiltButtonDown(bool inputMode, int analogValue = 0){ // 40 degree angle tilt/pitch/roll
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(4);                           gamepadSendReportFlag   = true;  break;  // [verified] quest
            case 2: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                      // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->setHat(8);                           gamepadSendReportFlag   = true;  break;  // [release] quest  release, set d-pad to neutral (0x08)
            case 2: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  // android
            case 3: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  // switch
            case 6: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                                      // wird aufgerufen falls kein case getroffen wurde
        }
    }
}

// Action Key (ist normal X-Key auf dem meta controller) ich verwende ihn z.b. für den funkbutton
// input Mode 0 = release, 1 set
void sendBTcommandActionKey(bool inputMode){   
    if(dbglvl) Serial.printf("[%lu.%03lu] sendBTcommandActionKey called- set report flag =true\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  // [verified] quest 
            case 2: hid->gamepad->press(BUTTON_3);       gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_5);      keyboardSendReportFlag  = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  // [verified] quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_3);     gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_5);    keyboardSendReportFlag  = true;  break;  // pinballFX 2025
            case 6: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
}

void sendBTcommandActionKeySecondKey(bool inputMode){   
    if(dbglvl) Serial.printf("[%lu.%03lu] sendBTcommandActionKeySecondKey called- set report flag =true\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { // SEND ACTIVE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  // [verified] quest 
            case 2: hid->gamepad->press(BUTTON_3);       gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyPress(KEY_5);      keyboardSendReportFlag  = true;  break;  // switch
            case 6: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  // [verified] quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
    else{
        switch (useMode) { // SEND RELEASE, abhängig von globaler variable: eumulationMode
            case 1: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  // [release] quest
            case 2: hid->gamepad->release(BUTTON_3);     gamepadSendReportFlag   = true;  break;  // android 5 = (Y)
            case 3: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // pc
            case 4: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  // iphone
            case 5: hid->keyboard->keyRelease(KEY_5);    keyboardSendReportFlag  = true;  break;  // pinballFX 2025
            case 6: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  // quest star wars pinball different keymap
           default: break;                                                                        // wird aufgerufen falls kein case getroffen wurde
        }
    }
}




// ESPNOW foot pedal datareceiver
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Quick and dirty.... Nur akzeptieren, wenn Länge exakt passt
  // TODO: bestätigungs system mit time stamps
  // batterie zustand übertragen alle 20-30 sekunden, falls es nicht wie bei BT teil des reports ist.

  if (len == sizeof(KeyEvent)) {
    KeyEvent ev;
    memcpy(&ev, incomingData, sizeof(ev));

    if (ev.state <= 1) {  // nur gültige Werte
      espnowAirButtonCurrentState = ev.state;  // refresh global
      //if(dbglvl) Serial.print("Neuer Zustand empfangen: ");
      //if(dbglvl) Serial.println(espnowAirButtonCurrentState);
      
      //if(espnowAirButtonCurrentState == 0){ sendBTcommandActionKey(0); }  // button nicht gesetzt ui->espnowButton(3);
      //if(espnowAirButtonCurrentState == 1){ sendBTcommandActionKey(1); }  // button gesetzt ui->espnowButton(2);
      
      // if(espnowAirButtonCurrentState == 255) if(dbglvl)Serial.println("Ping von Funkgame Controller empfangen");

    }
  } else {
    // if(dbglvl>9){
    // // Serial.printf("Unerwartetes Paket empfangen: len=%d\n", len);
    // for (int i=0; i<len; i++) {
    //   Serial.printf("%02X ", incomingData[i]);
    // }
    // Serial.println();
    // }
}
}

void serialWelcomeMessage(){
                            Serial.println(F("  ___  ___ _ _ _ ___ ___  ___ "));
                            Serial.println(F(" | _ \\| _ ) | | |_  |_  || __|"));
                            Serial.println(F(" |  _/| _ \\ | | |/ / / / |__ \\"));
                            Serial.println(F(" |_|  |___/\\___//___/___/|___/"));
                            Serial.println("");
                            Serial.println("PINBAL WIZARD 25");
                            Serial.println("(C)2020-2026 by VR-addicted");
}

// =========================================================================================================================================================== //
//     SSSS     EEEEEEE   TTTTTTT   UU   UU   PPPPPP   
//    SS   S    EE          TT      UU   UU   PP   PP 
//    SS        EE          TT      UU   UU   PP   PP  
//     SSSSS    EEEEE       TT      UU   IU   PPPPPP   
//         SS   EE          TT      UU   UU   PP     
//    S    SS   EE          TT      UU   UU   PP  
//     SSSSS    EEEEEEE     TT       UUUUU    PP   
// =========================================================================================================================================================== //

void setup() {

    

    Serial.begin(115200);  
    serialWelcomeMessage(); 
    if(!dbglvl){


    }            

    hid = new GamepadKeyboardHID(DEVICE_NAME, DEVICE_MANUFACTURER);
    
    adc1_config_width(ADC_WIDTH_BIT_12);                            // 0..4095
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);     // Spannung 3.3 

    if(dbglvl) Serial.println("RGB strip.Begin()");
    strip.Begin(); 
    RGBall(40);                                                     // setze rgb leds auf schwaches weiß
 


// ======================================================================================================= //
// BBBBB    LL        EEEEEEE      CCCC   OOOOO   MM   MM  PPPPPP   OOOOO   SSSSS   IIII  TTTTTTT  EEEEEEE
// BB   BB  LL        EE          CC   C OO   OO  MMM MMM  PP   PP OO   OO SS        II     TT     EE
// BBBBBB   LL        EEEEE       CC     OO   OO  MM M MM  PPPPPP  OO   OO  SSSSS    II     TT     EEEEE
// BB   BB  LL        EE          CC   C OO   OO  MM   MM  PP      OO   OO      SS   II     TT     EE
// BBBBBB   LLLLLLLL  EEEEEEE      CCCC   OOOOO   MM   MM  PP       OOOOO   SSSSS   IIII    TT     EEEEEEE
  
  if(dbglvl){ 
            uint8_t myMAC[6];
            esp_read_mac(myMAC, ESP_MAC_WIFI_STA);
            Serial.printf("esp_read_mac(): %02X:%02X:%02X:%02X:%02X:%02X\n",
                            myMAC[0], myMAC[1], myMAC[2], myMAC[3], myMAC[4], myMAC[5]);
            }

// Station-Mode aktivieren (Pflicht für ESP-NOW!)
WiFi.mode(WIFI_STA);


// Eigentlich überflüssig, alternative methode die eigene mac zu lesen. 
if(dbglvl){
            uint8_t macSTA[6];
            esp_wifi_get_mac(WIFI_IF_STA, macSTA);
            Serial.printf("esp_wifi_get_mac(): %02X:%02X:%02X:%02X:%02X:%02X\n",
            macSTA[0], macSTA[1], macSTA[2], macSTA[3], macSTA[4], macSTA[5]);
}
WiFi.disconnect(); 

// TODO END

  // Jetzt ESP-NOW starten
  if (esp_now_init() != ESP_OK) {
    if(dbglvl) Serial.println("ESP-NOW Init fehlgeschlagen");
    return;
  }
  if(dbglvl) Serial.println("ESP-NOW initialisiert");

  // Callback für Empfang registrieren
  esp_now_register_recv_cb(OnDataRecv);

  // Startzustand: keine Verbindung
  wasConnected = false;
  lastPacketTime = millis();


// Beispiel: Peer hinzufügen (nur nötig, wenn Empfänger auch senden soll)
// im setup(), nach esp_now_init() und vor dem Ende:
esp_now_peer_info_t peerInfo = {};
memcpy(peerInfo.peer_addr, senderMac, 6);
peerInfo.channel = 0;
peerInfo.encrypt = false;

if (esp_now_add_peer(&peerInfo) != ESP_OK) {
  if(dbglvl) Serial.println("Fehler: Peer konnte nicht hinzugefügt werden");

} else {
  if(dbglvl) Serial.println("Peer hinzugefügt (Sender)");

}



// ###############################################################################################


    // WICHTIG: Zuerst Display und UI initialisieren, bevor BLE gestartet wird
    if(dbglvl>1) Serial.println("Initializing I2C...");
    I2C_1.begin(TOUCH_SDA, TOUCH_SCL, 400000);

    
    if(dbglvl>1) Serial.println("Initializing TFT...");
    tft.init();
    

    ui = new GUI(tft, hid->gamepad, hid->keyboard);
    

    // Touch-Controller Setup NACH dem UI
    if(dbglvl>1) Serial.println("Checking GT911 connection...");
    if (isGT911Connected()) {
        if(dbglvl>1) {
            Serial.println("GT911 detected on I2C bus.");
            Serial.println("Initializing GT911...");
        }
        if (touch.begin(TOUCH_INT, TOUCH_RST, GT911_I2C_ADDR, 100000)) {
            if(dbglvl>1) Serial.println("GT911 initialized successfully.");
            touch.setRotation(GT911::Rotate::_0);
            
        } else {
            if(dbglvl>1) Serial.println("ERROR: GT911 initialization failed!");
        }
    } else {
        if(dbglvl>1) Serial.println("ERROR: GT911 not found on I2C bus!");
    }
    

    // // Sensor Setup
    // if(dbglvl>1) Serial.println("Initializing Gyro Sensor...");
      
    //   Wire.begin(I2C_SDA, I2C_SCL, 1000000);
    //   delay(50);

    //   // BMI160 Initialisierung 
    //   // 1. Hardware-Reset (optional, aber sauber), falls man im programm noch mal neu kalibrieren will, vllt nützlich
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x7E);
    //   Wire.write(0xB6); // Softreset
    //   Wire.endTransmission();
    //   delay(50);
    
    //   Wire.beginTransmission(BMI160_ADDR);     // SetAccelFilter first!!! TIEFPASSFILTER INTERNAL in BMI160
    //   Wire.write(0x40);                        // Accel Config Register
    //   Wire.write(0b00001011);                  // ODR=200Hz, BW=0b1011 (OSR4, ~200Hz Cutoff) //BW=0b1100 (NORMAL_AVG4, ~62.5Hz Cutoff),0b1000: OSR4 (höchste Bandbreite), 0b1111: OSR2 (stärkste Filterung)
    //   Wire.endTransmission();
    //   delay(50);
      
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x7E);     // CMD-Register
    //   Wire.write(0x11);     // Accel normal mode
    //   Wire.endTransmission();
    //   delay(50);
    
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x7E); 
    //   Wire.write(0x15);     // Gyro normal mode
    //   Wire.endTransmission();
    //   delay(50);
      
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x41);
    //   Wire.write(0x0F);     // Accel-Range ±16G (Register 0x41: 0x0F)
    //   Wire.endTransmission();
    //   delay(50);
      
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x43);
    //   Wire.write(0x00);     // Gyro-Range 250DPS (Register 0x43: 0x00)
    //   Wire.endTransmission();
    //   delay(50);
    
    //   // Auto-Kalibrierung (Accel)
    //   Wire.beginTransmission(BMI160_ADDR);
    //   Wire.write(0x7E);
    //   Wire.write(0x37);     // Accel Offset Kalibrierung
    //   Wire.endTransmission();
    //   delay(500);
    
    // power_mutex = xSemaphoreCreateMutex();   // Multitask Mutex für den Sensor
    // xTaskCreatePinnedToCore(
    //   sensorTask,
    //   "SensorTask",
    //   TASK_STACK,
    //   NULL,
    //   TASK_PRIORITY,
    //   NULL,
    //   0
    // );
    
    //   if(dbglvl>1) Serial.println("Initialisierung abgeschlossen");
   
     
    // UI mit Gamepad verknüpfen
    ui->begin();           // Jetzt ist ui initialisiert
    // ui->setTouch(&touch);  

    // ui->setGamepad(gamepad);
     ui->setGamepad(hid->gamepad);


    if(dbglvl>1) Serial.println("Initializing PWM...");
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(BACKLIGHT_PIN, PWM_CHANNEL);
    
    // Backlight einschalten  (macht intro() mit einem softstart sonnenaufgang)
    if(dbglvl>1) Serial.println("Showing intro...");
    ui->intro();  // hier in die warteschleife rgb rainbow animation
    
    



// Sensor Setup
    if(dbglvl>1) Serial.println("Initializing Gyro Sensor...");
      
      Wire.begin(I2C_SDA, I2C_SCL, 1000000);
      //delay(150);  

// TEST Anstatt delay Starte die Animation mit den gewünschten Parametern
RGB_animation(1, 1000, 150); 
delay(2000);
// Halte das Setup hier fest, bis die Animation durchgelaufen ist, lass aber die andere cpu ihren dienst machen. deshalb kein delay

// while(AnimationIsRunning) {
//     milliTimeCopy = millis(); // WICHTIG: Zeit für die Logik aktualisieren
//     RGB_animation();          // Frame berechnen und strip.Show() ausführen
//     delay(1);                 // Kleines Delay für den Watchdog-Timer (WDT)
// }



      // BMI160 Initialisierung 
      // 1. Hardware-Reset (optional, aber sauber), falls man im programm noch mal neu kalibrieren will, vllt nützlich
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);
      Wire.write(0xB6); // Softreset
      Wire.endTransmission();
      delay(50);
    
      Wire.beginTransmission(BMI160_ADDR);     // SetAccelFilter first!!! TIEFPASSFILTER INTERNAL in BMI160
      Wire.write(0x40);                        // Accel Config Register
      Wire.write(0b00001011);                  // ODR=200Hz, BW=0b1011 (OSR4, ~200Hz Cutoff) //BW=0b1100 (NORMAL_AVG4, ~62.5Hz Cutoff),0b1000: OSR4 (höchste Bandbreite), 0b1111: OSR2 (stärkste Filterung)
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);     // CMD-Register
      Wire.write(0x11);     // Accel normal mode
      Wire.endTransmission();
      delay(50);
    
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E); 
      Wire.write(0x15);     // Gyro normal mode
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x41);
      Wire.write(0x0F);     // Accel-Range ±16G (Register 0x41: 0x0F)
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x43);
      Wire.write(0x00);     // Gyro-Range 250DPS (Register 0x43: 0x00)
      Wire.endTransmission();
      delay(50);
    
      // Auto-Kalibrierung (Accel)
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);
      Wire.write(0x37);     // Accel Offset Kalibrierung
      Wire.endTransmission();
      delay(50);
    
    power_mutex = xSemaphoreCreateMutex();   // Multitask Mutex für den Sensor
    xTaskCreatePinnedToCore(
      sensorTask,
      "SensorTask",
      TASK_STACK,
      NULL,
      TASK_PRIORITY,
      NULL,
      0
    );
    
      if(dbglvl>1) Serial.println("BMI160 Initialisierung abgeschlossen");



// ========================================================================== //
   
    // Mechanical Switches soldered to IO0,IO4,IO16,IO17 
    // pinMode(ioPinSideLeft,  INPUT_PULLUP); // set internal pullup. if resistor is used, change to "INPUT"
    // pinMode(ioPinSideRight, INPUT_PULLUP); // set internal pullup. if resistor is used, change to "INPUT"
    // pinMode(ioPinFrontLeft,  INPUT_PULLUP);       // b-key gpio0 fix! shared with boot mode, easy to flash with
    // pinMode(ioPinFrontRight, INPUT);       // a-Key gpio4 theoretisch geht auch INPUT_PULLUP, aber wake up from sleep geht nur mit 47k pull resistor soldered for deep sleep wakeup
    // pinMode(ioPinSideX,      INPUT);        // an IO35 muss ein 10k-50k pullup widerstand angelötet werden 3.3v 
    
     // Mechanical Switches soldered to                                OLD: IO0,IO4,IO16,IO17 
    pinMode(ioPinSideLeft,   INPUT);       // set internal pullup. if resistor is used, change to "INPUT"
    pinMode(ioPinSideRight,  INPUT);      // set internal pullup. if resistor is used, change to "INPUT"
    pinMode(ioPinFrontLeft,  INPUT);       // b-key gpio0 fix! shared with boot mode, easy to flash with
    pinMode(ioPinFrontRight, INPUT);       // a-Key gpio4 theoretisch geht auch INPUT_PULLUP, aber wake up from sleep geht nur mit 47k pull resistor soldered for deep sleep wakeup
    pinMode(ioPinSideX,      INPUT);        // an IO35 muss ein 10k-50k pullup widerstand angelötet werden 3.3v 
    
    
    // PCB Power management                // Deep Sleep Wake-Up bei fallender Flanke, also wenn linker plunger gedrückt wird.
    // TODO: statt deep sleep 2x die leitung des batterie management chips auf ground ziehen, für endgültige abschaltung
    esp_sleep_enable_ext0_wakeup((gpio_num_t) ioPinFrontLeft, 0);  // hängt an gpio0, besser an ioPinFrontRight. mit 47k. verbraucht weniger strom der hängt an 10k pullup. somit checke high to low
    RGBbaseLight();
    if(dbglvl>1) Serial.println("Setup complete.");


}

//============================================================================================================================ //   
//   LLL         OOOOOO      OOOOOO     PPPPPPPP
//   LLL        OO    OO    OO    OO    PPP    PP
//   LLL        OO    OO    OO    OO    PPP    PP
//   LLL        OO    OO    OO    OO    PPPPPPP
//   LLL        OO    OO    OO    OO    PPP
//   LLL        OO    OO    OO    OO    PPP 
//   LLL        OO    OO    OO    OO    PPP
//   MAINLLLL    OOOOOO      OOOOOO     PPP 
//============================================================================================================================ //
 
void loop() {  
    loopCounter++;                      // für debug benchmark

    _isBleConnected = isBleConnected();
    milliTimeCopy = millis();           // to reduce traffic to the millis() function and for consistence  
    // loopStartTime = milliTimeCopy;   // Startzeit der aktuellen Loop. loopStartTime nur für benchmark nehmen.
    
    //lastLoopTime = loopStartTime - lastLoopStartTime;  // Zeit seit letzter Loop benchmark berechnen (nur für benchmark benutzen!!)
    //lastLoopStartTime = milliTimeCopy;// Aktuelle Startzeit für nächsten Durchlauf speichern. benchmark   
    
    gamepadSendReportFlag   = false;    // damit nur ein report pro schleife gesendet wird, auch wenn mehrere änderungen auftreten
    keyboardSendReportFlag  = false;    // damit nur ein report pro schleife gesendet wird, auch wenn mehrere änderungen auftreten


// touch time trap evtl komplett mit ins ins ui timetrap. touch ist eh immer kleiner als ui intervall. spart zyklen im main loop. testen. benchmark 
// >>>>> Time Trap 20-200 ms  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    
// mit throttle   
// if (processTouchTimeFlag <= milliTimeCopy) {
    
//     // Wenn die Zeit seit dem letzten Touch > 10 Sek ist -> 500ms, sonst 20ms
//     uint16_t nextInterval = (milliTimeCopy - lastTouchActivityTimerFlag > touchThrottleTimeout) ? 500 : processTouchInterval;
//     processTouchTimeFlag  =  milliTimeCopy + nextInterval + processTouchRepeatBlockerPerMenu ;  // so kann man easy 0-1000 ms in submenus aktivieren um bouncen zu reduzieren ohne zu tief in die logik rein zu gehen. 

//     if (touch.touched(GT911_MODE_POLLING) > 0) {
//         GTPoint point = touch.getPoint(0);
        
//         bool hasMoved = (abs(point.x - _cachedTouchX) > touchHysteresis) || (abs(point.y - _cachedTouchY) > touchHysteresis);

//         if (hasMoved || processTouchNextKeyTimeFlag <= milliTimeCopy) {
//             _lastTouchX = point.x;
//             _lastTouchY = point.y;
//             _cachedTouchX = point.x;
//             _cachedTouchY = point.y;
//             _touchDetected = true;

//             processTouchNextKeyTimeFlag = milliTimeCopy + 200;   // takt rate wenn man mit dem finger leichte bewegungen auf dem button macht
            
//             // NUR HIER setzen wir den Aktivitäts-Timer zurück
//             lastTouchActivityTimerFlag  = milliTimeCopy;

//             if(dbglvl) Serial.printf("[TOUCH] X:%d Y:%d | Mode: HighSpeed\n", _lastTouchX, _lastTouchY);
            
//             // Deine originalen Timer-Resets (die bleiben natürlich!)
//             ledTimeOffMillis  = milliTimeCopy + (uint32_t)ledTimeOff  * 1000;
//             stdMenuTimeMillis = milliTimeCopy + (uint32_t)stdMenuTime * 1000;
//             sleepTimerMillis  = milliTimeCopy + (uint32_t)sleepTimer  * 60000;
//         }
//     } else {
//         _cachedTouchX = -1;
//         _cachedTouchY = -1;
        
//     }
// }


if (processTouchTimeFlag <= milliTimeCopy) {
    
    // 1. Intervall bestimmen: Wenn innerhalb der letzten 4 Sek. Aktivität war -> Turbo (20ms)
    uint16_t currentInterval = (milliTimeCopy - lastTouchActivityTimerFlag <= touchThrottleTimeout) 
                               ? processTouchIntervalSpeed 
                               : processTouchIntervalThrottle;

    // Zeit für die nächste Abfrage setzen (inkl. Menü-Latenz-Modifier)
    processTouchTimeFlag = milliTimeCopy + currentInterval + processTouchRepeatBlockerPerMenu;

    // 2. Hardware-Abfrage nur im gewählten Intervall
    if (touch.touched(GT911_MODE_POLLING) > 0) {
        GTPoint point = touch.getPoint(0);
        
        // Hysterese-Check: Hat sich der Finger bewegt?
        bool hasMoved = (abs(point.x - _cachedTouchX) > touchHysteresis) || 
                        (abs(point.y - _cachedTouchY) > touchHysteresis);

        // 3. Trigger-Logik: Entweder Bewegung ODER Auto-Repeat Timer abgelaufen
        if (hasMoved || (milliTimeCopy >= processTouchNextKeyTimeFlag)) {
            
            // Werte speichern
            _lastTouchX = point.x;
            _lastTouchY = point.y;
            _cachedTouchX = point.x;
            _cachedTouchY = point.y;
            _touchDetected = true;

            // Timer für Auto-Repeat (200ms) und Turbo-Modus Aktivität (4000ms)
            processTouchNextKeyTimeFlag = milliTimeCopy + processTouchNextKeyDelay;
            lastTouchActivityTimerFlag  = milliTimeCopy;

            if(dbglvl) Serial.printf("[TOUCH] X:%d Y:%d | Turbo Active\n", _lastTouchX, _lastTouchY);
            
            // System-Timer verlängern
            ledTimeOffMillis  = milliTimeCopy + (uint32_t)ledTimeOff  * 1000;
            stdMenuTimeMillis = milliTimeCopy + (uint32_t)stdMenuTime * 1000;
            sleepTimerMillis  = milliTimeCopy + (uint32_t)sleepTimer  * 60000;
        }
    } else {
        // Finger weg: Cache löschen, damit der nächste erste Touch sofort hasMoved=true triggert
        _cachedTouchX = -1;
        _cachedTouchY = -1;
    }
}


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



//>>>>>> time trap 20-100 ms [ UIupdate() ] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
if (UIintervalTimerFlag <= milliTimeCopy) {               // UI update Timetrap
    UIintervalTimerFlag  = milliTimeCopy + UIinterval;
    ui->UIupdate(loopsPerSecond);                        //ui->UIupdate(loopsPerSecond, milliTimeCopy);              // refresh actual GUI menu
}  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   


//>>>>> time trap 1000 ms für z.b. benchmark und 1x sec alle timer-- und gegebenfalls led dimmen etc  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if (timeTrapOneSecond <= milliTimeCopy) {                     // millis() wir zählen ja ab 0, deshalb nur bis 999?
        timeTrapOneSecond =  milliTimeCopy + 1000;                // Zeitstempel für nächste runde speichern
        loopsPerSecond = loopCounter;                             // übernehme die sekündliche auszählung
        loopCounter = 0;                                          // resette zähler jede sekunde
        
    batteryESP32Status =  readBatteryPercent();                   // gemittelten prozent wert interner 8 byte ringbuffer
    if(batteryESP32Status != batteryESP32StatusLastround)
       {
        batteryESP32StatusLastround = batteryESP32Status;
        if( UImenu == 1 ) ui->drawBatteryLocal(batteryESP32Status); // TODO: change to flag methode
        if (dbglvl) { Serial.print("batteryESP32Status: "); Serial.println(batteryESP32Status);}
        if(batteryESP32Status < 2) sleepTimerMillis = 0;          // set trigger at 2% to let the device go to sleep
    }
    
    
    //  sleep timer react on underflow     
    if(milliTimeCopy > sleepTimerMillis){
        ui->DeepSleepShutDownDisplayAnimation();
        RGBshutDownSequence();
        delay(2000);
        RGBshutDownSequence();
        delay(2000);
        ui->DisplayAnimationPixelDestroy();
        if(dbglvl>1) Serial.println("esp_deep_sleep_start()");
        tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, TFT_BLACK);   // clear screen  
        ledcWrite(0, 0);                                              // display beleuchtung aus
        esp_deep_sleep_start();
    }  // wenn millis wieder größer als die gesetzte future zeitmarke wird...
    
    if(milliTimeCopy > stdMenuTimeMillis && stdMenu != UImenu)
            {
                ui->UIclearScreen = 1; // sets drawOnce automaticly to 1
                UImenu = stdMenu;
                stdMenuTimeMillis = milliTimeCopy + stdMenuTime * 1000;    // verlängere den menu auto switch timeout
            }
   
    // Check sekündlich differenz ledsleeptimer und _millis. als trigger wenn differenz >5000 (5 sek) ist. (set only led flags and send leds at end of loop)
    // wenn animation nicht läuft, starte eine neu, bis sie abgelaufen ist und AnimationIsRunning wieder 0, nach beenden der animation in der animation gesetzt.
if((60000*sleepTimer)-(sleepTimerMillis - milliTimeCopy) > AnimationActivationTime && !AnimationIsRunning){

    if(dbglvl)Serial.println("idle, check ob animation läuft, wenn nicht, starte eine neue");
    
    // --- DER LOGIK-EXORZISMUS ---
    // Wir nullen die Flags, die sendBTcommandActionKey steuern
    keyTimerFlagActionKey = 0; 
    flipFlopFlagActionKey = 0;
    releaseTrickFlagActionKey = false; // Das ist der Übeltäter, wenn er static ist!
    
    // Auch für die Front-Tasten zur Sicherheit
    keyTimerFlagFrontLeft = 0;
    flipFlopFlagFrontLeft = 0;
    
    if(!AnimationIsRunning){
    
    RGB_animation(random(1, 5), random(3000, 6000), random(80, 150));
    if(dbglvl) Serial.println("start new RGB animation");
    }
}

}  //<<<< end 1000ms time trap <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    


//>>>>>>> 10-100 ms gyro time trap  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if(gyroUpdateTimeTrapTimerFlag <= milliTimeCopy ){         // 10 ms - 50 ms time trap sweetspot ~20
       gyroUpdateTimeTrapTimerFlag =  milliTimeCopy + gyroTimeTrapTimerCycle ;

    // BMI160 Sensor aus Task1 von CPU0 holen auf sicherem weg. incl. Gyro und Accel
    int powerCalculatedLeft = 0, powerCalculatedRight = 0, powerCalculatedUp = 0;
        
    xSemaphoreTake(power_mutex, portMAX_DELAY);


// // Statt portMAX_DELAY (was alles einfriert)
// if (xSemaphoreTake(power_mutex, pdMS_TO_TICKS(2)) == pdTRUE) {
//     // ... berechne powerCalculated ...
//     xSemaphoreGive(power_mutex);
// } else {
//     // Sensor war beschäftigt oder blockiert - überspringen für Performance
// }





    //if(UImenu == 1 && !secondKeyButtonFlag){  // führe nur aus, wenn UImenu = 1 also "play" mode ist. 1 if statt 3, in allen anderen menus spart taktzyklen
    // wieder in allen menus verfügbar machen. keine timingprbleme mehr.
    if(!secondKeyButtonFlag){  // führe nur aus, wenn UImenu = 1 also "play" mode ist. 1 if statt 3, in allen anderen menus spart taktzyklen
          
            // Schlagerkennung
            if(power.left && !power.cnt_left) {
                powerCalculatedLeft = power.left * tiltGain;              // set value, and use >0 as trigger
                if(powerCalculatedLeft > 32767) powerCalculatedLeft = 32767; // max value for joystick
                //if(dbglvl >4 ) Serial.printf("show calculated values. Links: %.2fg RAW=%d powerCalculatedLeft=%d\n", RAW_TO_G(power.left), power.left, powerCalculatedLeft);        // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(1, powerCalculatedLeft);  // if(!secondKeyButtonFlag) // TODO: change to flag methode
                power.left = 0;  // reset parallel task variable
            }
            if(power.right && !power.cnt_right) {
                powerCalculatedRight = power.right * tiltGain;            // set value, and use >0 as trigger
                if(powerCalculatedRight > 32767) powerCalculatedRight = 32767; // max value for joystick
                //if(dbglvl >4 ) Serial.printf("show calculated values. Rechts: %.2fg RAW=%d, powerCalculatedRight=%d\n", RAW_TO_G(power.right), power.right, powerCalculatedRight);   // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(2, powerCalculatedRight);  // power.right *tiltGain  // TODO: change to flag methode
                power.right = 0;// reset parallel task variable
            }
            if(power.up && !power.cnt_up) {
                powerCalculatedUp = power.up * tiltGain;                   // set value, and use >0 as trigger
                if(powerCalculatedUp > 32767) powerCalculatedUp = 32767; // max value for joystick
                //if(dbglvl >4 ) Serial.printf("show calculated values. Hoch: %.2fg RAW=%d, powerCalculatedUp=%d\n", RAW_TO_G(power.up), power.up, powerCalculatedUp);                 // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(3, powerCalculatedUp);    // TODO: change to flag methode
                power.up = 0;// reset parallel task variable
                }

    
    }  
  
    xSemaphoreGive(power_mutex);
    

 static bool release_sendBTcommandTiltLeft  = 0;
 static bool release_sendBTcommandTiltRight = 0;
 static bool release_sendBTcommandTiltUp    = 0;


// wichtig, release block muss vor dem set block kommen! nicht verschieben. 
if(release_sendBTcommandTiltLeft || release_sendBTcommandTiltRight || release_sendBTcommandTiltUp) 
{ 
// hier noch mal ein zentral flag drüber, um 1 if pro loop gegen 3 zu tauschen. muss man mal im benchmark verfolgen.
    if(release_sendBTcommandTiltLeft){
        release_sendBTcommandTiltLeft=0;
        sendBTcommandTiltLeft(0, 0);     // reset X
        //if (dbglvl > 0) {Serial.printf("Set BT release_sendBTcommandTiltLeft\n");}  // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
        }
    if(release_sendBTcommandTiltRight){
        release_sendBTcommandTiltRight=0;
        sendBTcommandTiltRight(0, 0);    // reset X
        //if (dbglvl > 0) {Serial.printf("Set BT release_sendBTcommandTiltRight\n");} // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
    }
    if(release_sendBTcommandTiltUp){     // reset Y
        release_sendBTcommandTiltUp=0;
        sendBTcommandTiltFront(0, 0);
        //if (dbglvl > 0) {Serial.printf("Set BT release_sendBTcommandTiltUp\n");}     // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
        }
    }



 if (powerCalculatedLeft || powerCalculatedRight || powerCalculatedUp)  
      {
            // Sende Garantiert nur 2 richtungen mit dem höchsten ausschlag in diesem loop. pinballFX keyboard benötigt das
            if(powerCalculatedUp){
                        sendBTcommandTiltFront(1, powerCalculatedUp);
                        release_sendBTcommandTiltUp    = 1;
            };
                 
            // entweder links oder rechts. beides geht nicht. somit haben wir max up und/oder links oder rechts. also 2 tasten/richtungen
            if(powerCalculatedLeft  > powerCalculatedRight ){  // wenn beide 0 ist  0>0 auch 0. spart 
                        sendBTcommandTiltLeft(1, powerCalculatedLeft);
                        release_sendBTcommandTiltLeft  = 1;
            };

            if(powerCalculatedRight > powerCalculatedLeft){
                        sendBTcommandTiltRight(1, powerCalculatedRight);
                        release_sendBTcommandTiltRight = 1;
            };

        if(dbglvl)Serial.printf("Nudge TRY TO SEND:%s%s%s\n",release_sendBTcommandTiltUp?" UP":"", release_sendBTcommandTiltLeft?" LEFT":"",release_sendBTcommandTiltRight?" RIGHT":"");
      }





    // Winkelberechnung (stabiler Algorithmus OHNE Filter, ringbuffer könnte man machen)
 
    // Annahme: Sensor liegt "auf dem Kopf" und ist 90° um Z gedreht
    float x = -power.accel_x / 2048.0f;    
    float y = -power.accel_y / 2048.0f;    
    float z = -power.accel_z / 2048.0f;  

    pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;
    roll  = atan2(y, z) * 180.0 / PI;

    // Korrigiere Kugelorientation und verhindere seltsame effekte
    if (roll > 90) roll = 180 - roll;
    else if (roll < -90) roll = -180 - roll;

}   //<<<<<<<<<<<<<<< end 30-50 ms gyro timetrap <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




if(secondKeyButtonFlag)
{
// VIRTUAL KEYs aus Neigung /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Links - Rechts mehr als 20 Grad neigen, erzeugt 2 Tasten für je 1 Sekunde Erzeuge Tischauswahl 
// um z.B. RT/LT bzw analog alternative zu simulieren
        if(roll < -angleTrigger)  // roll < -(angleTrigger /2)  // x-achse neigen ist physikalisch mehr bewegung, daher reicht halber winkelwert als trigger 
        {
        keyTimerFlagAngleUp = (milliTimeCopy + 100);
        }        

        if(roll > angleTrigger)  // roll > (angleTrigger / 2) // x-achse neigen ist physikalisch mehr bewegung, daher reicht halber winkelwert als trigger 
        {
        keyTimerFlagAngleDown = (milliTimeCopy + 100);
        }

        if(pitch > angleTrigger / 2)   
        {
        keyTimerFlagAngleLeft = (milliTimeCopy + 100);   
        }

        if(pitch < -angleTrigger / 2)  
        {
        keyTimerFlagAngleRight = (milliTimeCopy + 100);   
        }



// Angle UP Trigger simulation aus 40 degree Neigungswinkel ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(keyTimerFlagAngleUp > milliTimeCopy){  // wenn größer, muss timer gesetzt sein und taste aktiv 
            
            if (flipFlopFlagAngleUp == 0){  
                                                sendBTcommandAngleTiltButtonDown(1, 32767); // TODO: roll * 100 falls es sinn macht diese daten analog zu nutzen. (movement z.b.)
                                                ui->drawVirtualTiltingJoystickKeys(1,1);    // TODO: change to flag methode
                                                flipFlopFlagAngleUp = 1;
                                            }
            }
            else
                {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
                if (flipFlopFlagAngleUp == 1){   
                                                sendBTcommandAngleTiltButtonDown(0);
                                                ui->drawVirtualTiltingJoystickKeys(1,0);    // TODO: change to flag methode
                                                flipFlopFlagAngleUp = 0;
                                                }
            }


// Angle Down Trigger simulation aus Neigungswinkel /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(keyTimerFlagAngleDown > milliTimeCopy){  // wenn größer, muss timer gesetzt sein und taste aktiv 
        
                if (flipFlopFlagAngleDown == 0){  
                                                sendBTcommandAngleTiltButtonUp(1, 32767);   // TODO: roll * 100 invertieren? analog joystick to 2/3 left
                                                ui->drawVirtualTiltingJoystickKeys(2,1);    // TODO: change to flag methode                                          
                                                flipFlopFlagAngleDown = 1;
                                                }
            }
            else
                {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
                if (flipFlopFlagAngleDown == 1){   
                                                sendBTcommandAngleTiltButtonUp(0);                                
                                                ui->drawVirtualTiltingJoystickKeys(2,0);    // TODO: change to flag methode
                                                flipFlopFlagAngleDown = 0;
                                                }
        }


// Angle Left Trigger simulation aus Neigungswinkel ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(keyTimerFlagAngleLeft > milliTimeCopy){  // wenn größer, muss timer gesetzt sein und taste aktiv 
            
            if (flipFlopFlagAngleLeft == 0){  
                                            sendBTcommandAngleTiltButtonRight(1, 32767);                            
                                            ui->drawVirtualTiltingJoystickKeys(3,1);        // TODO: change to flag methode
                                            flipFlopFlagAngleLeft = 1;
                                            }
            }
        else
            {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
            if (flipFlopFlagAngleLeft == 1){   
                                            sendBTcommandAngleTiltButtonRight(0);                                
                                            ui->drawVirtualTiltingJoystickKeys(3,0);        // TODO: change to flag methode
                                            flipFlopFlagAngleLeft = 0;
                                            }
        }


// Angle Right Trigger simulation aus Neigungswinkel /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(keyTimerFlagAngleRight > milliTimeCopy){  // wenn größer, muss timer gesetzt sein und taste aktiv 
            
            if (flipFlopFlagAngleRight == 0){  
                                                sendBTcommandAngleTiltButtonLeft(1, 32767);
                                                ui->drawVirtualTiltingJoystickKeys(4,1);    // TODO: change to flag methode
                                                flipFlopFlagAngleRight = 1;
                                            }
            }
            else
                {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
                if (flipFlopFlagAngleRight == 1){   
                                                sendBTcommandAngleTiltButtonLeft(0); 
                                                ui->drawVirtualTiltingJoystickKeys(4,0);   // TODO: change to flag methode
                                                flipFlopFlagAngleRight = 0;
                                                }
        }
}




// special ultra fast keyreading with rgb led emv prevention, while animation.
// 1. Die Sammel-Maske für alle 5 physikalischen Buttons
// (Stelle sicher, dass alle Pins in Bank 0 liegen, also < 32)
const uint32_t buttonMask = (1 << ioPinSideLeft) | 
                            (1 << ioPinSideRight) | 
                            (1 << ioPinFrontLeft) | 
                            (1 << ioPinFrontRight) | 
                            (1 << ioPinSideX);

// 2. Erster Snapshot vom Hardware-Register
uint32_t currentGPIO = GPIO.in;

// 3. Grober Check: Wurde IRGENDEINER dieser Pins auf LOW gezogen?
if ((currentGPIO & buttonMask) != buttonMask) {
    
    // RGB LED EMV BUG FIX: Nur verzögern, wenn die Animation wirklich funkt. Eigentlich nur notwendig wenn man keine kondensatoren einlötet. schadet aber auch nicht.
    if (AnimationIsRunning) {
        delayMicroseconds(300); 
        currentGPIO = GPIO.in; // Snapshot mit zweiter lesung, also sicherem Wert überschreiben
    }

    // --- AB HIER DIE GEFILTERTE AUSWERTUNG ---

    // Side Left
    if (!(currentGPIO & (1 << ioPinSideLeft))) {
        keyTimerFlagSideLeft = milliTimeCopy + debounceKey;
        if(dbglvl > 10) Serial.println("side L taster physikalisch ausgelöst, gpio ausgelesen");
    } 

    // Side Right
    if (!(currentGPIO & (1 << ioPinSideRight))) {
        keyTimerFlagSideRight = milliTimeCopy + debounceKey;
        if(dbglvl > 10) Serial.println("side R taster physikalisch ausgelöst, gpio ausgelesen");
    }

    // Front Left
    if (!(currentGPIO & (1 << ioPinFrontLeft))) {
        keyTimerFlagFrontLeft = milliTimeCopy + debounceKey; 
        if(dbglvl > 10) Serial.println("front L taster physikalisch ausgelöst, gpio ausgelesen");
    }

    // Front Right
    if (!(currentGPIO & (1 << ioPinFrontRight))) {
        keyTimerFlagFrontRight = milliTimeCopy + debounceKey;
        if(dbglvl > 10) Serial.println("front R taster physikalisch ausgelöst, gpio ausgelesen");
    }

    // Action Key (X)
    if (!(currentGPIO & (1 << ioPinSideX))) {
        keyTimerFlagActionKey = milliTimeCopy + 50; // Deine speziellen 50ms
        if(dbglvl > 10) Serial.println("action key taster physikalisch ausgelöst, gpio ausgelesen");
    }
}    // END RGB





// front left magic B button

if(secondKeySetLaterRelease && secondKeySetLaterReleaseTimerFlag < milliTimeCopy){// notwendig um den release nächstes loop sicherzustellen. TODO: evtl mit 100 ms timemarken flag arbeiten
    sendBTcommandPlungerLinks(0);      // release front left key
    secondKeySetLaterRelease = false;  // reset flag
    if(dbglvl)Serial.println("secondKeySetLaterRelease versucht release zu senden -> sendBTcommandPlungerLinks(0)");

}


if(keyTimerFlagFrontLeft > milliTimeCopy){ // pressed  // wenn größer, muss timer gesetzt sein und taste aktiv 
    //if(dbglvl)Serial.println("fotze");
    if (flipFlopFlagFrontLeft == 0){    // run only once by flip flop flag
                                        if(UImenu == 1) {ui->drawPhysicalVirtualKeys(2,1);                 // here is okay, no flags, its not time intense
                                                         ui->drawPhysicalVirtualKeys(6,0);}                // here is okay, no flags, its not time intense
                                        if(dbglvl>1)Serial.println("ff flag ==0, button front left triggered set time stamp");
                                        secondKeyButtonFlag = 0;  
                                        secondKeyButtonTimeMark = milliTimeCopy;                           // setze feste zeitmarke des erstcontact einmalig, um später die differenz zur aktuellen zeit auswerten zu können
                                        flipFlopFlagFrontLeft = 1;
                                        // RGB set front left to PRESSED-short (white)
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLpressed.R, LED_FrontLpressed.G, LED_FrontLpressed.B));    // FL-L
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0
                                        AnimationIsRunning = 0; // TODO: kann wohl weg, da am ende der mainschleife beim tasten druck dieses flag gesetzt wird
                                    }
}
else
    {                                   
    if (flipFlopFlagFrontLeft == 1){    // released  // taste losgelassen
                                    //if(dbglvl)Serial.println("titten");
                                    //check <500 ms, dann soll taste als B taste interpretiert werden // 
                                    if( milliTimeCopy - secondKeyButtonTimeMark < secondKeyActivationTime )  
                                      { if(dbglvl)Serial.println("schlitten");
                                        sendBTcommandPlungerLinks(1);  
                                        if(dbglvl)Serial.println("ff flag == 1 , sendBTcommandPlungerLinks(1)");  
                                        secondKeySetLaterRelease = true;                                   // hier muss das release flag gesetzt werden.
                                        secondKeySetLaterReleaseTimerFlag = milliTimeCopy + 100;      // setze future trigger
                                        secondKeyButtonFlag = 0;
                                        // RGB set front left to RELEASED-short (base color)  
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLbase.R,  LED_FrontLbase.G,  LED_FrontLbase.B));    // FR-L
                                        PixelReadyToSend++;                                                // set trigger, and use counter for what ever. reset to 0
                                        AnimationIsRunning = 0; // TODO: kann wohl weg, da am ende der mainschleife beim tasten druck dieses flag gesetzt wird
                                    } 
                                    
                                    hid->gamepad->setHat(8);                                               // release und lösche alle möglichen virtual keys um den kreis nächste code zeile
                                    gamepadSendReportFlag = 1;  

                                    if(UImenu == 1) {ui->drawPhysicalVirtualKeys(6,0);                     // here is okay, no flags, its not time intense 
                                                     ui->drawPhysicalVirtualKeys(2,0);                     // here is okay, no flags, its not time intense 
                                                     ui->fillSpriteBackground();                           // here is okay, no flags, its not time intense 
                                                     ui->drawVirtualTiltingJoystickKeys(1,0);  // clear UI // here is okay, no flags, its not time intense 
                                                     ui->drawVirtualTiltingJoystickKeys(2,0);              // here is okay, no flags, its not time intense 
                                                     ui->drawVirtualTiltingJoystickKeys(3,0);              // here is okay, no flags, its not time intense 
                                                     ui->drawVirtualTiltingJoystickKeys(4,0);              // here is okay, no flags, its not time intense 
                                                    }
                                    flipFlopFlagFrontLeft = 0;
                                    // RGB set front left to RELEASE-LONG & SHORT(shift)(base color)
                                    strip.SetPixelColor(4, RgbColor(LED_FrontLbase.R,  LED_FrontLbase.G,  LED_FrontLbase.B));    // FR-L
                                    //strip.SetPixelColor(4, RgbColor(255,  255,  20));    // FR-L
                                    PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0
                                    AnimationIsRunning = 0; // TODO: kann wohl weg, da am ende der mainschleife beim tasten druck dieses flag gesetzt wird
    }
}

// Enable [virtual] [second key] , if time is over 500ms (secondKeyActivationTime) and first key is pressed. like a shift key on a keyboard
if( milliTimeCopy - secondKeyButtonTimeMark >= secondKeyActivationTime && flipFlopFlagFrontLeft == 1 ){  // && flipFlopFlagFrontLeft == 1
    secondKeyButtonFlag = 1;
    if(UImenu == 1) ui->drawPhysicalVirtualKeys(6,1);                                                      // here is okay, no flags, its not time intense
    // RGB set front left to PRESS-LONG (shift)(blue)
    strip.SetPixelColor(4, RgbColor(LED_FrontLshifted.R,   LED_FrontLshifted.G,   LED_FrontLshifted.B  ));   // Front-L
    PixelReadyToSend++;                                    // set trigger, and use counter for what ever.  
    // AnimationIsRunning = 0; // TODO: kann wohl weg, da am ende der mainschleife beim tasten druck dieses flag gesetzt wird
if(dbglvl)Serial.println("milliTimeCopy - secondKeyButtonTimeMark >= secondKeyActivationTime && flipFlopFlagFrontLeft == 1 && !secondKeyButtonFlag");    
}
 else 
   {secondKeyButtonFlag = 0;  // hier nix weiter einfügen, läuft ständig durch. eigentlich setzt er es viel zu oft.
   }





// FrontRight  ( Taste A --> im Game "A" und Plunger)
static bool releaseTrickFlagFrontR;
if(keyTimerFlagFrontRight > milliTimeCopy){                                                // wenn größer, muss timer gesetzt sein und taste aktiv 
   
    if (flipFlopFlagFrontRight == 0){                                                      // set
                                    if(!secondKeyButtonFlag){                              // Standart Key set
                                        if(CheatLockRecordMode == 1){
                                        keyAbenchmarkTimeMark = millis();                  // merke time stamp beim drücken
                                        sendBTcommandPlungerRechts(1); 
                                        }                        
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(1,1);  // TODO: change to flag methode
                                        releaseTrickFlagFrontR = 0;                        // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                        
                                        strip.SetPixelColor(2, RgbColor(LED_FrontRpressed.R,  LED_FrontRpressed.G,  LED_FrontRpressed.B));    // FRont-R
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    else{                                                  // virtual "second" key front right set
                                        sendBTcommandPlungerRechtsSecondKey(1);                        
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(5,1);  // TODO: change to flag methode
                                        releaseTrickFlagFrontR = 1;                        // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                        
                                        strip.SetPixelColor(2, RgbColor(LED_FrontRshifted.R,  LED_FrontRshifted.G,  LED_FrontRshifted.B));    // FRont-R
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    flipFlopFlagFrontRight = 1;
                                    }
      }
else
    {                                                                                      // zeit ist abgelaufen, setze einmal auf weiß und auf release
    if (flipFlopFlagFrontRight == 1){                                                      // release 
                                    if(!releaseTrickFlagFrontR){                           // Standart Key release  // 
                if(CheatLockRecordMode == 1){
                                        skillShotMillisSend = millis() - keyAbenchmarkTimeMark;  // skillshot benchmark und variable aktualisieren
                                        sendBTcommandPlungerRechts(0); 
                                        }
                                        if(CheatLockRecordMode == 2) sendTimedPlungerButtonA = true;   // set virtual key "skillshot" with timed release                
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(1,0);  // TODO: change to flag methode
                                    }
                                    else{                                                  // virtual "second" key front right release
                                        sendBTcommandPlungerRechtsSecondKey(0);                       
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(5,0);  // TODO: change to flag methode
                                    }
                                    flipFlopFlagFrontRight = 0;

                                    strip.SetPixelColor(2, RgbColor(LED_FrontRbase.R,  LED_FrontRbase.G,  LED_FrontRbase.B));    // FRont-R
                                    PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 

    }
}





// SideLeft // Flipper links Darstellung und Sende BT command. Hier kommen die Tasten Abfragen und Flags setzen hin
static bool releaseTrickFlagSideL;
if(keyTimerFlagSideLeft > milliTimeCopy){                                                  // wenn größer, muss timer gesetzt sein und taste aktiv 
    
    if (flipFlopFlagSideLeft == 0){    
                                    if(!secondKeyButtonFlag){                              // Standart Key set
                                        sendBTcommandFlipperLinks(1); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(7,0);  // TODO: change to flag methode
                                        releaseTrickFlagSideL = 0;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                    
                                        strip.SetPixelColor(3, RgbColor(LED_FlipperLpressed.R, LED_FlipperLpressed.G, LED_FlipperLpressed.B));  // FL-L
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLflipped.R,   LED_FrontLflipped.G,   LED_FrontLflipped.B  ));  // FRONT-L
                                        strip.SetPixelColor(5, RgbColor(LED_NeopxLflipped.R,   LED_NeopxLflipped.G,   LED_NeopxLflipped.B  ));  // NEOPIXEL-L
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    else{                                                  // virtual "second" key front left set
                                        sendBTcommandFlipperLinksSecondKey(1); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,1);  // TODO: change to flag methode
                                        releaseTrickFlagSideL = 1;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                        
                                        strip.SetPixelColor(3, RgbColor(LED_FlipperLshifted.R, LED_FlipperLshifted.G, LED_FlipperLshifted.B));  // FL-L
                                        //strip.SetPixelColor(4, RgbColor(LED_FrontLflipped.R,   LED_FrontLflipped.G,   LED_FrontLflipped.B  ));  // FRONT-L
                                        strip.SetPixelColor(5, RgbColor(LED_NeopxLshifted.R,   LED_NeopxLshifted.G,   LED_NeopxLshifted.B  ));  // NEOPIXEL-L
                                    }
                                    ButtonFlipperLeftCounterToday++;
                                    ButtonFlipperLeftCounterAlltime++;
                                    flipFlopFlagSideLeft = 1;
                                    }
      }
else
    {                                                                                      // zeit ist abgelaufen, setze einmal auf weiß und auf release
    if (flipFlopFlagSideLeft == 1){   
                                    if(!releaseTrickFlagSideL){                            // Standart Key release  // 
                                        sendBTcommandFlipperLinks(0); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(7,1);  // TODO: change to flag methode
                                        
                                        strip.SetPixelColor(3, RgbColor(LED_FlipperLbase.R,  LED_FlipperLbase.G,  LED_FlipperLbase.B));    // FL-L
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLbase.R,    LED_FrontLbase.G,    LED_FrontLbase.B  ));    // FRONT-L
                                        strip.SetPixelColor(5, RgbColor(LED_NeopxLbase.R,    LED_NeopxLbase.G,    LED_NeopxLbase.B  ));    // NEOPIXEL-L
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    else{                                                  // virtual "second" key front right release
                                        sendBTcommandFlipperLinksSecondKey(0);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,0);  // TODO: change to flag methode
                                        
                                        strip.SetPixelColor(3, RgbColor(LED_FlipperLbase.R,  LED_FlipperLbase.G,  LED_FlipperLbase.B));    // FL-L
                                        //strip.SetPixelColor(4, RgbColor(LED_FrontLbase.R,    LED_FrontLbase.G,    LED_FrontLbase.B  ));    // FRONT-L
                                        strip.SetPixelColor(5, RgbColor(LED_NeopxLbase.R,    LED_NeopxLbase.G,    LED_NeopxLbase.B  ));    // NEOPIXEL-L
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    flipFlopFlagSideLeft = 0;
    }
}



// Flipper rechts Darstellung und Sende BT command
static bool releaseTrickFlagSideR;
if(keyTimerFlagSideRight > milliTimeCopy){                                                 // wenn größer, muss timer gesetzt sein und taste aktiv 
    
    if (flipFlopFlagSideRight == 0){  
                                    if(!secondKeyButtonFlag){                              // Standart Key set
                                        sendBTcommandFlipperRechts(1);                     // bleGamepad.press(BUTTON_8);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(8,0);  // draw red flipper // TODO: change to flag methode
                                        releaseTrickFlagSideR = 0;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                        strip.SetPixelColor(0, RgbColor(LED_NeopxRflipped.R,   LED_NeopxRflipped.G,   LED_NeopxRflipped.B  ));   // NEOPIXEL-R
                                        strip.SetPixelColor(1, RgbColor(LED_FlipperRpressed.R, LED_FlipperRpressed.G, LED_FlipperRpressed.B));   // FL-R
                                        strip.SetPixelColor(2, RgbColor(LED_FrontRflipped.R,   LED_FrontRflipped.G,   LED_FrontRflipped.B  ));   // Front-R
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever.  
                                    }
                                    else{                                                  // virtual "second" key front right set
                                        sendBTcommandFlipperRechtsSecondKey(1);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,1);  // TODO: change to flag methode
                                        releaseTrickFlagSideR = 1;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                    
                                        strip.SetPixelColor(0, RgbColor(LED_NeopxRshifted.R,   LED_NeopxRshifted.G,   LED_NeopxRshifted.B  ));   // NEOPIXEL-R
                                        strip.SetPixelColor(1, RgbColor(LED_FlipperRshifted.R, LED_FlipperRshifted.G, LED_FlipperRshifted.B));   // FL-R
                                        //strip.SetPixelColor(2, RgbColor(LED_FrontRflipped.R,   LED_FrontRflipped.G,   LED_FrontRflipped.B  ));   // Front-R
                                        PixelReadyToSend++;                                    // set trigger, and use counter for what ever.  
                                    }
                                    ButtonFlipperRightCounterToday++;
                                    ButtonFlipperRightCounterAlltime++;
                                    flipFlopFlagSideRight = 1;
                                    }
      }
else
    {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
    if (flipFlopFlagSideRight == 1){   
                                    if(!releaseTrickFlagSideR){                            // Standart Key release  // 
                                       sendBTcommandFlipperRechts(0); 
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(8,1);   // draw white flipper  // TODO: change to flag methode
                                       
                                       strip.SetPixelColor(0, RgbColor(LED_NeopxRbase.R,    LED_NeopxRbase.G,   LED_NeopxRbase.B  ));             // NEOPIXEL-R
                                       strip.SetPixelColor(1, RgbColor( LED_FlipperRbase.R, LED_FlipperRbase.G, LED_FlipperRbase.B));           // FL-R
                                       strip.SetPixelColor(2, RgbColor(LED_FrontRbase.R,    LED_FrontRbase.G,   LED_FrontRbase.B  ));             // Front-R
                                       PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    else{                                                  // virtual "second" key front right release
                                       sendBTcommandFlipperRechtsSecondKey(0);
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,0);   // TODO: change to flag methode
                                       
                                       strip.SetPixelColor(0, RgbColor(LED_NeopxRbase.R,    LED_NeopxRbase.G,   LED_NeopxRbase.B  ));             // NEOPIXEL-R
                                       strip.SetPixelColor(1, RgbColor( LED_FlipperRbase.R, LED_FlipperRbase.G, LED_FlipperRbase.B));           // FL-R
                                       //strip.SetPixelColor(2, RgbColor(LED_FrontRbase.R,    LED_FrontRbase.G,   LED_FrontRbase.B  ));             // Front-R
                                       PixelReadyToSend++;                                    // set trigger, and use counter for what ever. reset to 0 
                                    }
                                    flipFlopFlagSideRight = 0;
                                    }
}





if(espnowAirButtonCurrentState == 1) keyTimerFlagActionKey = milliTimeCopy + debounceKey;


//X-Button (physical on gpio35 oder neu 16) Darstellung und Sende BT command

if(keyTimerFlagActionKey > milliTimeCopy){                                                  // wenn größer, muss timer gesetzt sein und taste aktiv 
    
    if (flipFlopFlagActionKey == 0){  
                                    if(!secondKeyButtonFlag){                               // Standart Key set
                                        sendBTcommandActionKey(1);
                                        if(UImenu == 1) ui->espnowButton(2);                // draw x-button pressed // TODO: change to flag methode
                                        releaseTrickFlagActionKey = 0;                      // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
              
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLXbutton.R, LED_FrontLXbutton.G, LED_FrontLXbutton.B));     // FRONT-L
                                        strip.SetPixelColor(2, RgbColor(LED_FrontRXbutton.R, LED_FrontRXbutton.G, LED_FrontRXbutton.B));     // FRONT-R
                                        PixelReadyToSend++;
                                    }
                                    else{                                                   // virtual "second" key front right set
                                        sendBTcommandActionKeySecondKey(1);
                                        if(UImenu == 1) ui->espnowButton(3);                // TODO: erzeuge noch ein viertes icon für die shift+x key funktion. change to flag methode
                                        releaseTrickFlagActionKey = 1;                      // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                        
                                        strip.SetPixelColor(4, RgbColor(LED_FrontLshifted.R, LED_FrontLXbutton.G, LED_FrontLXbutton.B));     // FRONT-L
                                        strip.SetPixelColor(2, RgbColor(LED_FrontRshifted.R, LED_FrontRXbutton.G, LED_FrontRXbutton.B));     // FRONT-R
                                        PixelReadyToSend++;
                                    }
                                    flipFlopFlagActionKey = 1;
                                    }
      }
else
    {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
    if (flipFlopFlagActionKey == 1){   
                                    if(!releaseTrickFlagActionKey){                         // Standart Key release  // 
                                       sendBTcommandActionKeySecondKey(0); 
                                       if(UImenu == 1) ui->espnowButton(1);                 // draw white flipper  // TODO: change to flag methode

                                       //strip.SetPixelColor(4, RgbColor( LED_FrontLbase.R, LED_FrontLbase.G, LED_FrontLbase.B));      // FRONT-L  (front-l base color preset)
                                       strip.SetPixelColor(4, RgbColor( LED_FrontLbase.R, LED_FrontLbase.G, LED_FrontLbase.B));
                                       strip.SetPixelColor(2, RgbColor( LED_FrontRbase.R, LED_FrontRbase.G, LED_FrontRbase.B));      // FRONT-R  (front-l base color preset)
                                       PixelReadyToSend++;

                                    }
                                    else{                                                   // virtual "second" key front right release
                                       sendBTcommandActionKeySecondKey(0);
                                       if(UImenu == 1) ui->espnowButton(1);                 // TODO: change to flag methode

                                       strip.SetPixelColor(4, RgbColor( LED_FrontLbase.R, LED_FrontLbase.G, LED_FrontLbase.B));      // FRONT-L  (front-l base color preset)
                                       strip.SetPixelColor(2, RgbColor( LED_FrontRbase.R, LED_FrontRbase.G, LED_FrontRbase.B));      // FRONT-R  (front-l base color preset)
                                       PixelReadyToSend++;
                                    }
                                    flipFlopFlagActionKey = 0;
                                    }
}



// skillshot time trap
// set
if(sendTimedPlungerButtonA && sendTimedPlungerButtonATimerReleaseFlag == 0){    //&& zum entprellen + flipflip  //  empfange aus gui die flagge sendTimedPlungerButtonA = getimeter skillshot button gedrückt

ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] skill shot virtual key [] triggered. set virtual key time flag\n", ms/1000,ms%1000);  // debug 
   sendBTcommandPlungerRechts(1);    
   if(UImenu==4)ui->drawSkillShotButton(2);        // zeichne roten aktivitäts rahmen    // not time intense. is okay. 
   if(UImenu==1)ui->drawPhysicalVirtualKeys(1,2);                                        // not time intense. is okay. 
   // if(dbglvl > 1)Serial.println("sendTimedPlungerButtonA High Timestamp: ");          // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
   if(dbglvl){ ms = millis();Serial.printf("[%lu.%03lu]  skillshot time trap sendTimedPlungerButtonA set, sendBTcommandPlungerRechts(1)\n", ms/1000,ms%1000);}  // debug   
sendTimedPlungerButtonATimerReleaseFlag = ms + skillShotMillisSend; // neues future flag time basiert setzen
}

//release
if(sendTimedPlungerButtonATimerReleaseFlag != 0 && sendTimedPlungerButtonATimerReleaseFlag <= millis()){  // && checkt >0 && kleiner millitime. verhindert beim start eine fail release. achtung. keine millitime aus cache nehmen! realitime function nutzen.
   sendTimedPlungerButtonATimerReleaseFlag = 0;   // zeit marken flag wieder cleanen
   ms = millis();
   if(dbglvl) Serial.printf("[%lu.%03lu] skill shot release time trap sendTimedPlungerButtonA vor GFX darstellung\n",ms/1000,ms%1000);
   sendBTcommandPlungerRechts(0);
   if(UImenu==4)ui->drawSkillShotButton(3);       // lösche/überzeichne roten aktivitäts rahmen  // not time intense. is okay.
   if(UImenu==1)ui->drawPhysicalVirtualKeys(1,0);                                                // not time intense. is okay.
   sendTimedPlungerButtonA = false;  //  flag löschen
   if(dbglvl) {ms = millis(); Serial.printf("[%lu.%03lu] skill shot release time trap sendTimedPlungerButtonA release, sendBTcommandPlungerRechts(0)\n", ms/1000,ms%1000);}  // debug 
}


// sende report, falls taste oder gamepad gedrückt wurde und gerät verfügbar.
if(gamepadSendReportFlag){
   gamepadSendReportFlag = false;
   if(dbglvl) Serial.printf("[%lu.%03lu] gamepad report milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000);        // debug
//ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] realtime millis() vor gamepad report senden\n", ms/1000,ms%1000);      // debug    
   hid->gamepad->sendGamepadReport();     // ? SEND: manueller Report
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; // verlängere sleeptimer um x minuten bei tastendruck am pad. bei so vielen sekunden zeitunkritisch. hier reicht millitimecopy um cpu zu schonen
   if(dbglvl){ ms = millis();Serial.printf("[%lu.%03lu] realtime millis() nach gamepad report senden\n", ms/1000,ms%1000); } // debug 
AnimationIsRunning = 0;
}

if(keyboardSendReportFlag) {
   keyboardSendReportFlag = false; 
   if(dbglvl) Serial.printf("[%lu.%03lu] keyboard report milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
//ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] realtime millis() vor keyboard report senden\n", ms/1000,ms%1000);      // debug                    
   hid->keyboard->sendKeyReport();        // ? SEND: manueller Report
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; // verlängere sleeptimer um x minuten bei tastendruck am keyboard
   if(dbglvl){ms = millis(); Serial.printf("[%lu.%03lu] realtime millis() nach keyboard report senden\n", ms/1000,ms%1000); } // debug 
AnimationIsRunning = 0;
}



//>>>>>> time trap 10 ms [ RGB_animation() ]>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        if (RGBanimationFutureTimeFlag <= milliTimeCopy) {               // UI update Timetrap
            RGBanimationFutureTimeFlag  = RGBanimationFutureTimeFlag + 10;
            // RGB animation frame update
            if(AnimationIsRunning)RGB_animation();     // next frame rgb animation, if animation is running
            if(PixelReadyToSend){
                                PixelReadyToSend = 0;  // clear counter flag
                                strip.Show();
                                }
}  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

 








//    GGGGGG    AAAAA     MM    MM   EEEEEEE      OOOOOO    VV     VV   EEEEEEE   RRRRRR
//   GG        AA   AA    MMM  MMM   EE          OO    OO   VV     VV   EE        RR    RR
//   GG  GGG   AAAAAAA    MM MM MM   EEEEE       OO    OO    VV   VV    EEEEE     RRRRRR
//   GG    GG  AA   AA    MM    MM   EE          OO    OO     VV VV     EE        RR   RR
//    GGGGGG   AA   AA    MM    MM   EEEEEEE      OOOOOO       VV       EEEEEEE   RR    RR
//
//
// GAME OVER? NEVER EVER! :-)
// HAPPY PINBALL and PEACE   
// DOMin8or aka Vr-addicted 2025 

// What comes next?
// take a look into the todos

// pbfxc mystic quest 361
// pbfx  addams family 426

}


