# Horus

<div align="center">

[![Português](https://img.shields.io/badge/🇧🇷_Português-009C3B?style=for-the-badge&labelColor=009C3B)](./README-pt-br.md)
[![English](https://img.shields.io/badge/🇺🇸_English-002868?style=for-the-badge&labelColor=002868)](./README.md)

**15 x 15 cm mini-sumo robot controlled by a PlayStation 4 controller over BLE.**

</div>

Horus is a remotely controlled mini-sumo robot built around an ESP-WROOM-32U. A browser-based controller reads a PlayStation 4 controller and sends commands to the robot over Bluetooth Low Energy (BLE). The competition class has maximum dimensions of 150 x 150 mm and a maximum mass of 1,000 g.

## Contents

- [`3d/`](./3d/): STEP models for the chassis, cover, PCB base, and antenna support.
- [`pcbs/`](./pcbs/): PCB design and schematic images, plus Gerber manufacturing files.
- [`docs/imgs/`](./docs/imgs/): assembled robot photographs and 3D model views.
- [`code/`](./code/): Arduino firmware, modules, and Makefile.
- [`../global/3d/`](../global/3d/): shared wheel and logo models.

## 3D Models

Open the STEP files with a compatible CAD application:

- [Chassis](./3d/chassi.step)
- [Cover](./3d/cover.step)
- [PCB base](./3d/pcb_base.step)
- [Antenna support](./3d/antenna_support.step)

Horus uses the [wheel v5 model](../global/3d/wheel_v5.step).

### 3D-printing settings

Fill in the recommended material and print settings for each component:

| Component | Filament | Walls | Infill |
| --- | --- | --- | --- |
| [Chassis](./3d/chassi.step) | PETG | 2 | 20% |
| [Cover](./3d/cover.step) | PETG/PLA | 2 | 15% |
| [PCB base](./3d/pcb_base.step) | PETG/PLA | 2 | 15% |
| [Antenna support](./3d/antenna_support.step) | PETG/PLA | 2 | 15% |
| [Motor support](../global/3d/bracket.stl) | PETG | 3 | 20% |
| [Logo](../global/3d/mythos_logo.step) | PETG/PLA | 2 | - |
| [Wheel v5](../global/3d/wheel_v5.step) | PETG | 5 | 25% |

## PCB

The electronics are assembled on a double-sided, 80 x 50 mm PCB designed in EasyEDA. The board brings together the ESP-WROOM-32U, TB6612FNG dual H-bridge, LM7805CV 5 V regulator, status LEDs, power switch, 2S battery charger, and battery and motor connectors.

- [PCB design](./pcbs/pcb_design.png)
- [Schematic](./pcbs/schematic.png)
- [Gerber files](./pcbs/Gerber/)

The robot is powered by a 2S2P 18650 battery. The regulated 5 V rail supplies the logic, while the motors are connected directly to the battery. Firmware limits normal motor output to 75%; turbo raises the limit to 100% temporarily.

## Images

### Assembled robot

| Front view | Rear view | Competition |
| --- | --- | --- |
| ![Horus front view](./docs/imgs/vista_frontal.jpeg) | ![Horus rear view](./docs/imgs/vista_traseira.jpeg) | ![Horus at the competition](./docs/imgs/horus_3lugar.jpeg) |

### 3D model

| Front view | Top view | Rear view |
| --- | --- | --- |
| ![3D model front view](./docs/imgs/vista_frontal_3d.png) | ![3D model top view](./docs/imgs/vista_superior_3d.png) | ![3D model rear view](./docs/imgs/vista_traseira_3d.png) |

## Firmware

The firmware is written in C++ for the ESP32 Arduino core. The Makefile uses Arduino CLI and the `esp32:esp32:esp32` board definition. The firmware supports the PWM APIs in Arduino-ESP32 core versions 2 and 3.

### Main files

- [`code.ino`](./code/code.ino): initializes the modules and runs the BLE, action, and LED updates.
- [`config.h`](./code/config.h) and [`config.cpp`](./code/config.cpp): declarations and shared instances for motors, LEDs, actions, and BLE.
- [`pins.h`](./code/pins.h): ESP32 GPIO assignments.
- [`calibration.h`](./code/calibration.h): motor direction, PWM, speed limits, BLE name and power, joystick dead zone, and watchdog timeout.
- [`protocol.h`](./code/protocol.h): BLE service identifiers and command packet definitions.
- [`modules/ble.h`](./code/modules/ble.h): BLE server, connection events, and packet validation.
- [`modules/actions.h`](./code/modules/actions.h): joystick/button interpretation and differential-drive mixing.
- [`modules/motors.h`](./code/modules/motors.h): TB6612FNG direction and PWM output.
- [`modules/leds.h`](./code/modules/leds.h): status LED patterns.
- [`Makefile`](./code/Makefile): compile, upload, serial monitor, and cleanup commands.

### Build and upload

Install Arduino CLI and ESP32 board support. From `horus/code/`, run:

```sh
make ports
make compile
make upload PORT=/dev/ttyUSB0
make monitor PORT=/dev/ttyUSB0
```

Replace `/dev/ttyUSB0` with the board's serial port. To compile and upload in sequence, run `make flash PORT=/dev/ttyUSB0`. The serial monitor baud rate is 115200.

## Control and communication

The ESP32 advertises as `Mythos: HORUS` and acts as a BLE server with one writable characteristic. The companion browser controller is maintained separately and is not included in this directory. When the controller disconnects, the firmware stops the motors and resumes BLE advertising.

Each command packet is 7 bytes. Packets with an incorrect length, sync byte, or checksum are rejected.

| Byte | Content |
| --- | --- |
| 0 | Sync byte (`0xA5`) |
| 1 | Left stick X axis (unused) |
| 2 | Left stick Y axis (forward/reverse) |
| 3 | Right stick X axis (turn) |
| 4 | Right stick Y axis (unused) |
| 5 | Button bit mask |
| 6 | Checksum: sum of bytes 1-5 modulo 256 |

Axis values are offset by 100 and interpreted in the range -100 to +100. A dead zone of 8 is applied around the center. The left stick's vertical axis controls forward/reverse motion; the right stick's horizontal axis controls turning.

| Control | Action |
| --- | --- |
| R1 | Turbo while held; raises the motor limit to 100% |
| L1 | Stop while held |
| R2 | Turn right at full turn input while held |
| L2 | Turn left at full turn input while held |
| A | Connection test; turns on the green LED while held |

The firmware mixes forward and turn inputs for differential drive, then scales both motor outputs proportionally if either exceeds the command range. If no valid control update arrives for more than 300 ms, the watchdog stops the motors.

## Pinout and status

| Function | ESP32 pins |
| --- | --- |
| Motor A: PWM, IN1, IN2 | 32, 25, 33 |
| Motor B: PWM, IN1, IN2 | 14, 26, 27 |
| Green, yellow, and red LEDs | 18, 5, 17 |

Motor PWM uses 8-bit resolution at 7 kHz. Motor direction can be inverted in [`code/calibration.h`](./code/calibration.h) without changing the wiring.

| Robot state | LED indication |
| --- | --- |
| Startup | Red solid |
| Waiting for connection | Yellow blinking every 400 ms |
| Connected and stopped | Green solid |
| Moving | Yellow solid |
| Turbo active | Yellow and red blinking every 100 ms |

## Competition

So far, Horus has competed only in the sumo competition held during SNCT 2026 (National Week of Science and Technology) at IFES Serra, where it placed third among 9 teams.
