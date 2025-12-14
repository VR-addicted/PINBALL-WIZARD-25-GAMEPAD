




//! DONT INSTALL IT














#ifndef BUILD_NUMBER                      
 #define BUILD_NUMBER 0 
#endif

#ifndef DBGLVL_DEFAULT                    
 #define DBGLVL_DEFAULT 0 
#endif

#ifndef DBGLVLOSD_DEFAULT                 
 #define DBGLVLOSD_DEFAULT 0 
#endif

int     dbglvl      = DBGLVL_DEFAULT    ; 
int     dbglvlOSD   = DBGLVLOSD_DEFAULT ; 


//! test 1














// comment filter, und filter vom filter //!




/*!
21.01.2022 - 2025 by DOMin8or

 #################################################################################################################################
 Todo: Buzzer einbauen. Einschalt/ausschalt Melodie. Tilt Trigger Alarm optional
 
 Todo: Flag für sleep timer, der aber nur max alle 1000ms abgefragt wird.A0

 Todo: Checken on NimBLE irgend welche Nachrichten received, z.b. Force Feedback/Rumpble, falls ja --> serial.print() it

 Todo: Spiffs/NVS filesystem um Tastenclicks und Gameprofile zu speichern.

 Todo: limiter als grauen kreis im kreis visualisieren

 Todo: Battery Voltage Meter nur ins menu0() und nur sekündlich! checked with gemini. good idea. use the cheap yellow´s light sensor. remove sensor on pcb!

 Todo: Tiefpassfilter via software auf dem acc/gyro aktivieren. ich denke 50-200 hz max oder so müssten eigentlich reichen.
 
 // RGB LEDs seitlich
 // Action Key for pinballFX arcade mode, via food padel? ALMOST DONE
 #################################################################################################################################
 PCB HACKS:
 cut trace between IO16 and IO17, to solder to gpio side
 scratch gpio0 0 to solder on the trace. no need to cut the trace.
 desolder light sensor and solder a wire to the PCB. use the light sensor as a battery voltage divider.
 solder a wire to the PCB for the battery voltage divider. )
 ###################################################################################################################################
 -----------------------------------------------------------------------------------------------------------------------------------
 Tastenbelegungen PinBall FX Oculus Quest. PROFILE: 1   
 In Game Keys                   |  Game function                | bleGamepad commands
 A                             ->  Button "A",Confirm, Plunger -> BUTTON_1                          [real key, Pin IO16, 10-30ms debounce]
 B                             ->  Button "B" back             -> BUTTON_2                          [real key, Pin  IO0, 10-30ms debounce]
 Flipper Left                  ->                              -> BUTTON_7                          [real key, Pin IO17, 10-30ms debounce]
 Flipper Right                 ->                              -> BUTTON_8                          [real key, Pin  IO4, 10-30ms debounce]
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
#include <nvs_flash.h>                          
#include <esp_err.h>                            
#include <esp_system.h>                         
#include <esp_bt.h>                             
#include <math.h>
#include <GT911.h>
#include <TFT_eSPI.h>
#include "GUI.h"
#include <WiFi.h>
#include <esp_wifi.h>

unsigned long ms = 0;                           
int8_t batteryESP32Status  = 0;                 
int8_t batteryESPNOWstatus = 0;                 

#include <esp_now.h>                            

uint8_t senderMac[] = {0x78, 0xE3, 0x6D, 0x1A, 0x8C, 0xC8}; 
int TIMEOUT_MS = 1000;                          


unsigned long lastPacketTime = 0;
bool wasConnected = false;

volatile uint8_t espnowAirButtonCurrentState = 0;
volatile uint8_t espnowAirButtonCurrentStateSend = 0;


struct __attribute__((packed)) KeyEvent {
  uint8_t state;                                
};


const char* DEVICE_NAME = "PWZ25";              //! device name
const char* DEVICE_MANUFACTURER = "VR1337";     //! manufacturer
bool _isBleConnected = 0;

  
String macAdress = "00:00:00:00:00:00";        

#include "HIDKeycodes.h"
#include "HIDKeyboardTypes.h"
#include "GamepadKeyboardHID.h"

GamepadKeyboardHID* hid;


volatile bool btPairingActive = false;          
volatile uint16_t g_lastConnHandle = 0xFFFF;



void restoreHIDNotifications(NimBLEServer* srv) {
    NimBLEService* hidSvc = srv->getServiceByUUID(NimBLEUUID((uint16_t)0x1812));
    if (!hidSvc) return;
    
    auto chars = hidSvc->getCharacteristics();
    for (auto c : chars) {
        
        if (c->getProperties() & NIMBLE_PROPERTY::NOTIFY) {
            NimBLEDescriptor* cccd = c->getDescriptorByUUID(NimBLEUUID((uint16_t)0x2902));
            if (cccd) {
                uint8_t notifyOn[2] = {0x01, 0x00}; 
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
    
    
    
    
    
    
    NimBLEServer* srv = NimBLEDevice::getServer();
    if (!srv) return;
    
    uint8_t connCount = srv->getConnectedCount();
    
    
    if (connCount != lastConnCount) {
        if (connCount > 0) {
            
            auto connectedPeers = srv->getPeerDevices();
            if (!connectedPeers.empty()) {
                g_lastConnHandle = connectedPeers[0];
                
                
                NimBLEConnInfo connInfo = srv->getPeerInfo(g_lastConnHandle);
                macAdress = String(connInfo.getAddress().toString().c_str());
                
                if (dbglvl) {
                    Serial.printf("[BLE] Connected: %s (handle: 0x%04X)\n", 
                                  macAdress.c_str(), g_lastConnHandle);
                }
                
                
                
            }
        } else {
            
            if (dbglvl) {
                Serial.printf("[BLE] Disconnected (prev MAC: %s)\n", macAdress.c_str());
            }
            g_lastConnHandle = 0xFFFF;
            macAdress = "00:00:00:00:00:00";
        }
        
        lastConnCount = connCount;
    }
}






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

    
    probeService(NimBLEUUID((uint16_t)0x1800), {  }, "GAP");
    probeService(NimBLEUUID((uint16_t)0x1801), {  }, "GATT");



    if(dbglvl) Serial.println("[BLE] GAP/GATT services are managed by the stack (not listed in app server)");
    if(dbglvl) Serial.println("[BLE] GAP (0x1800) / GATT (0x1801): handled by controller, not app-created -> expected not to list here");
    
    
    probeService(NimBLEUUID((uint16_t)0x180F), { NimBLEUUID((uint16_t)0x2A19) }, "Battery");

    
    probeService(NimBLEUUID((uint16_t)0x180A), {
        NimBLEUUID((uint16_t)0x2A29), 
        NimBLEUUID((uint16_t)0x2A24), 
        NimBLEUUID((uint16_t)0x2A25), 
        NimBLEUUID((uint16_t)0x2A27), 
        NimBLEUUID((uint16_t)0x2A26)  
    }, "Device Information");

    
    probeService(NimBLEUUID((uint16_t)0x1812), {
        NimBLEUUID((uint16_t)0x2A4A), 
        NimBLEUUID((uint16_t)0x2A4B), 
        NimBLEUUID((uint16_t)0x2A4E), 
        NimBLEUUID((uint16_t)0x2A4D), 
        NimBLEUUID((uint16_t)0x2A4C)  
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
        macAdress.c_str(), 
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
  NimBLEDevice::deinit(true);  
  if(dbglvl)Serial.println("deinit()");
  delay(1000);
  if(dbglvl)Serial.println("NVS Flash Erase and Init started...");
  
  esp_err_t err = nvs_flash_erase();   
  if (err != ESP_OK) {
    if(dbglvl)Serial.printf("nvs_flash_erase() failed: %s\n", esp_err_to_name(err));
  }
  err = nvs_flash_init();
  if (err != ESP_OK) {
    if(dbglvl)Serial.printf("nvs_flash_init() failed: %s\n", esp_err_to_name(err));
  }
  delay(1000);
  if(dbglvl)Serial.println("NVS Flash Erase and Init done. Restarting...");
  
  esp_restart();
}




bool isBleConnected() {
    NimBLEServer* srv = NimBLEDevice::getServer();
    bool connected = srv && srv->getConnectedCount() > 0;
    
    
    
    
    
    
    
    
    


    
    return connected;
}






int8_t emulationMode = 1;                
int8_t emulationModeOverride = 0;        


#define debounceKey 10                   

uint8_t UIinterval  =   40;              
long unsigned UIintervalTimerFlag = 0;
uint16_t processTouchInterval = 200;     
unsigned long processTouchTimeFlag = 0;
unsigned long timeTrapOneSecond = 0;
uint8_t UImenu      =    0;              
int     sleepTimer  =   60;              
int     ledTimeOff  =   60;              
bool    drawOnce    =    1; 
uint8_t stdMenu     =    4;              
int     stdMenuTime =   20;              
int     tiltCounterGlob= 0;
int     dbgGamePad     = 0;              
unsigned long UIpreviousMillis  = 0;     
unsigned long ledTimeOffMillis  = millis() + ledTimeOff  * 1000;      
unsigned long sleepTimerMillis  = millis() + sleepTimer  * 1000 * 60; 
unsigned long stdMenuTimeMillis = millis() + stdMenuTime * 1000;      
unsigned long gyroUpdateTimeTrapTimerFlag = 0;
int8_t gyroTimeTrapTimerCycle = 30;      
bool     gamepadSendReportFlag  = false; 
bool     keyboardSendReportFlag = false; 
int      skillShotMillisSend    = 426 ;  
unsigned long skillShotMillisStartTime = 0;
int8_t CheatLockRecordMode = 1;    
unsigned long keyAbenchmarkTimeMark = 0; 


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
int tiltGain    =    12;                                     
int tiltTresholdMenuX = THRESHOLD_X / 100;                   
int tiltTresholdMenuY = THRESHOLD_Y / 100;                   
float pitch, roll;

int tiltLimiter   = 10;                                      

#define RAW_TO_G(raw) ((float)raw / 2048.0f)                 

#define GYRO_TO_DPS(raw) ((float)raw * 0.0076293945f)        



volatile struct {  
    
    int16_t left, right, up;
    uint8_t cnt_left, cnt_right, cnt_up;
    
    
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x, gyro_y, gyro_z;
  } power = {0};
  
  SemaphoreHandle_t power_mutex;
  
  
  void sensorTask(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(1);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1) {
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x12);
      Wire.endTransmission(false);
      Wire.requestFrom(BMI160_ADDR, 6);
      if(Wire.available() == 6) {
        int16_t ax = Wire.read() | (Wire.read() << 8);
        int16_t ay = Wire.read() | (Wire.read() << 8);
        int16_t az = Wire.read() | (Wire.read() << 8);
        
        xSemaphoreTake(power_mutex, portMAX_DELAY);
        
        
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
  
        
        power.accel_x = ax;
        power.accel_y = ay;
        power.accel_z = az;
  
        
        if(power.cnt_left) power.cnt_left--;
        if(power.cnt_right) power.cnt_right--;
        if(power.cnt_up) power.cnt_up--;
  
        xSemaphoreGive(power_mutex);
      }
  
      
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
  }   







#define TOUCH_SDA 33                                  
#define TOUCH_SCL 32                                  
#define TOUCH_INT 0                                   
#define TOUCH_RST 25                                  
#define GT911_I2C_ADDR 0x5D                           
#define SCREEN_WIDTH  240                             
#define SCREEN_HEIGHT 320                             

#define BACKLIGHT_PIN  27                             
#define PWM_CHANNEL    0                              
#define PWM_RESOLUTION 8                              
#define PWM_FREQUENCY  5000                           

#define IO_PIN_ADC_BATTERY 34 


uint32_t loopCounter       = 0;
uint32_t loopsPerSecond    = 0;
uint32_t lastLoopTime      = 0;
uint32_t lastSecondTime    = 0;
unsigned long loopStartTime     = 0;
unsigned long lastLoopStartTime = 0; 


uint32_t ButtonFlipperLeftCounterToday    = 0;        
uint32_t ButtonFlipperRightCounterToday   = 0;        
uint32_t ButtonFlipperLeftCounterAlltime  = 100;      
uint32_t ButtonFlipperRightCounterAlltime = 100;      


const uint8_t ioPinSideLeft   =   4 ;                 
const uint8_t ioPinSideRight  =  17 ;                 
const uint8_t ioPinFrontLeft  =   0 ;                 
const uint8_t ioPinFrontRight =  16 ;                 


int PWM_POWER      = 200;    


int  gamepadXfinal             = 0;
int  gamepadYfinal             = 0;




TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
GUI* ui = nullptr;




TwoWire I2C_1(1);
GT911 touch(&I2C_1);  

bool isGT911Connected() {
    I2C_1.beginTransmission(GT911_I2C_ADDR);
    return (I2C_1.endTransmission() == 0);
}


       

uint32_t milliTimeCopy           = 0;

uint32_t keyTimerFlagSideLeft    = 0;       
int flipFlopFlagSideLeft         = 0;

uint32_t keyTimerFlagSideRight   = 0;       
int flipFlopFlagSideRight        = 0;

uint32_t keyTimerFlagFrontLeft   = 0;       
int flipFlopFlagFrontLeft        = 0;

uint32_t keyTimerFlagFrontRight  = 0;        
int flipFlopFlagFrontRight       = 0;

uint32_t keyTimerFlagTwoButton   = 0;       
int flipFlopFlagTwoButton        = 0;

uint32_t keyTimerFlagFourButton  = 0;        
int flipFlopFlagFourButtons      = 0;

uint32_t keyTimerFlagTilt        = 0;       
int flipFlopFlagTilt             = 0;       
int keyTimerFlagTiltDirection    = 0;       

uint32_t secondKeyButtonTimeMark = 0;       
bool secondKeyButtonFlag         = false;   
int  secondKeyActivationTime     = 800;     
bool secondKeySetLaterRelease    = 0;
unsigned long secondKeySetLaterReleaseTimerFlag = 0;
bool sendTimedPlungerButtonA     = false;
unsigned long sendTimedPlungerButtonATimerReleaseFlag = 0;


#define debounceTilt 100 


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



int angleTrigger                = 12;        

int benchmarkTimerFlag          = 0;
int benchmarkRoundValue         = 0;

int counterKeysPressedOverall   = 1337;      
int counterKeysPressedToday     = 0;





void sendBTcommandPlungerLinks(bool inputMode){   

    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->press(BUTTON_2);           gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_3);           gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_1);           gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_1);           gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyPress(KEY_I);          keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->press(BUTTON_2);           gamepadSendReportFlag   = true;  break;  
           default: break;                                             
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->release(BUTTON_2);         gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_3);         gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_1);         gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_1);         gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyRelease(KEY_I);        keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->release(BUTTON_2);         gamepadSendReportFlag   = true;  break;  
           default: break;                                             
        }
    }
}




void sendBTcommandPlungerRechts(bool inputMode){   
  
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
                  switch (useMode) { 
                    
                    case 1: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  
                    case 2: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  
                    case 3: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  
                    case 4: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  
                    case 5: hid->keyboard->keyPress(KEY_8);   keyboardSendReportFlag  = true;  break;  
                    case 6: hid->gamepad->press(BUTTON_1);    gamepadSendReportFlag   = true;  break;  
                   default: break;                                     
                }
            }
        else{
                switch (useMode) { 
                    
                    case 1: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  
                    case 2: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  
                    case 3: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  
                    case 4: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  
                    case 5: hid->keyboard->keyRelease(KEY_8); keyboardSendReportFlag  = true;  break;  
                    case 6: hid->gamepad->release(BUTTON_1);  gamepadSendReportFlag   = true;  break;  
                   default: break;                                     
                }
            }
}




void sendBTcommandPlungerRechtsSecondKey(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
                
                switch (useMode) { 
                    case 1: hid->gamepad->press(BUTTON_11);    gamepadSendReportFlag   = true;  break;  
                    case 2: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  
                    case 3: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  
                    case 4: hid->gamepad->press(BUTTON_1);     gamepadSendReportFlag   = true;  break;  
                    case 5: hid->gamepad->press(BUTTON_2);     gamepadSendReportFlag   = true;  break;  
                    case 6: hid->gamepad->press(BUTTON_11);    gamepadSendReportFlag   = true;  break;  
                   default: break;                                     
                }
            }
        else{
                switch (useMode) { 
                    case 1: hid->gamepad->release(BUTTON_11);  gamepadSendReportFlag   = true;  break;  
                    case 2: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  
                    case 3: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  
                    case 4: hid->gamepad->release(BUTTON_1);   gamepadSendReportFlag   = true;  break;  
                    case 5: hid->gamepad->release(BUTTON_2);   gamepadSendReportFlag   = true;  break;  
                    case 6: hid->gamepad->release(BUTTON_11);  gamepadSendReportFlag   = true;  break;  
                   default: break;                                     
                }
            }
}




void sendBTcommandFlipperLinks(bool inputMode){   
    
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){  
        switch (useMode) { 
            case 1: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyPress(KEY_U);      keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->press(BUTTON_7);       gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
    else{           
        switch (useMode) { 
            case 1: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyRelease(KEY_U);    keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->release(BUTTON_7);     gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
}




void sendBTcommandFlipperLinksSecondKey(bool inputMode){   
    
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){  
        switch (useMode) { 
            case 1: hid->gamepad->setRZ(32767);           gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->press(BUTTON_7);        gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->press(BUTTON_15);       gamepadSendReportFlag   = true;  break;  
           default: break;                                         
        }
    }
    else{           
        switch (useMode) { 
            case 1: hid->gamepad->setRZ(0);               gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->release(BUTTON_7);      gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->release(BUTTON_15);     gamepadSendReportFlag   = true;  break;  
           default: break;                                         
        }
    }
}




void sendBTcommandFlipperRechts(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 

    
    
    if(inputMode){
        switch (useMode) { 
            
            case 1: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyPress(KEY_6);       keyboardSendReportFlag  = true;  break;   
            case 6: hid->gamepad->press(BUTTON_8);        gamepadSendReportFlag   = true;  break;  
           default: break;                                        
        }
    }
    else{
        switch (useMode) { 
            
            case 1: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyRelease(KEY_6);     keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->release(BUTTON_8);      gamepadSendReportFlag   = true;  break;  
           default: break;                                        
        }
    }
}




void sendBTcommandFlipperRechtsSecondKey(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 

    if(inputMode){
        switch (useMode) { 
            
            case 1: hid->gamepad->setZ(32767);           gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->press(BUTTON_8);       gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->setZ(0);               gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->release(BUTTON_8);     gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
}





void sendBTcommandTiltFront(bool inputMode, int analogValue ){ 
        
        
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        if(inputMode){
            switch (useMode) { 
                
                
                case 1: hid->gamepad->setLeftThumb(hid->gamepad->lx ,-analogValue);   gamepadSendReportFlag   = true;  break;  
                case 2: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  
                case 3: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  
                case 4: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  
                case 5: hid->keyboard->keyPress(KEY_A);                               keyboardSendReportFlag  = true;  break;  
                case 6: gamepadYfinal =-analogValue;                                  gamepadSendReportFlag   = true;  break;  
               default: break;                                                                
            }
        }
        else{
            switch (useMode) { 
                
                case 1: hid->gamepad->setLeftThumb(hid->gamepad->lx , 0);             gamepadSendReportFlag   = true;  break;  
                case 2: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  
                case 3: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  
                case 4: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  
                case 5: hid->keyboard->keyRelease(KEY_A);                             keyboardSendReportFlag  = true;  break;  
                case 6: gamepadYfinal = 0;                                            gamepadSendReportFlag   = true;  break;  
               default: break;                                                                
            }
        }
    }
    



void sendBTcommandTiltLeft(bool inputMode, int analogValue){ 
   
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        
        if(inputMode){
            switch (useMode) { 
                
                case 1: hid->gamepad->setLeftThumb(analogValue, hid->gamepad->ly);    gamepadSendReportFlag   = true; break;  
                case 2: hid->gamepad->setLeftThumb(-analogValue,0);                   gamepadSendReportFlag   = true; break;  
                case 3: hid->gamepad->setLeftThumb(0,-analogValue);                   gamepadSendReportFlag   = true; break;  
                case 4: hid->gamepad->setLeftThumb(0,-analogValue);                   gamepadSendReportFlag   = true; break;  
                case 5: hid->keyboard->keyPress(KEY_F);                               keyboardSendReportFlag  = true; break;  
                case 6: hid->gamepad->setLeftThumb(analogValue,0);                    gamepadSendReportFlag   = true; break;  
               default: break;                                                               
            }
        }
        else{
            switch (useMode) { 
                case 1: hid->gamepad->setLeftThumb(0 , hid->gamepad->ly);             gamepadSendReportFlag   = true; break;  
                case 2: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  
                case 3: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  
                case 4: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  
                case 5: hid->keyboard->keyRelease(KEY_F);                             keyboardSendReportFlag  = true; break;  
                case 6: hid->gamepad->setLeftThumb(0,0);                              gamepadSendReportFlag   = true; break;  
               default: break;                                                               
            }
        }
}
  



void sendBTcommandTiltRight(bool inputMode, int analogValue){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->setLeftThumb(-analogValue, hid->gamepad->ly);      gamepadSendReportFlag   = true; break;  
            case 2: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  
            case 3: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  
            case 4: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  
            case 5: hid->keyboard->keyPress(KEY_D);                                  keyboardSendReportFlag  = true; break;  
            case 6: hid->gamepad->setLeftThumb(-analogValue,0);                      gamepadSendReportFlag   = true; break;  
           default: break;                                                                   
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->setLeftThumb(0, hid->gamepad->ly);                 gamepadSendReportFlag   = true; break;  
            case 2: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  
            case 3: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  
            case 4: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  
            case 5: hid->keyboard->keyRelease(KEY_D);                                keyboardSendReportFlag  = true; break;  
            case 6: hid->gamepad->setLeftThumb(0,0);                                 gamepadSendReportFlag   = true; break;  
           default: break;                                                                   
        }
    }
}





void sendBTcommandAngleTiltButtonLeft(bool inputMode, int analogValue = 0){ 
        
        int8_t useMode = emulationMode;  
        if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
        
        
        if(inputMode){
            switch (useMode) { 
                
                case 1: hid->gamepad->setHat(6);                      gamepadSendReportFlag   = true;  break; 
                case 2: hid->gamepad->setLeftThumb(-analogValue,0);   gamepadSendReportFlag   = true;  break; 
                case 3: hid->gamepad->press(BUTTON_1);                gamepadSendReportFlag   = true;  break; 
                case 4: hid->gamepad->press(BUTTON_1);                gamepadSendReportFlag   = true;  break; 
                case 5: hid->gamepad->press(BUTTON_2);                gamepadSendReportFlag   = true;  break; 
                case 6: hid->gamepad->setLeftThumb(-analogValue,0);   gamepadSendReportFlag   = true;  break; 
               default: break;                                               
            }
        }
        else{
            switch (useMode) { 
                case 1: hid->gamepad->setHat(8);                      gamepadSendReportFlag   = true;  break;  
                case 2: hid->gamepad->setLeftThumb(0,0);              gamepadSendReportFlag   = true;  break;  
                case 3: hid->gamepad->release(BUTTON_1);              gamepadSendReportFlag   = true;  break;  
                case 4: hid->gamepad->release(BUTTON_1);              gamepadSendReportFlag   = true;  break;  
                case 5: hid->gamepad->release(BUTTON_2);              gamepadSendReportFlag   = true;  break;  
                case 6: hid->gamepad->release(BUTTON_2);              gamepadSendReportFlag   = true;  break;  
               default: break;                                                
            }
        }
    }





void sendBTcommandAngleTiltButtonRight(bool inputMode, int analogValue = 0){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->setHat(2);                          gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(analogValue,0);        gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_1);                    gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_1);                    gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->press(BUTTON_2);                    gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->press(BUTTON_2);                    gamepadSendReportFlag   = true;  break;  
           default: break;                                                  
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->setHat(8);                          gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_1);                  gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_1);                  gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->release(BUTTON_2);                  gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->release(BUTTON_2);                  gamepadSendReportFlag   = true;  break;  
           default: break;                                                  
        }
    }
}





void sendBTcommandAngleTiltButtonUp(bool inputMode, int analogValue = 0){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->setHat(0);                          gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->setLeftThumb(0,-analogValue);       gamepadSendReportFlag   = true;  break;  
           default: break;                                                    
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->setHat(8);                          gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->setLeftThumb(0,0);                  gamepadSendReportFlag   = true;  break;  
           default: break;                                                    
        }
    }
}





void sendBTcommandAngleTiltButtonDown(bool inputMode, int analogValue = 0){ 
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->setHat(4);                           gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->setLeftThumb(0,analogValue);         gamepadSendReportFlag   = true;  break;  
           default: break;                                                     
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->setHat(8);                           gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  
            case 5: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  
            case 6: hid->gamepad->setLeftThumb(0,0);                   gamepadSendReportFlag   = true;  break;  
           default: break;                                                     
        }
    }
}



void sendBTcommandActionKey(bool inputMode){   
    
    int8_t useMode = emulationMode;  
    if(emulationModeOverride > 0)  useMode = emulationModeOverride; 
    if(inputMode){
        switch (useMode) { 
            case 1: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->press(BUTTON_3);       gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->press(BUTTON_1);       gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyPress(KEY_5);      keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->press(BUTTON_2);       gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
    else{
        switch (useMode) { 
            case 1: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  
            case 2: hid->gamepad->release(BUTTON_3);     gamepadSendReportFlag   = true;  break;  
            case 3: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  
            case 4: hid->gamepad->release(BUTTON_1);     gamepadSendReportFlag   = true;  break;  
            case 5: hid->keyboard->keyRelease(KEY_5);    keyboardSendReportFlag  = true;  break;  
            case 6: hid->gamepad->release(BUTTON_2);     gamepadSendReportFlag   = true;  break;  
           default: break;                                       
        }
    }
}



inline bool readPinLow(uint8_t pin) {  
    return !(GPIO.in & (1 << pin));
}



void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
  
  

  if (len == sizeof(KeyEvent)) {
    KeyEvent ev;
    memcpy(&ev, incomingData, sizeof(ev));

    if (ev.state <= 1) {  
      espnowAirButtonCurrentState = ev.state;  
      
      
      
      
      
      
      

    }
  } else {
    
    
    
    
    
    
    
}
}













void setup() {

    

    if(dbglvl) Serial.begin(115200);  

    if(dbglvl) Serial.println("enter setup()");

    hid = new GamepadKeyboardHID(DEVICE_NAME, DEVICE_MANUFACTURER);








//!TODO:   

  
  if(dbglvl){ 
            uint8_t myMAC[6];
            esp_read_mac(myMAC, ESP_MAC_WIFI_STA);
            Serial.printf("esp_read_mac(): %02X:%02X:%02X:%02X:%02X:%02X\n",
                            myMAC[0], myMAC[1], myMAC[2], myMAC[3], myMAC[4], myMAC[5]);
            }


WiFi.mode(WIFI_STA);



if(dbglvl){
            uint8_t macSTA[6];
            esp_wifi_get_mac(WIFI_IF_STA, macSTA);
            Serial.printf("esp_wifi_get_mac(): %02X:%02X:%02X:%02X:%02X:%02X\n",
            macSTA[0], macSTA[1], macSTA[2], macSTA[3], macSTA[4], macSTA[5]);
}
WiFi.disconnect(); 



  
  if (esp_now_init() != ESP_OK) {
    if(dbglvl) Serial.println("ESP-NOW Init fehlgeschlagen");
    return;
  }
  if(dbglvl) Serial.println("ESP-NOW initialisiert");

  
  esp_now_register_recv_cb(OnDataRecv);

  
  wasConnected = false;
  lastPacketTime = millis();







esp_now_peer_info_t peerInfo = {};
memcpy(peerInfo.peer_addr, senderMac, 6);
peerInfo.channel = 0;
peerInfo.encrypt = false;

if (esp_now_add_peer(&peerInfo) != ESP_OK) {
  if(dbglvl) Serial.println("Fehler: Peer konnte nicht hinzugefügt werden");

} else {
  if(dbglvl) Serial.println("Peer hinzugefügt (Sender)");

}








    
    if(dbglvl>1) Serial.println("Initializing I2C...");
    I2C_1.begin(TOUCH_SDA, TOUCH_SCL, 400000);

    
    if(dbglvl>1) Serial.println("Initializing TFT...");
    tft.init();
    

    ui = new GUI(tft, hid->gamepad, hid->keyboard);
    

    
    if(dbglvl>1) Serial.println("Checking GT911 connection...");
    if (isGT911Connected()) {
        if(dbglvl>1) {
            Serial.println("GT911 detected on I2C bus.");
            Serial.println("Initializing GT911...");
        }
        if (touch.begin(TOUCH_INT, TOUCH_RST, GT911_I2C_ADDR, 100000)) {
            if(dbglvl>1) Serial.println("GT911 initialized successfully.");
            touch.setRotation(GT911::Rotate::_0);
            ui->setTouch(&touch);  
        } else {
            if(dbglvl>1) Serial.println("ERROR: GT911 initialization failed!");
        }
    } else {
        if(dbglvl>1) Serial.println("ERROR: GT911 not found on I2C bus!");
    }
    

    
    if(dbglvl>1) Serial.println("Initializing Gyro Sensor...");
      
      Wire.begin(I2C_SDA, I2C_SCL, 1000000);
      delay(50);

      
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);
      Wire.write(0xB6); 
      Wire.endTransmission();
      delay(50);
    
      Wire.beginTransmission(BMI160_ADDR);     
      Wire.write(0x40);                        
      Wire.write(0b00001011);                  
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);     
      Wire.write(0x11);     
      Wire.endTransmission();
      delay(50);
    
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E); 
      Wire.write(0x15);     
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x41);
      Wire.write(0x0F);     
      Wire.endTransmission();
      delay(50);
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x43);
      Wire.write(0x00);     
      Wire.endTransmission();
      delay(50);
    
      
      Wire.beginTransmission(BMI160_ADDR);
      Wire.write(0x7E);
      Wire.write(0x37);     
      Wire.endTransmission();
      delay(500);
    
    power_mutex = xSemaphoreCreateMutex();   
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
   
     
    
    ui->begin();           
    ui->setTouch(&touch);  

   
     ui->setGamepad(hid->gamepad);


    if(dbglvl>1) Serial.println("Initializing PWM...");
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(BACKLIGHT_PIN, PWM_CHANNEL);
    
    
    if(dbglvl>1) Serial.println("Showing intro...");
    ui->intro();



   
    
    pinMode(ioPinSideLeft,  INPUT_PULLUP); 
    pinMode(ioPinSideRight, INPUT_PULLUP); 
    pinMode(ioPinFrontLeft,  INPUT);       
    pinMode(ioPinFrontRight, INPUT);       

    
    
    
    esp_sleep_enable_ext0_wakeup((gpio_num_t) ioPinFrontLeft, 0);  

    if(dbglvl>1) Serial.println("Setup complete.");


}











 
void loop() {  
    loopCounter++;                      

    _isBleConnected = isBleConnected();
    milliTimeCopy = millis();           
    
    
    
    
    
    gamepadSendReportFlag   = false;    
    keyboardSendReportFlag  = false;    


 

        if (UIintervalTimerFlag <= milliTimeCopy) {    
        UIintervalTimerFlag  = milliTimeCopy + UIinterval;
        ui->UIupdate(loopsPerSecond, milliTimeCopy);            
}  
   



    if (timeTrapOneSecond <= milliTimeCopy) {                     
        timeTrapOneSecond =  milliTimeCopy + 1000;                
        loopsPerSecond = loopCounter;                             
        loopCounter = 0;                                          
        


    
        
    if(milliTimeCopy > sleepTimerMillis){esp_deep_sleep_start();}  
    
    if(milliTimeCopy > stdMenuTimeMillis && stdMenu != UImenu)
            {
                ui->UIclearScreen = 1; 
                UImenu = stdMenu;
                
                stdMenuTimeMillis = milliTimeCopy + stdMenuTime * 1000;    
            }
}  
    



    if(gyroUpdateTimeTrapTimerFlag <= milliTimeCopy ){         
       gyroUpdateTimeTrapTimerFlag =  milliTimeCopy + gyroTimeTrapTimerCycle ;

    
    int powerCalculatedLeft = 0, powerCalculatedRight = 0, powerCalculatedUp = 0;
    xSemaphoreTake(power_mutex, portMAX_DELAY);

    
    
    if(!secondKeyButtonFlag){  
          
            
            if(power.left && !power.cnt_left) {
                powerCalculatedLeft = power.left * tiltGain;              
                if(powerCalculatedLeft > 32767) powerCalculatedLeft = 32767; 
                
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(1, powerCalculatedLeft);  
                power.left = 0;  
            }
            if(power.right && !power.cnt_right) {
                powerCalculatedRight = power.right * tiltGain;            
                if(powerCalculatedRight > 32767) powerCalculatedRight = 32767; 
                
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(2, powerCalculatedRight);  
                power.right = 0;
            }
            if(power.up && !power.cnt_up) {
                powerCalculatedUp = power.up * tiltGain;                   
                if(powerCalculatedUp > 32767) powerCalculatedUp = 32767; 
                
                tiltCounterGlob++;
                if(UImenu == 1) ui->drawPeakMeterNudgeDirection(3, powerCalculatedUp);
                power.up = 0;
                }

    
    }  
  
    xSemaphoreGive(power_mutex);
    

 static bool release_sendBTcommandTiltLeft  = 0;
 static bool release_sendBTcommandTiltRight = 0;
 static bool release_sendBTcommandTiltUp    = 0;



if(release_sendBTcommandTiltLeft || release_sendBTcommandTiltRight || release_sendBTcommandTiltUp) 
{ 

    if(release_sendBTcommandTiltLeft){
        release_sendBTcommandTiltLeft=0;
        sendBTcommandTiltLeft(0, 0);     
        
        }
    if(release_sendBTcommandTiltRight){
        release_sendBTcommandTiltRight=0;
        sendBTcommandTiltRight(0, 0);    
        
    }
    if(release_sendBTcommandTiltUp){     
        release_sendBTcommandTiltUp=0;
        sendBTcommandTiltFront(0, 0);
        
        }
    }



 if (powerCalculatedLeft || powerCalculatedRight || powerCalculatedUp)  
      {
            
            if(powerCalculatedUp){
                        sendBTcommandTiltFront(1, powerCalculatedUp);
                        release_sendBTcommandTiltUp    = 1;
            };
                 
            
            if(powerCalculatedLeft  > powerCalculatedRight ){  
                        sendBTcommandTiltLeft(1, powerCalculatedLeft);
                        release_sendBTcommandTiltLeft  = 1;
            };

            if(powerCalculatedRight > powerCalculatedLeft){
                        sendBTcommandTiltRight(1, powerCalculatedRight);
                        release_sendBTcommandTiltRight = 1;
            };

        if(dbglvl)Serial.printf("Nudge TRY TO SEND:%s%s%s\n",release_sendBTcommandTiltUp?" UP":"", release_sendBTcommandTiltLeft?" LEFT":"",release_sendBTcommandTiltRight?" RIGHT":"");
      }





    
 
    
    float x = -power.accel_x / 2048.0f;    
    float y = -power.accel_y / 2048.0f;    
    float z = -power.accel_z / 2048.0f;  

    pitch = atan2(-x, sqrt(y * y + z * z)) * 180.0 / PI;
    roll  = atan2(y, z) * 180.0 / PI;

    
    if (roll > 90) roll = 180 - roll;
    else if (roll < -90) roll = -180 - roll;

}   




if(secondKeyButtonFlag)
{



        if(roll < -angleTrigger)  
        {
        keyTimerFlagAngleUp = (milliTimeCopy + 100);
        }        

        if(roll > angleTrigger)  
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




        if(keyTimerFlagAngleUp > milliTimeCopy){  
            
            if (flipFlopFlagAngleUp == 0){  
                                                sendBTcommandAngleTiltButtonDown(1, 32767); 
                                                ui->drawVirtualTiltingJoystickKeys(1,1);
                                                flipFlopFlagAngleUp = 1;
                                            }
            }
            else
                {   
                if (flipFlopFlagAngleUp == 1){   
                                                sendBTcommandAngleTiltButtonDown(0);
                                                ui->drawVirtualTiltingJoystickKeys(1,0);
                                                flipFlopFlagAngleUp = 0;
                                                }
            }



        if(keyTimerFlagAngleDown > milliTimeCopy){  
        
                if (flipFlopFlagAngleDown == 0){  
                                                sendBTcommandAngleTiltButtonUp(1, 32767); 
                                                ui->drawVirtualTiltingJoystickKeys(2,1);                                            
                                                flipFlopFlagAngleDown = 1;
                                                }
            }
            else
                {   
                if (flipFlopFlagAngleDown == 1){   
                                                sendBTcommandAngleTiltButtonUp(0);                                
                                                ui->drawVirtualTiltingJoystickKeys(2,0);
                                                flipFlopFlagAngleDown = 0;
                                                }
        }



        if(keyTimerFlagAngleLeft > milliTimeCopy){  
            
            if (flipFlopFlagAngleLeft == 0){  
                                            sendBTcommandAngleTiltButtonRight(1, 32767);                            
                                            ui->drawVirtualTiltingJoystickKeys(3,1);
                                            flipFlopFlagAngleLeft = 1;
                                            }
            }
        else
            {   
            if (flipFlopFlagAngleLeft == 1){   
                                            sendBTcommandAngleTiltButtonRight(0);                                
                                            ui->drawVirtualTiltingJoystickKeys(3,0);
                                            flipFlopFlagAngleLeft = 0;
                                            }
        }



        if(keyTimerFlagAngleRight > milliTimeCopy){  
            
            if (flipFlopFlagAngleRight == 0){  
                                                sendBTcommandAngleTiltButtonLeft(1, 32767);
                                                ui->drawVirtualTiltingJoystickKeys(4,1);
                                                flipFlopFlagAngleRight = 1;
                                            }
            }
            else
                {   
                if (flipFlopFlagAngleRight == 1){   
                                                sendBTcommandAngleTiltButtonLeft(0); 
                                                ui->drawVirtualTiltingJoystickKeys(4,0);
                                                flipFlopFlagAngleRight = 0;
                                                }
        }
}




if (readPinLow( ioPinSideLeft   )) keyTimerFlagSideLeft   = milliTimeCopy + debounceKey;
if (readPinLow( ioPinSideRight  )) keyTimerFlagSideRight  = milliTimeCopy + debounceKey;
if (readPinLow( ioPinFrontLeft  )) keyTimerFlagFrontLeft  = milliTimeCopy + debounceKey;
if (readPinLow( ioPinFrontRight )) keyTimerFlagFrontRight = milliTimeCopy + debounceKey;





if(secondKeySetLaterRelease && secondKeySetLaterReleaseTimerFlag < milliTimeCopy){
    sendBTcommandPlungerLinks(0);      
    secondKeySetLaterRelease = false;  
    
}


if(keyTimerFlagFrontLeft > milliTimeCopy){ 
    
    if (flipFlopFlagFrontLeft == 0){    
                                        if(UImenu == 1) {ui->drawPhysicalVirtualKeys(2,1);    
                                                         ui->drawPhysicalVirtualKeys(6,0);}
                                        
                                        secondKeyButtonFlag = 0;  
                                        secondKeyButtonTimeMark = milliTimeCopy;                           
                                        flipFlopFlagFrontLeft = 1;
   }
}
else
    {                                   
    if (flipFlopFlagFrontLeft == 1){    
                                    
                                    if( milliTimeCopy - secondKeyButtonTimeMark < secondKeyActivationTime )  
                                      { 
                                        sendBTcommandPlungerLinks(1);  
                                        
                                        secondKeySetLaterRelease = true;                                   
                                        secondKeySetLaterReleaseTimerFlag = milliTimeCopy + 100;
                                        secondKeyButtonFlag = 0;  
                                    } 
                                    
                                    hid->gamepad->setHat(8);                                               
                                    gamepadSendReportFlag = 1;  

                                    if(UImenu == 1) {ui->drawPhysicalVirtualKeys(6,0); 
                                                     ui->drawPhysicalVirtualKeys(2,0);
                                                     ui->fillSpriteBackground(); 
                                                     ui->drawVirtualTiltingJoystickKeys(1,0);  
                                                     ui->drawVirtualTiltingJoystickKeys(2,0);
                                                     ui->drawVirtualTiltingJoystickKeys(3,0);
                                                     ui->drawVirtualTiltingJoystickKeys(4,0);
                                                    }
                                    flipFlopFlagFrontLeft = 0;
                                     
    }
}


if( milliTimeCopy - secondKeyButtonTimeMark >= secondKeyActivationTime && flipFlopFlagFrontLeft == 1){
    secondKeyButtonFlag = 1;
    if(UImenu == 1) ui->drawPhysicalVirtualKeys(6,1);
    
    }
 else 
   {secondKeyButtonFlag = 0;}














static bool releaseTrickFlagFrontR;
if(keyTimerFlagFrontRight > milliTimeCopy){                                                
   
    if (flipFlopFlagFrontRight == 0){                                                      
                                    if(!secondKeyButtonFlag){                              
                                        if(CheatLockRecordMode == 1){
                                        keyAbenchmarkTimeMark = millis();                  
                                        sendBTcommandPlungerRechts(1); 
                                        }                        
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(1,1);
                                        releaseTrickFlagFrontR = 0;                        
                                    }
                                    else{                                                  
                                        sendBTcommandPlungerRechtsSecondKey(1);                        
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(5,1);
                                        releaseTrickFlagFrontR = 1;                        
                                    }
                                    flipFlopFlagFrontRight = 1;
                                    }
      }
else
    {                                                                                      
    if (flipFlopFlagFrontRight == 1){                                                      
                                    if(!releaseTrickFlagFrontR){                           
                if(CheatLockRecordMode == 1){
                                        skillShotMillisSend = millis() - keyAbenchmarkTimeMark;  
                                        sendBTcommandPlungerRechts(0); 
                                        }
                                        if(CheatLockRecordMode == 2) sendTimedPlungerButtonA = true;   
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(1,0);
                                    }
                                    else{                                                  
                                        sendBTcommandPlungerRechtsSecondKey(0);                       
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(5,0);
                                    }
                                    flipFlopFlagFrontRight = 0;
    }
}













static bool releaseTrickFlagSideL;
if(keyTimerFlagSideLeft > milliTimeCopy){                                                  
    
    if (flipFlopFlagSideLeft == 0){    
                                    if(!secondKeyButtonFlag){                              
                                        sendBTcommandFlipperLinks(1); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(7,0);
                                        releaseTrickFlagSideL = 0;                         
                                    }
                                    else{                                                  
                                        sendBTcommandFlipperLinksSecondKey(1); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,1);
                                        releaseTrickFlagSideL = 1;                         
                                    }
                                    ButtonFlipperLeftCounterToday++;
                                    ButtonFlipperLeftCounterAlltime++;
                                    flipFlopFlagSideLeft = 1;
                                    }
      }
else
    {                                                                                      
    if (flipFlopFlagSideLeft == 1){   
                                    if(!releaseTrickFlagSideL){                            
                                        sendBTcommandFlipperLinks(0); 
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(7,1);
                                    }
                                    else{                                                  
                                        sendBTcommandFlipperLinksSecondKey(0);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(3,0);
                                    }
                                    flipFlopFlagSideLeft = 0;
    }
}




static bool releaseTrickFlagSideR;
if(keyTimerFlagSideRight > milliTimeCopy){                                                 
    
    if (flipFlopFlagSideRight == 0){  
                                    if(!secondKeyButtonFlag){                              
                                        sendBTcommandFlipperRechts(1);                     
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(8,0);  
                                        releaseTrickFlagSideR = 0;                         
                                    }
                                    else{                                                  
                                        sendBTcommandFlipperRechtsSecondKey(1);
                                        if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,1);
                                        releaseTrickFlagSideR = 1;                         
                                    }
                                    ButtonFlipperRightCounterToday++;
                                    ButtonFlipperRightCounterAlltime++;
                                    flipFlopFlagSideRight = 1;
                                    }
      }
else
    {   
    if (flipFlopFlagSideRight == 1){   
                                    if(!releaseTrickFlagSideR){                             
                                       sendBTcommandFlipperRechts(0); 
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(8,1);    
                                    }
                                    else{                                                   
                                       sendBTcommandFlipperRechtsSecondKey(0);
                                       if(UImenu == 1) ui->drawPhysicalVirtualKeys(4,0);
                                    }
                                    flipFlopFlagSideRight = 0;
                                    }
}






if(sendTimedPlungerButtonA && sendTimedPlungerButtonATimerReleaseFlag == 0){    

ms = millis();if(dbglvl) Serial.printf("[%lu.%03lu] skill shot virtual key [] triggered. set virtual key time flag\n", ms/1000,ms%1000);  
   sendBTcommandPlungerRechts(1);    
   if(UImenu==4)ui->drawSkillShotButton(2);        
   if(UImenu==1)ui->drawPhysicalVirtualKeys(1,2); 
   
   if(dbglvl){ ms = millis();Serial.printf("[%lu.%03lu]  skillshot time trap sendTimedPlungerButtonA set, sendBTcommandPlungerRechts(1)\n", ms/1000,ms%1000);}  
sendTimedPlungerButtonATimerReleaseFlag = ms + skillShotMillisSend; 
}


if(sendTimedPlungerButtonATimerReleaseFlag != 0 && sendTimedPlungerButtonATimerReleaseFlag <= millis()){  
   sendTimedPlungerButtonATimerReleaseFlag = 0;   
   ms = millis();
   if(dbglvl) Serial.printf("[%lu.%03lu] skill shot release time trap sendTimedPlungerButtonA vor GFX darstellung\n",ms/1000,ms%1000);
   sendBTcommandPlungerRechts(0);
   if(UImenu==4)ui->drawSkillShotButton(3);       
   if(UImenu==1)ui->drawPhysicalVirtualKeys(1,0);
   sendTimedPlungerButtonA = false;  
   if(dbglvl) {ms = millis(); Serial.printf("[%lu.%03lu] skill shot release time trap sendTimedPlungerButtonA release, sendBTcommandPlungerRechts(0)\n", ms/1000,ms%1000);}  
}



if(gamepadSendReportFlag){
   gamepadSendReportFlag = false;
   if(dbglvl) Serial.printf("[%lu.%03lu] milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000); 

   hid->gamepad->sendGamepadReport();     
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; 
   if(dbglvl){ ms = millis();Serial.printf("[%lu.%03lu] realtime millis() nach gamepad report senden\n", ms/1000,ms%1000); } 
}

if(keyboardSendReportFlag) {
   keyboardSendReportFlag = false; 
   if(dbglvl) Serial.printf("[%lu.%03lu] milliTimeCopy (round start time)\n", milliTimeCopy/1000,milliTimeCopy%1000);  

   hid->keyboard->sendKeyReport();        
   sleepTimerMillis  = milliTimeCopy + sleepTimer  * 60000; 
   if(dbglvl){ms = millis(); Serial.printf("[%lu.%03lu] realtime millis() nach keyboard report senden\n", ms/1000,ms%1000); } 
}






















}

