# Autonomous Dustbin-Collecting Robot

A course project for **IRE 212** at **University of Frontier Technology Bangladesh**.

The robot autonomously detects dustbins using a camera and a custom-trained YOLOv5 model, navigates toward them, picks them up with a robotic arm, and dumps them at a designated zone — no human input needed once it's running.

> Demo video: *(add link here)*

---

## What it does

- Detects dustbins in real-time using a Pi Camera + YOLOv5 (TFLite) running on Raspberry Pi 4
- Follows a lane using OpenCV
- Picks up detected objects using a HiWonder 4-DOF arm
- After 5 pickups, navigates to a dump zone automatically

We trained the model on a coffee cup dataset as a dustbin proxy — it worked surprisingly well in practice.

---

## Hardware used

- Raspberry Pi 4 (runs the ML model and main control loop)
- Arduino Uno (controls motors and the arm)
- TS400 chassis
- HiWonder 4-DOF robotic arm
- Pi Camera V2
- HC-SR04 ultrasonic sensor
- L298N motor driver

---

## Project structure

```
├── raspberry_pi/
│   ├── main.py              # Main control loop
│   └── trash_detector.py   # TFLite inference
├── arduino/
│   └── control.ino         # Motor and arm control
├── dataset/
│   └── data.yaml
└── README.md
```

---

## Training the model

We trained a YOLOv5s model on a custom coffee cup dataset (used as a dustbin proxy). After training, the model was converted to TFLite using TensorFlow's converter and deployed on the Raspberry Pi 4. Input resolution was kept at 320×320 to balance speed and accuracy on edge hardware.

---

## Team

| Name | Role |
|---|---|
| [Istiaque Ahmed](https://github.com/) | Software (ML pipeline, control loop, detection) |
| Suwaybid Ahmed Muaz | Hardware (chassis, arm, wiring) |
| Sazid Ahmed | Documentation |

**Course:** IRE 212 — University of Frontier Technology Bangladesh

---

## Acknowledgements

- [Ultralytics YOLOv5](https://github.com/ultralytics/yolov5)
- [HiWonder Robotics](https://www.hiwonder.com/)
