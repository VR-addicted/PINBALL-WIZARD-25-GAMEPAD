# PINBALL-WIZARD-25-GAMEPAD
<div>
<img src="Pictures\pbwz25-WIP-12-2025.jpg" width="800">
</div>


Central firmware for the great Pinball Wizard 2025 with Blutooth BLE + ESP NOW. 
Based on ESP32 "CHEAP YELLOW PCB" from China/Amazon (20€-30€). 
+ 3.2 Inch Ips Display 
+ GT911 Touch Controller 
+ BMI160 Acc Gyro Sensor
+ xxxxx Battery Controller
+ Sanwa or Qanba Switches (or size comparable clones)
+ 1000 mAh Rechargeable Battery
+ Bluetooth BLE
+ Gamepad emulation
+ Keyboard at same time. (Hybrid protocol)
+ 1/300000 of a second button press reaction time 
+ Wireless foot pedal for arcade mode "X" button. works.
+ Auto Power off sleep timer to save battery
+ Display light dimmer timer to save battery
+ Adjustable gyroscope for nudging
+ Different key map profiles for different games
+ Speed optimized code
+ Cheat mode
+ Working Devices: Meta Quest, Android Smart Phones
+ Temporarely not working connecting to PC. (it worked in the past, but i missconfigured something)

+ Optional RGB Led Qanba keys support in future.
+ Optional Docking Station in progress
+ Optional Battery Meter (Voltage divider)
+ Optional "X" Button next to the flipper button.


Future Ideas without any warranty:
+ Animated solenoid driven flippers and freeplay "knock" (depends on the game and platform)
+ Rumble  (depends on the game and platform)
+ Powerbank inside to power the RGB leds and the Meta Quest
+ UART over bluetooth to configure it via APK tool on the Quest


You can find All 3d CAD files and the BOM on Printables.

Check all 4 Project Repos for all optional components! 

The Project uses in this model 4 Physical Buttons, but it can emulate much more Keys
while Long Press and hold the left front key + one of the 3 other switches, we have 3 more keys. 
Like the "Shift" Key on your keyboard does.
These keys can be stored in profiles and can be mapped to any keyboard or gamepad HID output.
This will give more controll over the game`s menu system, reset view in VR and other available features.
Plus we have 4 more Virtual Buttons in combination with the gyro sensor.
In long pressed mode (aka shift mode) we can take nudges or tilting the controller to simulate 4 more keys to trigger in game functions.


You can flash, config and debug the device with an usb cable and your browser. 
(Testet with Edge/Chrome. Check your browser for USB support enabled.)
Dont mind this wired URL. Its free webspace for developers.
https://pinball-wizard-25.netlify.app/
Don`t install nightly builds if you are not envolved or instructed. 
Nightlys are realtime snapshots with bugs and/or unfinished functions!

Stay stable, and the force will be with you. 
