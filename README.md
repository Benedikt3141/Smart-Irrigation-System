![Preview](images/Progress/Preview.jpg)

→ Version one of this project (didn't completely work)

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
- 4 Buttons
- *Tank fill state sensor*

###### Other features and outputs

- **Battery powered**
- 2.2" TFT LCD Screen with resistive touch
- Buzzer
- SD log with data saved as .csv
- LED-Strip status indicator
- magnetic valve for watering Control
- *web interface*
- *Push Emails when water is empty/error etc.*

![Schematic](src/pcb/ProjectSchematic.jpg)

![PCB](src/pcb/Project_PCB_Layout.jpg)

![Rendering](src/pcb/PCB_PlantWatering.png)

## Repository Structure

This repository contains both the firmware and the PCB design files.

- `/firmware/` contains the PlatformIO firmware project.
- `/firmware/src/` contains the ESP32 source code.
- `/src/pcb/` contains the KiCad PCB design files.
- `/src/pcb/gerbers/` contains the exported Gerber files for manufacturing.

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
└── cart_jlcpcb.png
```

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

You can either use the provided Gerber files or modify the KiCad project and generate new manufacturing files for your preferred PCB supplier.

You can find the gerberfiles [here ](src/pcb/Smart_Irrigation_SystemV2/production). The correlating `BOM.csv` and `position.csv` can be found in the same directories.

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

Thank you, Hack Club, for supporting my previous request. I would be very grateful if you could help me with this next chapter of my project!

## AI Usage

Allof the code was written by myself. Since this is a work in progress, the code may still contain bugs or parts that could be improved.

I used AI for debugging assistance, spelling correction, research and for generating the concept visualization shown at the top of this README.

The PCB design, hardware architecture and most of the firmware were
created completely by myself.

