# Daedalus

<div align="center">

[![Português](https://img.shields.io/badge/🇧🇷_Português-009C3B?style=for-the-badge&labelColor=009C3B)](./README-pt-br.md)
[![English](https://img.shields.io/badge/🇺🇸_English-002868?style=for-the-badge&labelColor=002868)](./README.md)

**Line-following robot with seven infrared sensors and an ESP-WROOM-32U.**

</div>

Daedalus is an autonomous robot that detects and follows a line using seven infrared sensors. Its firmware controls the motors with a PID controller and provides sensor calibration using the button and onboard LEDs.

## Contents

- [`3d/`](./3d/): STEP CAD models for the chassis, PCB support, and rear wing.
- [`pcbs/`](./pcbs/): electronics design images and Gerber files for board fabrication.
- [`docs/imgs/`](./docs/imgs/): robot photographs and 3D model views.
- [`code/`](./code/): Arduino sketch, firmware modules, and Makefile.

## 3D Models

Open the STEP files with a compatible CAD application:

- [Chassis](./3d/chassi.step)
- [PCB support](./3d/pcb_support.step)
- [Rear wing](./3d/rear_wing.step)

## PCB

The [`pcbs/`](./pcbs/) directory contains electronics design images and Gerber manufacturing files, including copper layers, board outline, and drill data.

- [PCB design](./pcbs/pcb_design.png)
- [Schematic](./pcbs/schematic.png)
- [Gerber files](./pcbs/Gerber/)

## Images

### Assembled robot

| Front view | Top view | Rear view |
| --- | --- | --- |
| ![Daedalus front view](./docs/imgs/vista_frontal.jpeg) | ![Daedalus top view](./docs/imgs/vista_superior.jpeg) | ![Daedalus rear view](./docs/imgs/vista_traseira.jpeg) |

### 3D model

| Front view | Top view | Rear view |
| --- | --- | --- |
| ![3D model front view](./docs/imgs/3d_vista_frontal.png) | ![3D model top view](./docs/imgs/3d_vista_superior.png) | ![3D model rear view](./docs/imgs/3d_vista_traseira.png) |

## Firmware

The firmware is in [`code/`](./code/) and targets an ESP32 board using Arduino. The `Makefile` uses Arduino CLI and the `esp32:esp32:esp32` board definition.

### Main files

- [`code.ino`](./code/code.ino): initialization, robot states, calibration, and line-following logic.
- [`config.h`](./code/config.h) and [`config.cpp`](./code/config.cpp): configuration and sensor, motor, LED, and PID instances.
- [`pins.h`](./code/pins.h): ESP32 pin assignments.
- [`calibration.cpp`](./code/calibration.cpp): sensor calibration and storage in the ESP32 Preferences memory.
- [`modules/`](./code/modules/): IR sensor, motor, LED, and PID implementations.
- [`Makefile`](./code/Makefile): shortcuts to compile, upload firmware, and open the serial monitor.

### Build and upload

Install Arduino CLI and ESP32 board support. From `daedalus/code/`, run:

```sh
make ports
make compile
make upload PORT=/dev/ttyUSB0
make monitor PORT=/dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with your board's serial port. To compile and upload in sequence, run `make flash PORT=/dev/ttyUSB0`. The serial monitor baud rate is 115200.

## Operation

1. On startup, the robot remains stopped and waits for the button.
2. To calibrate, press and release the button before three seconds have elapsed. Follow the LED indication: place the sensors over the black surface and press the button; then place them over the white surface and press it again. The values are stored on the ESP32 and the robot restarts.
3. To start line following, hold the button for at least three seconds and release it.
4. While following the line, press the button to stop and return to the waiting state.

The controller uses seven normalized sensor readings and PID to adjust the motors. If the line is no longer detected, the robot briefly confirms the loss and searches in the last known direction.

## Pinout

| Function | ESP32 pins |
| --- | --- |
| IR sensors, left to right | 36, 39, 34, 4, 35, 32, 33 |
| IR sensor LED | 14 |
| Motor A (right): PWM, IN1, IN2 | 25, 26, 27 |
| Motor B (left): PWM, IN1, IN2 | 13, 21, 22 |
| Button | 19 |
| Green, yellow, and red LEDs | 18, 5, 17 |

PID, speed, and line-search parameters are defined in [`code/calibration.h`](./code/calibration.h).
