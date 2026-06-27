# Smart Irrigation System

The Smart Irrigation System is a battery-powered monitoring and control system for watering up to six individual plants.
Each plant is monitored by a capacitive soil moisture sensor connected to the main control unit.

The project currently focuses on the main monitoring and control unit. A separate watering unit is planned but not fully designed yet. Communication between both units is intended to use ESP-NOW.



## Project Overview

The system consists of two main parts:



### Main Monitoring and Control Unit

The main unit collects sensor data, displays current measurements on a TFT screen, logs data to an SD card and indicates the system status using an LED strip.

Each soil moisture sensor is connected individually using an audio jack connector.



### Watering Unit

The watering unit is planned as a separate module. It will receive commands from the main control unit via ESP-NOW and control the water flow using a solenoid valve.
The unit should also monitor the water tank level using an ultrasonic sensor.

This part of the system is still work in progress.



## Feature overview



###### Sensors (Inputs)

* \[x] **Soil mositure monitoring** for individual plants
* \[x] **Temperature**
* \[x] Humidity
* \[x] Air pressure
* \[x] Air Quality
* \[x] CO2 concentration
* \[x] Solar intensity
* \[x] 4 Buttons
* \[ ] Tank fill state sensor



###### Other features and outputs

* \[x] **Battery powered**
* \[x] 2.4" TFT LCD Screen with resistive touch
* \[x] SD log with data saved as .csv
* \[x] LED-Strip status indicator
* \[ ] magnetic valve for watering Control
* \[ ] web interface
* \[ ] Push Emails when water is empty/error etc.


![Schematic](https://github.com/Benedikt3141/Smart-Irrigation-System/tree/main/PCB\_Files/Schematic.jpeg)

![PCB](https://github.com/Benedikt3141/Smart-Irrigation-System/tree/main/PCB\_Files/PCB_Layout.jpeg)


## Getting started



#### Requirements

To work with this project, you need:



* VS Code
* PlatformIO or pioarduino IDE Extension
* C++ Compiler
* KiCad 10.0.1 or later



#### Installation



Use git to clone this repository into your computer.

```
git clone https://github.com/Benedikt3141/Smart-Irrigation-System.git

```

Open the project folder in VS Code and build/upload the firmware using PlatformIO or pioarduino.



###### Hardware

###### 

You can either use the provided Gerber files or modify the KiCad project and generate new manufacturing files for your preferred PCB supplier.



### 

## License



[MIT](https://choosealicense.com/licenses/mit/)



## Current Project Status



This project is still in development.



The main monitoring unit is mostly implemented. The watering unit, web interface and notification system are planned but not finished yet.



## Sources

[MQ2 Library](https://github.com/labay11/MQ-2-sensor-library/)
[SD Card](https://RandomNerdTutorials.com/esp32-web-server-microsd-card/)



## AI Usage

I only used AI for debugging, spelling correction and research.
Most of the code was written by myself. Since this is a work in progress, the code may still contain bugs or parts that could be improved.

