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
+ Optional RGB Led Qanba keys support in future. work in progress
+ Optional Docking Station. work in progress
+ Optional Battery Meter (Voltage divider). works
+ Optional "X" Button next to the flipper button. work in progress


Future Ideas without any warranty:
+ Mechanicaliy animated solenoid driven flippers and freeplay "knock" (depends on the game and platform)
+ Rumble  (depends on the game and platform)
+ Powerbank inside, to power the RGB button leds and the Meta Quest. work in progress
+ UART over bluetooth to configure it via APK tool on the Quest
+ Analog Plunger (depends on the game)

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


If you beginn the project, feel free to get in contact with me on printables, meta, discord or here.
You can ask me any question about this project. You are wellcome.
Discord: vraddicted
Meta: VR-addicted

HINT, small manual: If the device goes to sleep, or you powered it off via menu, press the left front button to re-start the device. I write a manual, if some people are interested in the project.
And press the bluetooth icon in the BT menu, to be ready to connect to a device.


Stay stable, and the force will be with you. 