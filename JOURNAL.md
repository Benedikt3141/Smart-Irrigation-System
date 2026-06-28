# Journal



#### April 2026



Looked for posible solutions for my Problem of a smart Irrigation System and looked for existing repos, but Nothing really fitted my Needs so I decided to build my own. I did Research on Hardware and learned how capaciative soil moisture sensors work.



Time consumption: \~5h



#### End of April 2026



Ordered first soil moisture sensors and tried to monitor soil moisture with an ESP32 Dev Kit C. I had some issues with using Wifi simulatnuous with the Analog read capabilities of the ESP. I had to figure out which six Pins were available with the ESP and Wifi. I coded the first Basic working construct and designed a Pretty Looking web inteface. For that I had to revise my html skills (aren't really existing I think html is boring). I also needed to decide wich file Format I had to choose for the database. I Chose .csv because ist much more lightweight than SQL or json databases. Moreover I had worked with .csv files before so it was much more easy to get in and modify them using c++ on the ESP. I couldn't find a good existing library/code so I had to code it completely from Scratch.



Time Consumption: \~6h



#### Beginning of May 2026



I decided to use a Display as Addition to the web interface and ordered a 2.4" TFT LCD Screens with resistive touch but the Display i ordered was especially designed for an Arduino r3 or any of ist mods. I had to decide weather I Need to order a new one, use the ESP and connect it with wires to the ESP and loosing many of ist gpios or to connect the Arduino with the ESP using any Kind of protocol. I did some Research on different Kind of busses, the capabilities of the buildin chip of an Arduino uno and came to the conclusion that no bus is really capable of transphereing decent amount of data reliable and fast with the Arduino because Arduino processors are much too weak for processing diagrams and so the diagrams would Need to process on the esp before transphere. That means I came up with the solution of directly wiring the Display too the ESP and live with the lack of free GPIOs.



Trial and Error, Reasearch and concideration took About 8h



#### 

#### Mid of May 2026



I first had to precisely state my Goals for this Project because I had more and more Ideas what to add to the Project like a battery, an USBC Connection, many different sensors, a small solar Panel and some more good or stupid ideas. For that I had to draw a diagram of the Concept and note down wich GPIO is used for which device. This took longer than I expected becaus some Pins aren't  really capable of doing what wanted to use them for. moreover ther was still a lack of analog Input Pins so i found a I2C analog Input extender and ordered it for my breadboard testing.

Besides that I wanted to have the Board not only as Breadboard but as solid PCB so I did some Research on what PCB desgning programs exist and Chose KiCAD as my prefered PCB designig program. Therefor I had to get used to the principles of PCB design and some crazy traits of KiCad and the General workflow of designing a PCB. This took also way longer than I had first expected so Time went on until end of may 2026 when I started to design the PCB for this Project. On the complete PCB design I started to realize that the proportions I thought of didnt fit reality and I decided to do some seperate PCBs stacked on top of each other. I splited the Project and first designed the PCBs as 4Layer PCBs because it was way easier than 2Layer PCBs and I hadn't much to handle with parts orientation. Future me has changed this to 2Layer PCB later because it is way cheaper in manufacturing. 



In Addition I decided to go with JLCPCB for PCB manufacturing and researched parts. This was quite complex for me because I hadnt much to do with the specific Features of some components before because I just used some breadboard parts and thruehole resistors, capacitors diodes and singlewire sensors. But I came up with the first design of the PCB at end of may. As I realized it was much to expensive (around 250€) for both boards I focused more on the code than on the Hardware.



This whole process of getting used to PCB design Software, first attempts, Fails and finally some success took so Long. I didn't stop the time but it must have taken much more than 20h. 



