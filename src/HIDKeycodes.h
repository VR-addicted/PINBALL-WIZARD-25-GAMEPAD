#pragma once

// HID Usage IDs für Buchstaben A–Z
#define KEY_A 0x04
#define KEY_B 0x05
#define KEY_C 0x06
#define KEY_D 0x07
#define KEY_E 0x08
#define KEY_F 0x09
#define KEY_G 0x0A
#define KEY_H 0x0B
#define KEY_I 0x0C
#define KEY_J 0x0D
#define KEY_K 0x0E
#define KEY_L 0x0F
#define KEY_M 0x10
#define KEY_N 0x11
#define KEY_O 0x12
#define KEY_P 0x13
#define KEY_Q 0x14
#define KEY_R 0x15
#define KEY_S 0x16
#define KEY_T 0x17
#define KEY_U 0x18
#define KEY_V 0x19
#define KEY_W 0x1A
#define KEY_X 0x1B
#define KEY_Y 0x1C
#define KEY_Z 0x1D

// HID Usage IDs für Zahlen 0–9
#define KEY_1 0x1E
#define KEY_2 0x1F
#define KEY_3 0x20
#define KEY_4 0x21
#define KEY_5 0x22
#define KEY_6 0x23
#define KEY_7 0x24
#define KEY_8 0x25
#define KEY_9 0x26
#define KEY_0 0x27

// Zusatztasten (Standard-HID)
#define KEY_ENTER     0x28
#define KEY_ESCAPE    0x29
#define KEY_BACKSPACE 0x2A
#define KEY_TAB       0x2B
#define KEY_SPACE     0x2C

// Optional: Menü / Android-spezifisch (HID Consumer Page oder Android Keycodes)
// Diese sind nicht im Standard-Keyboard-Usage-Table, müssten ggf. separat gesendet werden
#define KEY_MENU     0x76  // Beispielwert, je nach Implementierung anpassen

// Gamepad Button Bitmasken
#define BUTTON_1   0x0001  // Bit 0  BUTTON_A
#define BUTTON_2   0x0002  // Bit 1  BUTTON_B
#define BUTTON_3   0x0004  // Bit 2  BUTTON_X
#define BUTTON_4   0x0008  // Bit 3  BUTTON_Y
#define BUTTON_5   0x0010  // Bit 4  BUTTON_LB
#define BUTTON_6   0x0020  // Bit 5  BUTTON_RB
#define BUTTON_7   0x0040  // Bit 6  BUTTON_BACK
#define BUTTON_8   0x0080  // Bit 7  BUTTON_START
#define BUTTON_9   0x0100  // Bit 8  BUTTON_LS
#define BUTTON_10  0x0200  // Bit 9  BUTTON_RS
#define BUTTON_11  0x0400  // Bit 10 BUTTON_HOME
#define BUTTON_12  0x0800  // Bit 11 BUTTON_TOUCH
#define BUTTON_13  0x1000  // Bit 12 BUTTON_DPAD_UP
#define BUTTON_14  0x2000  // Bit 13 BUTTON_DPAD_DOWN
#define BUTTON_15  0x4000  // Bit 14 BUTTON_DPAD_LEFT
#define BUTTON_16  0x8000  // Bit 15 BUTTON_DPAD_RIGHT

// Gamepad Button Bitmasken (Xbox-Layout) gleiche wie // Gamepad Button Bitmasken nur mit vernünftigen namen.
#define BUTTON_A          0x0001  // Bit 0
#define BUTTON_B          0x0002  // Bit 1
#define BUTTON_X          0x0004  // Bit 2
#define BUTTON_Y          0x0008  // Bit 3

#define BUTTON_LB         0x0010  // Bit 4 - Left Bumper
#define BUTTON_RB         0x0020  // Bit 5 - Right Bumper
#define BUTTON_BACK       0x0040  // Bit 6 - Back / Select
#define BUTTON_START      0x0080  // Bit 7 - Start

#define BUTTON_LS         0x0100  // Bit 8 - Left Stick (Press)
#define BUTTON_RS         0x0200  // Bit 9 - Right Stick (Press)

#define BUTTON_HOME       0x0400  // Bit 10 - Guide / Home
#define BUTTON_TOUCH      0x0800  // Bit 11 - Touchpad / Extra (bei PS-Controllern)

#define BUTTON_DPAD_UP    0x1000  // Bit 12  windows only
#define BUTTON_DPAD_DOWN  0x2000  // Bit 13  windows only
#define BUTTON_DPAD_LEFT  0x4000  // Bit 14  windows only
#define BUTTON_DPAD_RIGHT 0x8000  // Bit 15  windows only
