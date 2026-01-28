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
!! I think a good Candidate is a Waveshare ESP32 S3 the 3.5" version with integrated gyro/acc.    
!! Some Displays are on the way and we will see what display wins.    
!! There are some candidates in the 25$ to 40$ range.    
!! The new display will have bigger screen, better display resolution, extensa7 insteead of intensa6 mcu,     
!! newer & faster Bluetooth and USB-HOST as well, for extra extra low latency.    
!! Working power button, inbuild volt meter for the battery, better gyro/acc sensor,     
!! and some other goodies like stereo speaker connector,     
!! and a 16 pin header in 2.54 size where we easy connect the flipper buttons and what ever es needed.    


!! Update 28.01.2026 the winners for the new CPU/DISPLAY CONTEST are:  
!!  
!! for the PBWZ26: Waveshare S3, 3.5" with housing (the model with cam + QSPI !)  
!! ATTENTION only this model has the faster QSPI interface with faster drivers ~30$  
!! exact model name: ESP32-S3-Touch-LCD-3.5B-C  ("B-C" at the end!!!)  
!! https://www.aliexpress.com/item/1005009056920045.html  
!!  
!! for the PBWZ27: SpotPear Electronics P4+C6, 4,3" with housing, 2x 400mhz. A BEAST for ~30$  
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

https://pinball-wizard-25.netlify.app/
## **[FLASH FIRMWARE ONLINE IN YOUR BROWSER](https://pinball-wizard-25.netlify.app/)**

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
