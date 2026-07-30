# PINBALL-WIZARD-25-GAMEPAD
<div>
<img src="Pictures\pbwz25-WIP-12-2025.jpg" width="800">
</div>


<div>
<img src="Pictures\PBWZ25-PCB-MOD-BASIS.jpg" width="800">
</div>

📺 
[![Watch the video](https://img.youtube.com/vi/zgfT5R-a2N8/maxresdefault.jpg)](https://www.youtube.com/watch?v=zgfT5R-a2N8)

**Video:** https://www.youtube.com/watch?v=zgfT5R-a2N8

// 31.07.2026 Finaly the Meta Quest with FW 2.6 works with "analog" gamepad protocol, exactly like pinballFX classic.  
// i just defined the action button in profile 1 (standart boot up profile (profiles are in the bluetooth menu))  
// now profile 1 works for pinballFX  2026 + Pinballfx classic per profile 1  
// have phun, and play with the nudge sensitivity setting and in debug menu with the deadzones.  

// 18.2.2026 FINALY X-Arcade support on Meta Quest. No more glitching hands. 

// 31.1.2026  
// VALENTINES UPDATE. RGB ANIMATONS, NEW KEY SAMPLE METHODE WITH EMV SHIELD AGAINST RGB LED INTEREFERENCES.  
//   
// SET serial debug level allways to 0 if you dont want to debug, that increases speed to 295000 rounds per secound  
//  
// I CHANGED THE IO PINS FOR THE BUTTONS. YOU CAN USE THE OLD BUTTON PINS, BUT THEN YOU HAVE TO CHANGE IT IN THE SOURCE  
// Now i solder the FLIPPER L+R, RGB LED, FRONT RIGHT PINS to the removed TF CARD READER PINS. PICTURE WILL FOLLOW.  
// That minimises the traces to cut to only 1, and we can freely use the onboard pull up resistors for for the card   
  

!!    
!! 1.1.2026 Developement of the next great pinball controller PBWZ-26 has begun.    
!!    
!! Unfortunely the Cheap Yellow Display esp32 from China has changing PCBs. Thats annoying.    
!! Specially the Power Off function makes trouble and i dont want to use a sliding power switch.    
!! The device will work, but if you get a wrong board, you have to install a switch, instead of the button.    
!! For me its inaceptable, its not a big deal, but it shows that future pcb could do more trouble.    
!! That makes reproducing the pinball wizard not easier. So i decided to pay 5-10 bucks more,    
!! for a good seller like Waveshare with a good supply chain and good documentation.    
!! On top we become better hardware in all cases.     


!! Update 28.01.2026 the winners for the new CPU/DISPLAY CONTEST are:  
!!  
!! for the PBWZ26: Waveshare S3, 3.5" with housing (the model with cam + QSPI [quad spi])  
!! ATTENTION only this model has the faster QSPI interface with faster drivers ~30$  
!! exact model name: ESP32-S3-Touch-LCD-3.5B-C  ("B-C" at the end!!!)  
!! https://www.aliexpress.com/item/1005009056920045.html  
!!  
!! for the PBWZ27: GUITION P4+C6, 4,3" with housing, 2x 400mhz. A BEAST for ~30$  
!! https://www.aliexpress.com/item/1005009673625472.html  
!!  
!! i think some parts of the firmware needs some polish for the new P4 cpu features...  
!! the cpu has so much more power and new features, it may take a time to write a new  
!! firmware. do not expect a P4 firmware before xmas 2026.   
!! the S3 firmware is in the work, because it needs different QSPI compatible drivers.  
!! do not order the new devices, before i release something, except you like the devices.  
!! meanwhile you can use it as touch+voice remote for home assistant   
!! or as usb-desktop extender, or what ever project you can find on github.   
!! both devices has sound cards + mic onboard, and costs 5 bucks more then the   
!! fucking cheap yellow crap, but they has so much more to offer.    
!! easy reachable pin headers and a dozend more features. they are better in every case.  
!! no pcb hacking, no trace cutting, less soldering and much much easier soldering. just cables.   
!! for easy use, i recommend to buy the model with case. maybe thats part of   
!! my future design. i dont know at this point. better you get the case for cents.  
!!
!! new features on the boards:  
!! soundcard with microphone onboard, S3 better cpu, P4 much better CPU, bigger displays,  
!! faster display busses, newer Bluetooth standarts with more bandwidth and responsiveness,  
!! easy reachable usb ports with usb host function to play via usb-cable connection as well,  
!! cameras. (btw. on the p4, the cam works fluidly 30fps in the demo. so much power...)  
!! the waveshare has a better gyro/acceleration sensor type QMI8658 onboard, then the old BMI160.   
!! on the p4 we can connect the same 2 dollar gyro sensor with 4/5 pin jst.  
!! they have battery connectors and battery management chips, with different voltmeters.  
!! full power ON/OFF controll by buttons and by software. onboard stereo amplifier + speaker,  
!! the waveshare 3.5" has 480x320 px (double pixel density then the cheap yellow crap)    
!! the P4 4.3" has 480X800 (4 times pixel then the cheap yellow crap, with MIPI DSI/CSI 2 highspeed   interface, h264 video decoder,jpg onboard, separate WIFI/BLUETOOTH chip the new esp c6)   
!! much more Sram/PSram and much more i forgot.  

-----------------------------------------------------------------------------------------

Central firmware for the great Pinball Wizard 2025 with Blutooth BLE + ESP NOW. 
Based on ESP32 "CHEAP YELLOW PCB" from China/Amazon (20€-30€). 
+ 3.2 Inch Ips Display 
+ GT911 Touch Controller 
+ BMI160 Acc Gyro Sensor
+ xxxxx Battery Controller
+ Sanwa or Qanba Switches (or size comparable clones)
+ 1000 mAh rechargeable main Battery
+ Bluetooth BLE
+ Gamepad emulation
+ Keyboard emulation at same time. (Hybrid protocol)
+ Wireless foot pedal for arcade mode "X" button. (esp-now protocol)
+ Auto Power off sleep timer to save battery
+ Display light dimmer timer to save battery
+ On Display adjustable gyroscope/accelerometer for nudging
+ On Display different key map profiles for different games
+ Speed optimized code (1/300000 of a second button press reaction time)
+ Cheat mode
+ Working Devices: Meta Quest, Android Smart Phones
+ Temporarely not working connecting to PC. (it worked in the past, but i missconfigured something)
+ Ergonomic multi purpose design. On your legs, your table, on a docking station and/or on a stand.
+ Adjustable Flipper buttons positions for your needings
+ Mainframe from 50 to 60 cm easy extendable
+ Flipper-Button press logger / counter for your statistics or button life cycle logging.


Optional:
+ Optional RGB Led Qanba keys support in future. finished
+ Optional Docking Station. work in progress
+ Optional Battery Meter (Voltage divider). finished
+ Optional "X" Button next to the flipper button. finished
+ Optional 2 addon batterys 18650. work in progress

Future Ideas without any warranty:
+ Mechanicaly animated solenoid driven flippers and freeplay "knock" (depends on the game and platform)
+ Rumble  (depends on the game and platform)
+ Powerbank inside, to power the RGB button leds and the Meta Quest. work in progress
+ UART over bluetooth to configure it via APK tool on the Quest
+ Analog Plunger (depends on the game)

You can find All 3d CAD files and the BOM on Printables as well.
## **[BOM (Bill of materials)](https://github.com/VR-addicted/PINBALL-WIZARD-25-GAMEPAD/blob/main/DOCS/BOM_and_Schematic_Infos_PBWZ25.pdf)**

Check all 4 Project Repos for all optional components! 

The Project uses in this model 4 Physical Buttons, but it can emulate much more Keys
while Long Press and hold the left front key + one of the 3 other switches, we have 3 more keys. 
Like the "Shift" Key on your keyboard does.
These keys can be stored in profiles and can be mapped to any keyboard or gamepad HID output.
This will give more controll over the game`s menu system, reset view in VR and other available features.
Plus we have 4 more Virtual Buttons in combination with the gyro sensor.
In long pressed mode (aka shift mode) we can take nudges or tilting the controller to simulate 4 more keys to trigger in game functions.

Can you use the ESP32 Cheap Yellow Display Board for your own projects, without the Pinball Wizard Case?
Yes, absolutely. But order one with same Touch controller, Same Display and same Display Resolution.
Exakt type: ESP-Display,ESP Modul WiFi Bluetooth 3.2" Zoll 240 * 320 ESP Display TFT Modul ESP-2432S032C-I capacitive Touchscreen with GT911 touch Controller.
https://www.amazon.de/dp/B0D3WDQ6FJ
(Or stay tuned, i am working on the PBWZ26 and PBWZ27 as well. PBWZ26 with Waveshare Esp32 s3 3.5 Display Module and PBWZ27 with 4.3 inch display, P4+C6 CPU the beast...)


You can flash, config and debug the device with an usb cable and your browser. 
(Testet with Edge/Chrome. Check your browser for USB support enabled.)
Dont mind this wired URL. Its free webspace for developers.


## **[FLASH FIRMWARE ONLINE IN YOUR BROWSER](https://pinball-wizard-25.netlify.app/)**
https://pinball-wizard-25.netlify.app/

Don`t install nightly builds if you are not envolved or instructed. 
Nightlys are realtime snapshots with bugs and/or unfinished functions!


If you begin the project, feel free to get in contact with me on printables, meta, discord or here.  
You can ask me any question about this project. You are welcome.  
Discord: vraddicted  
Meta: VR-addicted  

HINT, small manual: If the device goes to sleep, or you powered it off via menu, press the left front button to re-start the device.
I write a manual, if some people are interested in the project.  
  
TO CONNECT TO A BLUETOOTH DEVICE PRESS THAT BLUETOOTH ICON IN THE BLUTETOOTH MENU. (Its not selfexplaining for some people. :-) )  
IF YOU HAVE PROBLEMS CONNECTING, BE SURE YOU ALREADY DELETED THE OLD CONNECTION.  
  
THERE ARE TWO BUTTONS TO CLEAN OLD CONNECTIONS. USE IT.  
IF AN OLD PAIRED DEVICE TAKES OVER THE PAIRING IN LESS THEN A SECOND, DISABLE THE OTHER DEVICE TEMPORARELY.  
SOME BLUETOOTH DEVICES ARE CACHING OLD CONNECTIONS AND THE ESP32 REMEMBERS AND CONNECTS IT IMMEDEATELY.  
IF THE QUEST LOSE BT CONNECTION, IN WHAT CASE EVER, ITS FASTER TO SIMPLE RESTART THE PBWZ25.  
AFTER RESTART, ITS PAIRED AND CONNECTED. IF YOU TRY TO REPAIR THE PBWZ25, YOU CAN DO THAT; BUT THIS 
PROCEDURE WILL RESTART THE PBWZ25 AS WELL AND COSTS MORE STEPS TO REPAIR.  
  
THIS SOUNDS HORRIBLE? NOPE. ABSOLUTELY NOT  
This happens very rarely, and possibly because I test so much, I provoke it more often than necessary.  
The system runs very stably, quickly, and reliably. I just wanted to point out this last possibility.   

  
Stay stable, and the force will be with you.   
  
More project links:  
  
https://github.com/VR-addicted/PINBALL-WIZARD-25-WEB-TOOL  
  
https://github.com/VR-addicted/PINBALL-WIZARD-25-PEDAL  
   
CAD:  
  
https://github.com/VR-addicted/PINBALL-WIZARD-25-CAD  
  
https://www.thingiverse.com/thing:7241122  
  
https://www.printables.com/model/1516410-pinball-wizard-25-bluetooth-controller-with-gyro-t  
