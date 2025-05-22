# 🧠 Autonomous Micromouse Maze-Solving Robot

This project demonstrates the design and implementation of an **autonomous micromouse robot** that:
- Navigates a maze using the **right-wall following algorithm**
- Crosses a **causeway** and climbs a **spiral slope** using edge-following logic
- Operates fully autonomously and completes the task in **3 minutes 11 second**

---

## 🏗️ Project Scope

The project was completed as part of the SER300 Mechatronic Design unit at Deakin University. It involved the complete end-to-end development of an embedded mechatronic system including:

- Mechanical design and 3D printed chassis  
- Electrical system including custom-designed PCB  
- Firmware development on STM32 microcontroller using C and HAL drivers

---

## 📦 Features

- Right-Wall Following Algorithm for maze navigation  
- Edge-Following Algorithm for elevated tracks (causeway and spiral)  
- Encoder-based turning logic with calibrated motor ticks  
- Compact robot (70mm x 70mm x 45mm) designed to maximize maneuverability  
- IR-based obstacle and edge detection  
- Runs on STM32L432KC Nucleo board using HAL drivers in STM32CubeIDE  
- LiPo battery powered with voltage buck regulator and overcurrent protection via SMD fuse  
- Fully autonomous behavior switching based on environmental sensor input  

---

## 🧰 Technologies and Tools Used

| Area              | Tools & Components                                  |
|-------------------|-----------------------------------------------------|
| Microcontroller   | STM32L432KC (ARM Cortex-M4)                         |
| Programming       | C (STM32 HAL in STM32CubeIDE)                      |
| PCB Design        | Altium Designer                                     |
| CAD               | Autodesk Inventor                                   |
| Power Supply      | 7.4V 1000mAh LiPo Battery                            |
| Sensors           | 4x IR Proximity Sensors (38 kHz Pololu)            |
| Motors            | N20 Geared DC Motors with Encoders (1:50)          |
| Motor Control     | TB6612FNG Dual H-Bridge Motor Driver                |
| Voltage Regulator | Mini 360 Buck Converter                             |

---

## 🧠 Skills Gained

- Embedded C programming with STM32 HAL  
- Encoder interfacing and interrupt handling  
- PID logic and PWM motor control  
- PCB schematic and layout design using Altium  
- CAD modeling and rapid prototyping via 3D printing  
- Problem-solving mechanical & sensor integration issues  
- Debugging and optimization of autonomous behaviors

---

## 🚀 Project Accomplishments

- Designed and 3D printed a minimal chassis optimized for balance and center of mass  
- Developed a custom single-layer PCB to host motor drivers, regulators, IR sensors, and connectors  
- Achieved precise 90° turns via encoder interrupts and tested thresholds  
- Implemented adaptive behavior switching between maze and causeway segments  
- Resolved real-world issues such as motor imbalance and sensor mounting errors through practical design iterations
- The robot successfully completed all the required tasks in **3 minutes and 11 seconds**, staying well within the allotted time constraint.

