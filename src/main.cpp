
//!   THIS IS A STABLE NIGHTLY BUILD
//!   MAYBE SOME COMPONETS ARE NOT FULLY INTEGRATED
//!   TRY IT AT YOUR OWN RISK

// xmas
// 2025

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

/*!



 #################################################################################################################################
 Todo: Buzzer einbauen. Einschalt/ausschalt Melodie. Tilt Trigger Alarm optional
 
 Todo: Checken on NimBLE irgend welche Nachrichten received, z.b. Force Feedback/Rumpble, falls ja --> serial.print() it

 Todo: Spiffs/NVS filesystem um Tastenclicks und Gameprofile zu speichern.

 Todo: limiter als grauen kreis im kreis visualisieren

 Todo: Battery Voltage Meter from foot pedal synchen!

 Todo: Tiefpassfilter via software auf dem acc/gyro aktivieren. ich denke 50-200 hz max oder so müssten eigentlich reichen.
 
 Todo: Qanba Gravity RGB LEDs einbauen


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

unsigned long ms = 0;                           // decorative for millis to 22.493 format
int8_t batteryESP32Status  = 0;                 // this esp32  0-100%  3.3-4.2 V
int8_t batteryESPNOWstatus = 0;                 // external esp foot controller
int8_t batteryESP32StatusLastround = -1;   

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

  // Falls noch kein einziges Sample vorhanden ist → 0%
  if (count == 0) return 0;

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
volatile uint8_t espnowAirButtonCurrentStateSend = 0;
// Callback wenn Daten empfangen wurden

struct __attribute__((packed)) KeyEvent {
  uint8_t state;                                      // 0=release, 1=press
};


const char* DEVICE_NAME = "PWZ25";                    //! device name
const char* DEVICE_MANUFACTURER = "VR1337";           //! manufacturer
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
}



// BT END =====================


int8_t emulationMode = 1;                //bluetooth HID profiles 1 = Quest, 2 = PC, 3 = Android , 4 = Iphone, 5 = Switch (per funktion und if/case rutsche) 
int8_t emulationModeOverride = 0;        // 0 automatic mode in emulationMode, 


#define debounceKey 10                   // 10 ms

uint8_t UIinterval  =   40;              // sets every x ms screenrefresh. costs power. 30 to 50 is very good. 50 makes display minimal slower, but reaction is at 50 ms 4 times higher. 
long unsigned UIintervalTimerFlag = 0;
uint16_t processTouchInterval = 200;     //200ms also 5x in der sekunde testen
unsigned long processTouchTimeFlag = 0;
unsigned long timeTrapOneSecond = 0;
uint8_t UImenu      =    0;              // Startmenü-Index (auch in klasse lese und schreibbar?) // später über filesystem oder in rtc speichern
int     sleepTimer  =   60;              // 60 Minuten nach letztem tastendruck deep sleep shutdown. später über filesystem oder in rtc speichern
int     ledTimeOff  =   60;              // 60 Sekunden = 1 minuten bis die leds zum stromsparen ausgehen. jede taste/touch reaktiviert timer
bool    drawOnce    =    1; 
uint8_t stdMenu     =    4;              // fallback menu, next variable defines timeout time
int     stdMenuTime =   20;              // springt danach zurück in stdMenu
int     tiltCounterGlob= 0;
int     dbgGamePad     = 0;              // um die richtigen tasten codes mit rechter flipper taste raus zu bekommen.
unsigned long UIpreviousMillis  = 0;     // Letzter Zeitpunkt, zu dem der Code ausgeführt wurde
unsigned long ledTimeOffMillis  = millis() + ledTimeOff  * 1000;      // 90 seKunden bis die LEDs ausgehen
unsigned long sleepTimerMillis  = millis() + sleepTimer  * 1000 * 60; // 10 Minuten deep sleep timer
unsigned long stdMenuTimeMillis = millis() + stdMenuTime * 1000;      // 20 Sekunden bis zum nächsten Menü
unsigned long gyroUpdateTimeTrapTimerFlag = 0;
int8_t gyroTimeTrapTimerCycle = 30;      // z.b. 20 ms also 50x die sekunde mit dem per separtem parallel task gesampelten daten synchen
bool     gamepadSendReportFlag  = false; // wenn true, wird am ende der schleife ein gamepad report gesendet. so können mehrere tasten in einem report gesendet werden.
bool     keyboardSendReportFlag = false; // wenn true, wird am ende der schleife ein keyboard report gesendet. so können mehrere tasten in einem report gesendet werden.
int      skillShotMillisSend    = 426 ;  // ms
unsigned long skillShotMillisStartTime = 0;
int8_t CheatLockRecordMode = 1;    // 1 standart mode alles wie immer aber samplet immer press-release zeitdifferenz, 2 skillshot->A taste bei release triggert sendTimedPlungerButtonA 
unsigned long keyAbenchmarkTimeMark = 0; // speicher bei drücken von A den timestamp und bei release ziehen wir millis() ab. differenz = benchmark

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
#define TOUCH_INT 0                                   // Interrupt-Pin (falls nicht verwendet, auf -1 setzen)
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
uint32_t ButtonFlipperLeftCounterToday    = 0;        // counter
uint32_t ButtonFlipperRightCounterToday   = 0;        // 0 = nicht gedrückt, 1 = gedrückt
uint32_t ButtonFlipperLeftCounterAlltime  = 100;      // 0 = nicht gedrückt, 1 = gedrückt 
uint32_t ButtonFlipperRightCounterAlltime = 100;      // 0 = nicht gedrückt, 1 = gedrückt


const uint8_t ioPinSideLeft   =   4 ;                 // cut pcb trace
const uint8_t ioPinSideRight  =  17 ;                 // cut pcb trace
const uint8_t ioPinFrontLeft  =   0 ;                 // only scratch pcb trace GPIO0 shared with boot mode, easy to flash with (FRONT LEFT)
const uint8_t ioPinFrontRight =  16 ;                 // cut pcb trace



int  gamepadXfinal             = 0;
int  gamepadYfinal             = 0;


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


/* global timer vars */                     // tastenabfrage variablen timemarks and flipflop flags

uint32_t milliTimeCopy           = 0;

uint32_t keyTimerFlagSideLeft    = 0;       // Flipper links debounce Timemark
int flipFlopFlagSideLeft         = 0;

uint32_t keyTimerFlagSideRight   = 0;       // Flipper rechts debounce Timemark
int flipFlopFlagSideRight        = 0;

uint32_t keyTimerFlagFrontLeft   = 0;       // Front Taste links
int flipFlopFlagFrontLeft        = 0;

uint32_t keyTimerFlagFrontRight  = 0;       // Plunger
int flipFlopFlagFrontRight       = 0;

uint32_t keyTimerFlagTwoButton   = 0;       // spezial keys durch 2 front tasten kombo ausgelöst
int flipFlopFlagTwoButton        = 0;

uint32_t keyTimerFlagFourButton  = 0;       // spezial keys durch 4 tasten kombo ausgelöst
int flipFlopFlagFourButtons      = 0;

uint32_t keyTimerFlagTilt        = 0;       // Timer Flag, wenn sich dieser ändert in größer als aktuelle zeit, setze auch direction, dann wird "flipFlopFlagTilt" high, bis debounce zuende ist
int flipFlopFlagTilt             = 0;       // änderung eines neuen timers wird das hier high
int keyTimerFlagTiltDirection    = 0;       // 0 = nicht zeichnen (überspringen), 1 = links, 2 = rechts, 3 = hoch, 4 = reset hintergrund

uint32_t secondKeyButtonTimeMark = 0;       // Merke Timestamp sobald die taste antriggert, um später simple die differenz zu 400ms normal to multi function button schnell berechnen zu können.
bool secondKeyButtonFlag         = false;   // wenn dieser true ist, dann wird die zweite taste gedrückt, und die erste taste wird nicht mehr abgefragt.
int  secondKeyActivationTime     = 800;     // 400 ms bis der zweite button erkannt wird.
bool secondKeySetLaterRelease    = 0;
unsigned long secondKeySetLaterReleaseTimerFlag = 0;
bool sendTimedPlungerButtonA     = false;
unsigned long sendTimedPlungerButtonATimerReleaseFlag = 0;

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

//float accelerationTriggerG      = 1.9;          // 1.8g  G-Force value to trigger the Tilt Key  ACHTUNG, ES MUSS EINE KOMMA ZAHL SEIN. z.b. 1.8 , 2.0 oder 2.3 etc.

int angleTrigger                = 12;             // Bei Neigung mehr als .. Grad, aktiviere Spezial tasten. Z.b. rechter joystick, d-pad

int benchmarkTimerFlag          = 0;
int benchmarkRoundValue         = 0;

int counterKeysPressedOverall   = 1337;           // TODO: load/save to lokal storage for overall pressed and session pressed keys  
int counterKeysPressedToday     = 0;



// Plunger links (meta taste B)
// input Mode 0 = release, 1 set
void sendBTcommandPlungerLinks(bool inputMode){   // diese taste zuerst abfragen für multi button 500ms trick

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
        //if(dbglvl  > 0 )Serial.printf("sendBTcommandAngleTiltButtonLeft(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
        
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
    //if(dbglvl  > 0 )Serial.printf("sendBTcommandAngleTiltButtonRight(inputMode:%d) useMode:%d, analogValue:%d\n",inputMode,useMode,analogValue); // to safe every possible cpu cylcle, de-comment it only if needed for develpemnt and to understand the var values. 
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

// Action Key (ist normal X auf dem meta controller) ich verwende ihn z.b. für den funkbutton
// input Mode 0 = release, 1 set
void sendBTcommandActionKey(bool inputMode){   
    
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


// super schnelle funktion zum lesen der manuellen tasten, die alle gepulluped sind. deutlich schneller als digitalRead
inline bool readPinLow(uint8_t pin) {  // gut lesbare turbo funktion zum lesen der 4 manuellen tasten, die alle gepulluped sind. deutlich schneller als digitalRead
    return !(GPIO.in & (1 << pin));
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

    

    if(dbglvl) Serial.begin(115200);  // 1 only fps/lopps/sec, ab 2 mit serial bus ausgabe

    if(dbglvl) Serial.println("enter setup()");

    hid = new GamepadKeyboardHID(DEVICE_NAME, DEVICE_MANUFACTURER);
    
    adc1_config_width(ADC_WIDTH_BIT_12);                            // 0..4095
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);     // Spannung 3.3 


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
            ui->setTouch(&touch);  // Touch HIER setzen, nach erfolgreicher Init
        } else {
            if(dbglvl>1) Serial.println("ERROR: GT911 initialization failed!");
        }
    } else {
        if(dbglvl>1) Serial.println("ERROR: GT911 not found on I2C bus!");
    }
    

    // Sensor Setup
    if(dbglvl>1) Serial.println("Initializing Gyro Sensor...");
      
      Wire.begin(I2C_SDA, I2C_SCL, 1000000);
      delay(50);

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
      delay(500);
    
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
    
      if(dbglvl>1) Serial.println("Initialisierung abgeschlossen");
   
     
    // UI mit Gamepad verknüpfen
    ui->begin();           // Jetzt ist ui initialisiert
    ui->setTouch(&touch);  

   // ui->setGamepad(gamepad);
     ui->setGamepad(hid->gamepad);


    if(dbglvl>1) Serial.println("Initializing PWM...");
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(BACKLIGHT_PIN, PWM_CHANNEL);
    
    // Backlight einschalten  (macht intro() mit einem softstart sonnenaufgang)
    if(dbglvl>1) Serial.println("Showing intro...");
    ui->intro();


// ========================================================================== //
   
    // Mechanical Switches soldered to IO0,IO4,IO16,IO17 
    pinMode(ioPinSideLeft,  INPUT_PULLUP); // set internal pullup. if resistor is used, change to "INPUT"
    pinMode(ioPinSideRight, INPUT_PULLUP); // set internal pullup. if resistor is used, change to "INPUT"
    pinMode(ioPinFrontLeft,  INPUT);       // b-key gpio0 fix! shared with boot mode, easy to flash with
    pinMode(ioPinFrontRight, INPUT);       // a-Key gpio4 theoretisch geht auch INPUT_PULLUP, aber wake up from sleep geht nur mit 47k pull resistor soldered for deep sleep wakeup

    
    // PCB Power management                // Deep Sleep Wake-Up bei fallender Flanke, also wenn linker plunger gedrückt wird.
    // TODO: statt deep sleep 2x die leitung des batterie management chips auf ground ziehen, für endgültige abschaltung
    esp_sleep_enable_ext0_wakeup((gpio_num_t) ioPinFrontLeft, 0);  // hängt an gpio0, besser an ioPinFrontRight. mit 47k. verbraucht weniger strom der hängt an 10k pullup. somit checke high to low

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


 
//>>>>>> time trap 20-100 ms [ UIupdate() ] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        if (UIintervalTimerFlag <= milliTimeCopy) {               // UI update Timetrap
        UIintervalTimerFlag  = milliTimeCopy + UIinterval;
        ui->UIupdate(loopsPerSecond, milliTimeCopy);              // refresh actual GUI menu, with time trap
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
    }
    
    // Display dimmer timer, sleep timer react on underflow 
        
    if(milliTimeCopy > sleepTimerMillis){esp_deep_sleep_start();}  // wenn millis wieder größer als die gesetzte future zeitmarke wird...
    
    if(milliTimeCopy > stdMenuTimeMillis && stdMenu != UImenu)
            {
                ui->UIclearScreen = 1; // setz drawOnce automaticly to 1
                UImenu = stdMenu;
                // drawOnce = 1;
                stdMenuTimeMillis = milliTimeCopy + stdMenuTime * 1000;    // verlängere den menu auto switch timeout
            }
}  //<<<< end 1000ms time trap <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    


//>>>>>>> 10-100 ms gyro time trap  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if(gyroUpdateTimeTrapTimerFlag <= milliTimeCopy ){         // 10 ms - 50 ms time trap sweetspot ~20
       gyroUpdateTimeTrapTimerFlag =  milliTimeCopy + gyroTimeTrapTimerCycle ;

    // BMI160 Sensor aus Task1 von CPU0 holen auf sicherem weg. incl. Gyro und Accel
    int powerCalculatedLeft = 0, powerCalculatedRight = 0, powerCalculatedUp = 0;
    xSemaphoreTake(power_mutex, portMAX_DELAY);

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



// better readable and compiler optimzed version hardware key readings, without rtos. 
if (readPinLow( ioPinSideLeft   )) keyTimerFlagSideLeft   = milliTimeCopy + debounceKey;
if (readPinLow( ioPinSideRight  )) keyTimerFlagSideRight  = milliTimeCopy + debounceKey;
if (readPinLow( ioPinFrontLeft  )) keyTimerFlagFrontLeft  = milliTimeCopy + debounceKey;
if (readPinLow( ioPinFrontRight )) keyTimerFlagFrontRight = milliTimeCopy + debounceKey;



// front left magic B button

if(secondKeySetLaterRelease && secondKeySetLaterReleaseTimerFlag < milliTimeCopy){// notwendig um den release nächstes loop sicherzustellen. TODO: evtl mit 100 ms timemarken flag arbeiten
    sendBTcommandPlungerLinks(0);      // release front left key
    secondKeySetLaterRelease = false;  // reset flag
    // if(dbglvl > 1)Serial.println("secondKeySetLaterRelease versucht release zu senden -> sendBTcommandPlungerLinks(0)");
}


if(keyTimerFlagFrontLeft > milliTimeCopy){ // pressed  // wenn größer, muss timer gesetzt sein und taste aktiv 
    
    if (flipFlopFlagFrontLeft == 0){    // run only once by flip flop flag
                                        if(UImenu == 1) {ui->drawPhysicalVirtualKeys(2,1);                 // here is okay, no flags, its not time intense
                                                         ui->drawPhysicalVirtualKeys(6,0);}                // here is okay, no flags, its not time intense
                                        // if(dbglvl>1)Serial.println("button front left triggered set time stamp");
                                        secondKeyButtonFlag = 0;  
                                        secondKeyButtonTimeMark = milliTimeCopy;                           // setze feste zeitmarke des erstcontact einmalig, um später die differenz zur aktuellen zeit auswerten zu können
                                        flipFlopFlagFrontLeft = 1;
   }
}
else
    {                                   
    if (flipFlopFlagFrontLeft == 1){    // released  // taste losgelassen
                                    //check <500 ms, dann soll taste als B taste interpretiert werden // 
                                    if( milliTimeCopy - secondKeyButtonTimeMark < secondKeyActivationTime )  
                                      { 
                                        sendBTcommandPlungerLinks(1);  
                                        // if(dbglvl)Serial.println("sendBTcommandPlungerLinks(1)");  
                                        secondKeySetLaterRelease = true;                                   // hier muss das release flag gesetzt werden.
                                        secondKeySetLaterReleaseTimerFlag = milliTimeCopy + 100;
                                        secondKeyButtonFlag = 0;  
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
                                     
    }
}

// Enable [virtual] [second key] , if time is over 500ms (secondKeyActivationTime) and first key is pressed. like a shift key on a keyboard
if( milliTimeCopy - secondKeyButtonTimeMark >= secondKeyActivationTime && flipFlopFlagFrontLeft == 1){
    secondKeyButtonFlag = 1;
    if(UImenu == 1) ui->drawPhysicalVirtualKeys(6,1);                                                      // here is okay, no flags, its not time intense
    //if(dbglvl)Serial.println("SHIFT for second keys active");  
    }
 else 
   {secondKeyButtonFlag = 0;}





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
                                    }
                                    else{                                                  // virtual "second" key front right set
                                        sendBTcommandPlungerRechtsSecondKey(1);                        
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(5,1);  // TODO: change to flag methode
                                        releaseTrickFlagFrontR = 1;                        // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
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
                                    }
                                    else{                                                  // virtual "second" key front left set
                                        sendBTcommandFlipperLinksSecondKey(1); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,1);  // TODO: change to flag methode
                                        releaseTrickFlagSideL = 1;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
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
                                    }
                                    else{                                                  // virtual "second" key front right release
                                        sendBTcommandFlipperLinksSecondKey(0);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,0);  // TODO: change to flag methode
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
                                    }
                                    else{                                                  // virtual "second" key front right set
                                        sendBTcommandFlipperRechtsSecondKey(1);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,1);  // TODO: change to flag methode
                                        releaseTrickFlagSideR = 1;                         // stellt 100% zuverlässig ausschließlich den korrekten button nach benutzung zurück
                                    }
                                    ButtonFlipperRightCounterToday++;
                                    ButtonFlipperRightCounterAlltime++;
                                    flipFlopFlagSideRight = 1;
                                    }
      }
else
    {   // zeit ist abgelaufen, setze einmal auf weiß und auf release
    if (flipFlopFlagSideRight == 1){   
                                    if(!releaseTrickFlagSideR){                             // Standart Key release  // 
                                       sendBTcommandFlipperRechts(0); 
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(8,1);    // draw white flipper  // TODO: change to flag methode
                                    }
                                    else{                                                   // virtual "second" key front right release
                                       sendBTcommandFlipperRechtsSecondKey(0);
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,0);    // TODO: change to flag methode
                                    }
                                    flipFlopFlagSideRight = 0;
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
   if(dbglvl) Serial.printf("[%lu.%03lu] milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000);        // debug
//ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] realtime millis() vor gamepad report senden\n", ms/1000,ms%1000);      // debug    
   hid->gamepad->sendGamepadReport();     // ? SEND: manueller Report
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; // verlängere sleeptimer um x minuten bei tastendruck am pad. bei so vielen sekunden zeitunkritisch. hier reicht millitimecopy um cpu zu schonen
   if(dbglvl){ ms = millis();Serial.printf("[%lu.%03lu] realtime millis() nach gamepad report senden\n", ms/1000,ms%1000); } // debug 
}

if(keyboardSendReportFlag) {
   keyboardSendReportFlag = false; 
   if(dbglvl) Serial.printf("[%lu.%03lu] milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000);         // debug
//ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] realtime millis() vor keyboard report senden\n", ms/1000,ms%1000);      // debug                    
   hid->keyboard->sendKeyReport();        // ? SEND: manueller Report
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; // verlängere sleeptimer um x minuten bei tastendruck am keyboard
   if(dbglvl){ms = millis(); Serial.printf("[%lu.%03lu] realtime millis() nach keyboard report senden\n", ms/1000,ms%1000); } // debug 
}



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


}

