
#include "GamepadKeyboardHID.h"
#include <cstring> 
#include <WiFi.h>  

extern int dbglvl;

extern bool _isBleConnected;


static const uint8_t reportMap[] = {

    
    0x05, 0x01,        
    0x09, 0x05,        
    0xA1, 0x01,        
    0x85, 0x01,        

    
    0x05, 0x09,        
    0x19, 0x01,        
    0x29, 0x10,        
    0x15, 0x00,        
    0x25, 0x01,        
    0x75, 0x01,        
    0x95, 0x10,        
    0x81, 0x02,        

    
    0x05, 0x01,        
    0x09, 0x39,        
    0x15, 0x00,        
    0x25, 0x07,        
    0x35, 0x00,        
    0x46, 0x3B, 0x01,  
    0x65, 0x14,        
    0x75, 0x04,        
    0x95, 0x01,        
    0x81, 0x02,        
    0x75, 0x04,        
    0x95, 0x01,        
    0x81, 0x03,        

    
    0x05, 0x01,        
    0x09, 0x30,        
    0x09, 0x31,        
    0x09, 0x32,        
    0x09, 0x35,        
    0x16, 0x01, 0x80,  
    0x26, 0xFF, 0x7F,  
    0x75, 0x10,        
    0x95, 0x04,        
    0x81, 0x02,        

    0xC0,              


    
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01,
    0x85, 0x02,              
    0x05, 0x07,
    0x19, 0xE0, 0x29, 0xE7,  
    0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08,
    0x81, 0x02,              
    0x95, 0x01, 0x75, 0x08,
    0x81, 0x01,              
    0x95, 0x06, 0x75, 0x08,  
    0x15, 0x00, 0x25, 0x65,
    0x05, 0x07, 0x19, 0x00, 0x29, 0x65,
    0x81, 0x00,              
    0xC0                     
};

GamepadKeyboardHID::GamepadKeyboardHID(const char* name, const char* manufacturer) {
     
    WiFi.mode(WIFI_OFF);
    btStop(); 
    delay(100);

    NimBLEDevice::init(name);
    NimBLEDevice::setSecurityAuth(true, true, true);  
                                                      
                                                      

    NimBLEServer* pServer = NimBLEDevice::createServer();
    NimBLEHIDDevice* hid = new NimBLEHIDDevice(pServer);

    
    {
        NimBLEService* dis = pServer->createService(BLEUUID((uint16_t)0x180A));
        NimBLECharacteristic* manu = dis->createCharacteristic(
            BLEUUID((uint16_t)0x2A29), NIMBLE_PROPERTY::READ
        );
        manu->setValue(manufacturer);
        dis->start();
    }

    
    hid->setReportMap((uint8_t*)reportMap, sizeof(reportMap));

    
    NimBLEService* hidSvc = pServer->getServiceByUUID(BLEUUID((uint16_t)0x1812));

    
    NimBLECharacteristic* gpInput = hidSvc->createCharacteristic(
        BLEUUID((uint16_t)0x2A4D), 
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    {
        uint8_t ref[2] = { 1, 1 }; 
        gpInput->createDescriptor(BLEUUID((uint16_t)0x2908))->setValue(ref, sizeof(ref));
    }

    
    NimBLECharacteristic* kbInput = hidSvc->createCharacteristic(
        BLEUUID((uint16_t)0x2A4D),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    {
        uint8_t ref[2] = { 2, 1 }; 
        kbInput->createDescriptor(BLEUUID((uint16_t)0x2908))->setValue(ref, sizeof(ref));
    }

    gamepad  = new GamepadHID(gpInput);
    keyboard = new KeyboardHID(kbInput);

    
    hid->startServices();

    
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();

    adv->setName(std::string(name));                  
    adv->addServiceUUID(BLEUUID((uint16_t)0x1812));   
    adv->setManufacturerData(std::string(manufacturer));
    adv->setAppearance(0x03C0);                       
    adv->enableScanResponse(true);                    
    adv->setMinInterval(96);                          
    adv->setMaxInterval(160);                         
    adv->start(0);





}


GamepadHID::GamepadHID(NimBLECharacteristic* input) : inputChar(input) {}

void GamepadHID::press(uint16_t buttonMask) {
    buttons |= buttonMask; 
}

void GamepadHID::release(uint16_t buttonMask) {
    buttons &= ~buttonMask; 
}

void GamepadHID::setLeftThumb(int16_t x, int16_t y) {
    lx = x;
    ly = y; 
}

void GamepadHID::setZ(int16_t value) {
    z = value;
}

void GamepadHID::setRZ(int16_t value) {
    rz = value;
}

void GamepadHID::setHat(uint8_t direction) {
    if (direction <= 8) {
        hat = direction;
    }
}

extern unsigned long ms; 
void GamepadHID::sendGamepadReport() {


    uint8_t report[12]; 
                    

    
    report[0] = buttons & 0xFF;
    report[1] = (buttons >> 8) & 0xFF;

    
    report[2] = (hat <= 7 ? hat : 0x0F) | 0xF0; 

    
    report[3] = lx & 0xFF;
    report[4] = (lx >> 8) & 0xFF;
    report[5] = ly & 0xFF;
    report[6] = (ly >> 8) & 0xFF;
    report[7] = z & 0xFF;
    report[8] = (z >> 8) & 0xFF;
    report[9] = rz & 0xFF;
    report[10] = (rz >> 8) & 0xFF;

    
    report[11] = 0;


    inputChar->setValue(report, sizeof(report));
    if(_isBleConnected) inputChar->notify();

    if(dbglvl){    
        ms=millis();
        Serial.printf("[%lu.%03lu real] sendGamepadReport() %s", ms/1000,ms%1000, _isBleConnected ? "BT connection okay. gamepad sent. [ " : "not send!!! no BT connection? [ ");
        for(int i=0; i < sizeof(report); i++){
            Serial.print(report[i]);
            Serial.print(" ");
        }
        
        Serial.println("]");
    }
    
}


KeyboardHID::KeyboardHID(NimBLECharacteristic* input) : inputChar(input) {}

void KeyboardHID::keyPress(uint8_t keycode) {
    for (int i = 0; i < 6; i++) {
        if (keys[i] == 0) { keys[i] = keycode; break; }
    }
}

void KeyboardHID::keyRelease(uint8_t keycode) {
    for (int i = 0; i < 6; i++) {
        if (keys[i] == keycode) { keys[i] = 0; }
    }
}



void KeyboardHID::sendKeyReport() {
    uint8_t report[8] = {0}; 
    std::memcpy(&report[2], keys, sizeof(keys));
    
    inputChar->setValue(report, sizeof(report));
    if(_isBleConnected) inputChar->notify();
    
    if(dbglvl){
        ms = millis();
        Serial.printf("[%lu.%03lu] sendKeyReport() returns: %s", ms/1000,ms%1000 , _isBleConnected ? "keys already sent. [ " : "not send!!! no BT connection? [ ");
        for(int i=0; i < sizeof(report);i++){
            Serial.print(report[i]);
            Serial.print(" ");
        }
        Serial.println("]");
    }

}












