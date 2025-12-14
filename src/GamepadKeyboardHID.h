
#pragma once
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>


class GamepadHID;
class KeyboardHID;

class GamepadKeyboardHID {
public:
    GamepadKeyboardHID(const char* name, const char* manufacturer);
    GamepadHID* gamepad;
    KeyboardHID* keyboard;
};


class GamepadHID {
public:
    GamepadHID(NimBLECharacteristic* input);

    
    void press(uint16_t buttonMask);
    void release(uint16_t buttonMask);

    
    void setLeftThumb(int16_t x, int16_t y);

    
    void sendGamepadReport();

    void setZ(int16_t value);
    void setRZ(int16_t value);

    void setHat(uint8_t direction); 
    uint8_t hat = 8;                

   
    int16_t lx = 0, ly = 0;
    int16_t z = 0, rz = 0;


private:
    NimBLECharacteristic* inputChar;
    uint16_t buttons = 0;
    
    
};


class KeyboardHID {
public:
    KeyboardHID(NimBLECharacteristic* input);

    
    void keyPress(uint8_t keycode);
    void keyRelease(uint8_t keycode);

    
    void sendKeyReport();

private:
    NimBLECharacteristic* inputChar;
    uint8_t keys[6] = {0}; 
};


