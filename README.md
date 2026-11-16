![Preview](images/Progress/Back_Render.png)
→ Current Version as .step Assembly ([Assembly](src/assembly_and_models/SmartIrrigationSystem.FCStd))

![Title](images/Progress/ascii-art-text.png)

# About

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

![Concept](Concept.jpg)

## Feature overview

###### Sensors (Inputs)

- **Soil mositure monitoring** for individual plants
- **Temperature**
- Humidity
- Air pressure
- Air Quality
- CO2 concentration
- Solar intensity
- 3 Buttons
- *Tank fill state sensor*

###### Other features and outputs

- **Battery powered**
- 3.2" TFT LCD Screen with resistive touch
- Buzzer
- SD log with data saved as .csv
- LED-Strip status indicator
- Lighting for the Buttons
- magnetic valve for watering Control
- *web interface*
- *Push Emails when water is empty/error etc.*

![Schematic](src/pcb/ProjectSchematic.jpg)

![PCB](src/pcb/Project_PCB_Layout.jpg)

![Rendering](src/pcb/PCB_PlantWatering.png)

![RenderingBack](images/Progress/Top_Render.png)

![Preview](images/Progress/Preview.jpg)
   → Version one of this project (didn't completely work)

## Repository Structure

This repository contains both the firmware and the PCB design files.

- `/firmware/` contains the PlatformIO firmware project.
- `/firmware/src/` contains the ESP32 source code.
- `/src/pcb/` contains the KiCad PCB design files.
- `/src/pcb/Smart_Irrigation_SystemV2/production/` contains the exported Gerber files for manufacturing.
- `/src/pcb/assembly_and_models` contains 3d assembly files

## File Structure

```
Smart-Irrigation-System/
├── firmware/
├── images/
├── src/│  
│   ├── assembly_and_models/
│   │   ├── SmartIrrigationSystem-Assembly.step
│   │   └── SmartIrrigationSystem.FCStd
│   └── pcb/
│       └── Smart_Irrigation_SytemV2/
├── .gitignore
├── BOM.csv
├── Concept.jpg
├── JOURNAL.md
├── LICENSE
├── README.md
├── cart_adafruit.png
├── cart_amazon.png
└── cart_jlcpcb.png
```

## Getting started

#### Requirements

To work with this project, you need:

* VS Code
* PlatformIO or pioarduino IDE Extension
* C++ Compiler
* KiCad 10.0.1 or later
* You also need a 3D printer with Filament you want to use for printing the housing and other regular hardware tools like a soldering iron 

#### Installation

Use git to clone this repository into your computer.

```
git clone https://github.com/Benedikt3141/Smart-Irrigation-System.git
```

Open the project folder in VS Code and build/upload the firmware using PlatformIO or pioarduino IDE.

###### Hardware

You can either use the provided Gerber files or modify the KiCad project and generate new manufacturing files for your preferred PCB supplier.

You can find the gerberfiles for JLCPCB [here ](src/pcb/Smart_Irrigation_SystemV2/production). The correlating `BOM.csv` and `position.csv` can be found in the same directories.

## License

[MIT](https://choosealicense.com/licenses/mit/)

## Current Project Status

This project is still in development.

The main monitoring unit is mostly implemented. The watering unit, web interface and notification system are planned but not finished yet.

## Sources

[MQ2 Library](https://github.com/labay11/MQ-2-sensor-library/)
[SD Card](https://RandomNerdTutorials.com/esp32-web-server-microsd-card/)

## Why I Think I Should Receive Financial Sponsorship for This Project

I already received $180 in rewards from Hack Club in July 2026. Since then, I have spent nearly 100 hours working on this project. During this time, I have gained many new skills and a lot of knowledge in PCB design, including effective routing techniques, reading datasheets, organizing schematics, and working with Git. I have also developed personal skills, especially resilience and perseverance throughout this long process.

I started this project seven months ago, and I'm still determined to bring it to completion. Even though I've had to pause my work several times because I'm still in school, have exams coming up in two years, and have many other hobbies and responsibilities that take up my free time, I've always found a way to keep making progress.

I've carefully reviewed and checked my design multiple times, and I'm now quite confident that the hardware design is ready to be finalized.

I'm really proud of what I've accomplished and everything I've learned along the way. This project has been a challenging but incredibly rewarding experience, and I'm excited to take the next step toward completing it.

Thank you, Hack Club, for supporting my previous request. I would be very grateful if you could help me with this next chapter of my project! I need 500$ (450€) to finance this the new pcb (SmartIrrigationSystemV2.5) and the other neccessary parts mentioned in the BOM like Displays, fans, buttons, LEDs etc.. The rest of the money I'll need for this project iteration I'll pay from my own money.

## AI Usage

Allof the code was written by myself. Since this is a work in progress, the code may still contain bugs or parts that could be improved.

I used AI for debugging assistance, spelling correction, research and for generating the concept visualization shown at the top of this README.

The PCB design, hardware architecture and most of the firmware were
created completely by myself.

## Bill of Materials (BOM)

| Item                                                            | Qty | Unit price | Total price | Purchase link                                                  | Notes                                                      |
| ----------------------------------------------------------------- | ----: | -----------: | ------------: | ---------------------------------------------------------------- | ------------------------------------------------------------ |
| PCB_Board                                                       |   1 |       15€ |        15€ |[Product](cart_jlcpcb.png)                                                             | —                                                         |
| PCB_Assembly                                                    |   1 |      186€ |       186€ | [Product](cart_jlcpcb.png)                                                            | —                                                         |
| PCB Shipping and Taxes                                          |   1 |      100€ |       100€ | [Product](cart_jlcpcb.png)                                                             | Last time I ordered, I had to pay 60€ tax before delivery |
| Nocuta NF-A6x15 5V PWM                                          |   1 |       17€ |        17€ | [Product](https://www.amazon.de/dp/B0DJP3987K?tag=noctua09-21) | Fan                                                        |
| 3.2 TFT Display with Resistive Touchscreen                      |   2 |       15€ |        30€ | [Product](https://www.adafruit.com/product/4278)               | Touchscreen                                                |
| Kailh Mechanical Key Switches - Tactile Brown - 10-pack WS2812B |   1 |        7€ |         7€ | [Product](https://www.adafruit.com/product/4954)               | Switches                                                   |
| NeoPixel Nano 2020 RGB LEDs -10-Pack -WS2812B                   |   1 |        4€ |         4€ | [Product](https://www.adafruit.com/product/4684)               | LEDs for under the switches                                |
| NeoPixel Stick -8 x 5050 RGBW LEDs -WarmWhite ~3000K            |   2 |        8€ |        16€ | [Product](https://www.adafruit.com/product/2867)               | LEDs for under the board                                   |
| Translucent Keycaps for MX Compatible Switches - 10 pack        |   1 |        5€ |         5€ | [Product](https://www.adafruit.com/product/4956)               | Keycaps                                                    |
| Lithium Ion Polymer Battery - 3.7v 2500mAh                      |   1 |       15€ |        15€ | [Product](https://www.adafruit.com/product/328)                | LiPo                                                       |
| SoilMoisture Sensors 6pcs                      |   1 |       9€ |        9€ | [Product](https://www.amazon.de/Aideepen-Bodenfeuchtigkeitssensor-Hygrometer-Modul-korrosionsbest%C3%A4ndiges-Arduin0-Pflanzenbew%C3%A4sserung/dp/B08GCRZVSR/ref=sr_1_4?s=industrial)                | Moisture sensors I already bought                                                       |
| Shipping                                                        |   1 |       50€ |        50€ | —                                                             | Shipping from Adafruit and Amazon                          |

**Total: 445 €**

