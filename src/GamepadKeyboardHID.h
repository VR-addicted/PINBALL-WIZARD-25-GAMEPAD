
#pragma once
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>

// Vorwärtsdeklarationen
class GamepadHID;
class KeyboardHID;

class GamepadKeyboardHID {
public:
    GamepadKeyboardHID(const char* name, const char* manufacturer);
    GamepadHID* gamepad;
    KeyboardHID* keyboard;
};

// --- Gamepad ---
class GamepadHID {
public:
    GamepadHID(NimBLECharacteristic* input);

    // Buttons setzen/löschen (nur interner Zustand, kein automatisches Senden)
    void press(uint16_t buttonMask);
    void release(uint16_t buttonMask);

    // Linken Stick setzen (-32767..32767)
    void setLeftThumb(int16_t x, int16_t y);

    // Report explizit senden
    void sendGamepadReport();

    void setZ(int16_t value);
    void setRZ(int16_t value);

    void setHat(uint8_t direction); // 0–7, 8 = neutral
    uint8_t hat = 8;                // Default neutral

   
    int16_t lx = 0, ly = 0;
    int16_t z = 0, rz = 0;


private:
    NimBLECharacteristic* inputChar;
    uint16_t buttons = 0;
    // int16_t lx = 0, ly = 0;
    // int16_t z = 0, rz = 0;
};

// --- Keyboard ---
class KeyboardHID {
public:
    KeyboardHID(NimBLECharacteristic* input);

    // Keys setzen/löschen (nur interner Zustand, kein automatisches Senden)
    void keyPress(uint8_t keycode);
    void keyRelease(uint8_t keycode);

    // Report explizit senden
    void sendKeyReport();

private:
    NimBLECharacteristic* inputChar;
    uint8_t keys[6] = {0}; // max. 6 gleichzeitige normale Keys
};


