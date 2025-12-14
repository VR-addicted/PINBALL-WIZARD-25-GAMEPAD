#ifndef GUI_H
#define GUI_H

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0 
#endif

#define BUILD_VERSION BUILD_NUMBER  













#include <TFT_eSPI.h>                         
#include <GT911.h>                            
#include "Fonts/custom/7Segment7_S520pt7b.h"  
#include "Fonts/custom/7Segment7_S510pt7b.h"  




#include <GamepadKeyboardHID.h>

#include <cstdio>    

extern TFT_eSprite spr;


class GUI {
  
  public:
    
    
    GUI(TFT_eSPI& display, GamepadHID* gp, KeyboardHID* kb);  

    bool UIclearScreen = true  ;
    #define bgColor 0x2104
    void clearBluetoothPairings();
    void clearActiveBluetoothPairing();
    void startFreshPairing();
   
    String getDeviceName(const String& macAddress);

    

    
    void setGamepad(GamepadHID* gp);
    void setKeyboard(KeyboardHID* kb);

    void topMenu();                               
    bool buttonBack(bool mode = 0);               
    bool saveButton(bool mode = 0);
    bool espnowButton(int8_t mode = 0);           
    bool cheatButton(int8_t mode = 0);            
    bool drawSkillShotButton(int8_t mode = 0);
    void begin();                                 
    void intro();                                 
    void menu0();                                 
    void menu1();                                 
    void menu2();                                 
    void menu3();                                 
    void menu4();                                 
    void menu5();                                 
    void menu6();                                 

    void drawButtonTilt(int value);

    int8_t drawBTprofile(int8_t mode);
    void drawVirtualTiltingJoystickKeys(int8_t direction, int8_t mode);
    void drawTiltDirection(int8_t direction, int8_t mode);
    void drawPhysicalVirtualKeys(int8_t key, int8_t mode);

        
    void drawWaterBubble();

    void drawText(int x, int y, const char* text);
    void drawRectangle(int x, int y, int w, int h, uint32_t color);
    void UIupdate(int loopsPerSecond, int loopTimeMs);        
    void drawDebug(int loopsPerSecond, int loopTimeMs);       
    
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

    void drawPeakMeterNudgeDirection(int8_t direction = 0, int inputPower = 0); 
    
    void setTouch(GT911* touch);
    bool processTouch(int* x, int* y);

    void fillSpriteBackground();
    
    void optimizeForQuest();
    bool isQuestDevice(const String& macAddress);

private:

    
    
    
    GamepadHID* _gamepad = nullptr;
    KeyboardHID* _keyboard = nullptr;
    
    bool _touchDetected = false;
    int _lastTouchX = 0;
    int _lastTouchY = 0;


    #define SCREEN_WIDTH 240        
    #define SCREEN_HEIGHT 320       

    TFT_eSPI& _tft;                 
    void initDisplay();
    
    
    const char* texts[6] = {
      "PLAY",
      "BLUETOOT",
      "SETTINGS",
      "CHEAT",
      "DEBUG",
      "OFF"
      
    };
    int numEntries = sizeof(texts) / sizeof(texts[0]);
    int Ycells[10];                  
    
    
    
    bool flagUItopButtonsDraw = true;
    bool flagUImenu0DrawRowCells = true;
    bool flagUImenu1SpriteDown = true;
    bool flagUImenu1drawTiltButton = false;
    bool flagUImenu1drawPlungerLeft = false;
    bool flagUImenu2drawBTsymbol = true;
    bool flagUImenu6drawMenuOnce = true;
    bool flagUImenuDebugBackground     = true;

    
    u_int32_t timeMarkUIbuttonFlipperLeft   = 0;
    u_int32_t timeMarkUIbuttonFlipperRight  = 0;
    u_int32_t timeMarkUIbuttonPlungerLeft   = 0;
    u_int32_t timeMarkUIbuttonPlungerRight  = 0;
    u_int32_t timeMarkUIdrawDebug = 0;

    
    GT911* _touch = nullptr;
};


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