#ifndef GUI_H
#define GUI_H

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0 // "0000"  // Falls das Skript nicht läuft, Standardwert setzen  // Fallback, falls PlatformIO es nicht setzt
#endif

#define BUILD_VERSION BUILD_NUMBER  // Versionsnummer setzen


// ========================================================================== //
// TODO
// FIXME
// BUG
// HACK  
// file SSH:
// strg + d ---> erzeugt kommentarzeilen 




#include <TFT_eSPI.h>                         // Bibliothek für den Touchscreen
#include <GT911.h>                            // Touch-Bibliothek
#include "Fonts/custom/7Segment7_S520pt7b.h"  // SIMPLE 7-Segment-Font 20px. 8 numeric signs in one 240 px row
#include "Fonts/custom/7Segment7_S510pt7b.h"  // SIMPLE 7-Segment-Font 10px. 16+ numeric signs in one 240 px row

// #include <BleCompositeHID.h>


#include <GamepadKeyboardHID.h>

#include <cstdio>    // für sscanf

extern TFT_eSprite spr;


class GUI {
  
  public:
    
    //GUI(TFT_eSPI& display, BleCompositeHID* hid = nullptr);
    GUI(TFT_eSPI& display, GamepadHID* gp, KeyboardHID* kb);  

    bool UIclearScreen = true  ;
    #define bgColor 0x2104
    void clearBluetoothPairings();
    void clearActiveBluetoothPairing();
    void startFreshPairing();
   
    String getDeviceName(const String& macAddress);

    // void setGamepad(GamepadHID* gp);

    // GamepadDevice* _gamepad;
    void setGamepad(GamepadHID* gp);
    void setKeyboard(KeyboardHID* kb);

    void topMenu();                               // Blauer Balken oben
    bool buttonBack(bool mode = 0);               // zurück button oben links
    bool saveButton(bool mode = 0);
    bool espnowButton(int8_t mode = 0);           // esp status button / maybe esp now koppel menu wenn man drauf drückt.
    bool cheatButton(int8_t mode = 0);            // cheat button oben rechts
    bool drawSkillShotButton(int8_t mode = 0);
    void begin();                                 // Startet die Klasse
    void intro();                                 // Start-Animation
    void menu0();                                 // Hauptmenü mit 7 Balken
    void menu1();                                 // "Play" – Flipperball mit Button-Anzeige und Klick-Counter  Flag, setzt interes draw once, once
    void menu2();                                 // Bluetooth Einstellungen
    void menu3();                                 // Settings
    void menu4();                                 // Cheat
    void menu5();                                 // Info
    void menu6();                                 // Debug

    void drawButtonTilt(int value);

    int8_t drawBTprofile(int8_t mode);
    void drawVirtualTiltingJoystickKeys(int8_t direction, int8_t mode);
    void drawTiltDirection(int8_t direction, int8_t mode);
    void drawPhysicalVirtualKeys(int8_t key, int8_t mode);

        
    void drawWaterBubble();// mode 0 = kreis (wasserblase), mode 1 = pfeil links, mode 2 = pfeil rechts, mode 3 = pfeil oben, mode 4 = pfeil unten

    void drawText(int x, int y, const char* text);
    void drawRectangle(int x, int y, int w, int h, uint32_t color);
    void UIupdate(int loopsPerSecond, int loopTimeMs);        // in jedem loop ein durchlauf, aber nur alle 30ms gehts in die routine rein.
    void drawDebug(int loopsPerSecond, int loopTimeMs);       // stelle benchmark zahlen dar
    
    void draw7SegmentMillisecondBig(uint32_t output);
    void draw7SegmentNumberSmall(uint32_t number, int segmentPosX, int segmentPosY, bool updateUI);
    void draw7SegmentNumberBig();
    
    int8_t drawButtonCheatPlusMinus(int8_t mode = 0);
    void drawTextCheatProfileName(int8_t profileNumber);
    void drawCheatProfileSelector();

bool drawButtonCheatLockRecord(int8_t mode = 0);
bool buttonCheatBackToPlay(int8_t mode = 0);
bool drawButtonCheatStoreProfile(int8_t mode = 0);



    void drawBatteryLocal(uint8_t levelPercent);
    void drawBatteryPedal(uint8_t levelPercent);

    void drawPeakMeterNudgeDirection(int8_t direction = 0, int inputPower = 0); // 0 or none = screenrefresh, counters down, 1 = left, 2 = right, 3 = up, 4 = down
    // Neue Touch-bezogene Methoden:
    void setTouch(GT911* touch);
    bool processTouch(int* x, int* y);

    void fillSpriteBackground();
    
    void optimizeForQuest();
    bool isQuestDevice(const String& macAddress);

private:

    //BleCompositeHID* _hid;
    //  XboxGamepadDevice* _gamepad;   // statt GamepadDevice*
    //GamepadDevice* _gamepad; 
    GamepadHID* _gamepad = nullptr;
    KeyboardHID* _keyboard = nullptr;
    // Neue Touch-Zustandsvariablen nun in globalen variablen
    bool _touchDetected = false;
    int _lastTouchX = 0;
    int _lastTouchY = 0;


    #define SCREEN_WIDTH 240        // Korrekte Breite des Touchscreens
    #define SCREEN_HEIGHT 320       // Korrekte Höhe des Touchscreens

    TFT_eSPI& _tft;                 // Referenz zum TFT-Display
    void initDisplay();
    
    // Array mit 6 Textfeldern
    const char* texts[6] = {
      "PLAY",
      "BLUETOOT",
      "SETTINGS",
      "CHEAT",
      "DEBUG",
      "OFF"
      
    };
    int numEntries = sizeof(texts) / sizeof(texts[0]);
    int Ycells[10];                  // Array für die gespeicherten Y-Koordinaten
    
    // Interne Flags
    
    bool flagUItopButtonsDraw = true;
    bool flagUImenu0DrawRowCells = true;
    bool flagUImenu1SpriteDown = true;
    bool flagUImenu1drawTiltButton = false;
    bool flagUImenu1drawPlungerLeft = false;
    bool flagUImenu2drawBTsymbol = true;
    bool flagUImenu6drawMenuOnce = true;
    bool flagUImenuDebugBackground     = true;

    // time marks
    u_int32_t timeMarkUIbuttonFlipperLeft   = 0;
    u_int32_t timeMarkUIbuttonFlipperRight  = 0;
    u_int32_t timeMarkUIbuttonPlungerLeft   = 0;
    u_int32_t timeMarkUIbuttonPlungerRight  = 0;
    u_int32_t timeMarkUIdrawDebug = 0;

    // Touch-Instanz, die über den Setter initialisiert wird
    GT911* _touch = nullptr;
};

// Zweite Klasse
class MenuItem {
  private:
      
      int minValue, maxValue, currentValue;
      bool modMode;
      
      struct ButtonCoords {
          int xStart, xEnd, yStart, yEnd;
      };
      
      ButtonCoords btnMinus, btnPlus;
  
  public:
      MenuItem(const char* text, int min, int max, int initial, bool mod = false);
      
      char label[15];
      void setButtonY(int yStart);
      void draw(TFT_eSPI& tft);
      int checkTouch(int touchX, int touchY);
      int updateValue(int delta);
      int getValue() const;
      const char* getLabel() const;
  };



#endif