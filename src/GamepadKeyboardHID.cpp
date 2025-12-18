
#include "GamepadKeyboardHID.h"
#include <cstring> // memcpy
#include <WiFi.h>  // to disable it to have less interference

extern int dbglvl;

extern bool _isBleConnected;

// HID Report Map: Gamepad (ID 1) + Keyboard (ID 2)
static const uint8_t reportMap[] = {

    // Gamepad
    0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
    0x09, 0x05,        //   USAGE (Gamepad)
    0xA1, 0x01,        //   COLLECTION (Application)
    0x85, 0x01,        //   REPORT_ID (1)

    // 16 Buttons
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
    0x29, 0x10,        //   USAGE_MAXIMUM (Button 16)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x10,        //   REPORT_COUNT (16)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)

    // Hat switch (D-Pad)
    0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
    0x09, 0x39,        //   USAGE (Hat switch)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x07,        //   LOGICAL_MAXIMUM (7)
    0x35, 0x00,        //   PHYSICAL_MINIMUM (0)
    0x46, 0x3B, 0x01,  //   PHYSICAL_MAXIMUM (315)
    0x65, 0x14,        //   UNIT (Eng Rot:Angular Pos)
    0x75, 0x04,        //   REPORT_SIZE (4)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x75, 0x04,        //   REPORT_SIZE (4) - Padding
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x03,        //   INPUT (Const,Var,Abs)

    // Axes X, Y, Z, RZ
    0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
    0x09, 0x30,        //   USAGE (X)
    0x09, 0x31,        //   USAGE (Y)
    0x09, 0x32,        //   USAGE (Z)
    0x09, 0x35,        //   USAGE (RZ)
    0x16, 0x01, 0x80,  //   LOGICAL_MINIMUM (-32767)
    0x26, 0xFF, 0x7F,  //   LOGICAL_MAXIMUM (32767)
    0x75, 0x10,        //   REPORT_SIZE (16)
    0x95, 0x04,        //   REPORT_COUNT (4)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)

    0xC0,              // END_COLLECTION (Gamepad)


    // Keyboard
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01,
    0x85, 0x02,              // REPORT_ID (2)
    0x05, 0x07,
    0x19, 0xE0, 0x29, 0xE7,  // Modifiers
    0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08,
    0x81, 0x02,              // INPUT (Mod)
    0x95, 0x01, 0x75, 0x08,
    0x81, 0x01,              // Reserved
    0x95, 0x06, 0x75, 0x08,  // 6 keys
    0x15, 0x00, 0x25, 0x65,
    0x05, 0x07, 0x19, 0x00, 0x29, 0x65,
    0x81, 0x00,              // INPUT (Keys)
    0xC0                     // END_COLLECTION (keyboard)
};

GamepadKeyboardHID::GamepadKeyboardHID(const char* name, const char* manufacturer) {
     // 0) WiFi aus (Reduziert Interferenz)
    WiFi.mode(WIFI_OFF);
    btStop(); // stellen wir sicher, dass Classic bluetooth nicht stört
    delay(100);

    NimBLEDevice::init(name);
    NimBLEDevice::setSecurityAuth(true, true, true);  // false false false geht mit 
                                                      // false true  false geht mit
                                                      // true  true  true  geht mit 

    NimBLEServer* pServer = NimBLEDevice::createServer();
    NimBLEHIDDevice* hid = new NimBLEHIDDevice(pServer);

    // Device Information Service (0x180A) – optional, hier nur Manufacturer Name (0x2A29)
    {
        NimBLEService* dis = pServer->createService(BLEUUID((uint16_t)0x180A));
        NimBLECharacteristic* manu = dis->createCharacteristic(
            BLEUUID((uint16_t)0x2A29), NIMBLE_PROPERTY::READ
        );
        manu->setValue(manufacturer);
        dis->start();
    }

    // HID Report Map
    hid->setReportMap((uint8_t*)reportMap, sizeof(reportMap));

    // Den HID-Service vom Server holen (0x1812)
    NimBLEService* hidSvc = pServer->getServiceByUUID(BLEUUID((uint16_t)0x1812));

    // Gamepad Input Report (Report ID 1)
    NimBLECharacteristic* gpInput = hidSvc->createCharacteristic(
        BLEUUID((uint16_t)0x2A4D), // HID Report
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    {
        uint8_t ref[2] = { 1, 1 }; // Report ID = 1, Type = Input(1)
        gpInput->createDescriptor(BLEUUID((uint16_t)0x2908))->setValue(ref, sizeof(ref));
    }

    // Keyboard Input Report (Report ID 2)
    NimBLECharacteristic* kbInput = hidSvc->createCharacteristic(
        BLEUUID((uint16_t)0x2A4D),
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    {
        uint8_t ref[2] = { 2, 1 }; // Report ID = 2, Type = Input(1)
        kbInput->createDescriptor(BLEUUID((uint16_t)0x2908))->setValue(ref, sizeof(ref));
    }

    gamepad  = new GamepadHID(gpInput);
    keyboard = new KeyboardHID(kbInput);

    // Services starten
    hid->startServices();

    // Advertising – HID Service UUID
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();

    adv->setName(std::string(name));                  // ADV_NAME aus Konstruktor
    adv->addServiceUUID(BLEUUID((uint16_t)0x1812));   // HID Service
    adv->setManufacturerData(std::string(manufacturer));
    adv->setAppearance(0x03C0);                       // Generic HID
    adv->enableScanResponse(true);                    // Name auch im Scan Response
    adv->setMinInterval(96);                          // ~60 ms
    adv->setMaxInterval(160);                         // ~100 ms
    adv->start(0);





}

// --- GamepadHID -------------------------------------------------------------------
GamepadHID::GamepadHID(NimBLECharacteristic* input) : inputChar(input) {}

void GamepadHID::press(uint16_t buttonMask) {
    buttons |= buttonMask; // kein Auto-Send
}

void GamepadHID::release(uint16_t buttonMask) {
    buttons &= ~buttonMask; // kein Auto-Send
}

void GamepadHID::setLeftThumb(int16_t x, int16_t y) {
    lx = x;
    ly = y; // kein Auto-Send
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


    uint8_t report[12]; // 2 Bytes Buttons + 1 Byte Hat+Pad + 8 Bytes Axes = 11 Bytes
                    // aber wir runden auf 12 für sauberes Alignment

    // Buttons
    report[0] = buttons & 0xFF;
    report[1] = (buttons >> 8) & 0xFF;

    // Hat switch (4 Bit) + Padding (4 Bit)
    report[2] = (hat <= 7 ? hat : 0x0F) | 0xF0; // 0xF = neutral

    // Axes
    report[3] = lx & 0xFF;
    report[4] = (lx >> 8) & 0xFF;
    report[5] = ly & 0xFF;
    report[6] = (ly >> 8) & 0xFF;
    report[7] = z & 0xFF;
    report[8] = (z >> 8) & 0xFF;
    report[9] = rz & 0xFF;
    report[10] = (rz >> 8) & 0xFF;

    // optional Padding Byte
    report[11] = 0;


    inputChar->setValue(report, sizeof(report));
    if(_isBleConnected) inputChar->notify();

    if(dbglvl){    // [%d] time
        ms=millis();
        Serial.printf("[%lu.%03lu real] sendGamepadReport() %s", ms/1000,ms%1000, _isBleConnected ? "BT connection okay. gamepad sent. [ " : "not send!!! no BT connection? [ ");
        for(int i=0; i < sizeof(report); i++){
            Serial.print(report[i]);
            Serial.print(" ");
        }
        
        Serial.println("]");
    }
    
}

// --- KeyboardHID -------------------------------------------------------------------
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
    uint8_t report[8] = {0}; // [0]=Modifiers, [1]=Reserved, [2..7]=6 Keys
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












