# Hubert — 3-DOF Robotic Manipulator

**Inverse Kinematics–driven motion planning with Python & Arduino, built for ~$250.**

*Designed, programmed, and assembled as a self-directed learning project.*

---

## Demo Videos

| Path Tracing Demo | Hardware Overview | Project Presentation |
|---|---|---|
| [![Path Tracing](https://img.youtube.com/vi/4sgCwkX76dg/hqdefault.jpg)](https://youtu.be/4sgCwkX76dg?si=9VvJAM_YSKhd0c1-) | [![Hardware Overview](https://img.youtube.com/vi/wcLp37mRvIc/hqdefault.jpg)](https://youtu.be/wcLp37mRvIc?si=lRgipF_aaHsbhIL2) | [![Project Presentation](https://img.youtube.com/vi/JEB1u3Z_pXA/hqdefault.jpg)](https://youtu.be/JEB1u3Z_pXA?si=sM1r-wsa0qPkrcNg) |
| Hubert traces a scripted 3D semi-circle path. | Walk-through of the mechanical and electrical design. | Full project presentation and live demo. |

---

## Project Overview

Hubert is a **3-Degree-of-Freedom (3-DOF) robotic manipulator** capable of precise end-effector positioning from Cartesian coordinate inputs. The arm features three revolute joints (base rotation, shoulder, and elbow) plus a two-finger servo gripper, all driven by a custom **Inverse Kinematics solver that I derived from first principles** using algebra and geometry.

I started this project as a 10th-grade student with almost no prior experience in robotics. I chose to build a manipulator arm because the future of robotics is trending toward humanoid platforms, and an articulated arm is a natural entry point for developing core competencies in mechanical design, embedded systems, and motion planning.

### Barriers Overcome

- **Cost.** The entire project was completed for approximately **$250** in parts and tooling (excluding a 3D printer), making it accessible without institutional funding.
- **Self-taught curriculum.** With no structured coursework available, I taught myself 3D CAD modeling (Fusion 360), electronics (wiring, PWM, I²C), and programming (C++, Python, serial communication) through YouTube tutorials and online documentation—learning as I built.

---

## Bill of Materials

| Category        | Component                                                                                                                   | Function                        |
|-----------------|-----------------------------------------------------------------------------------------------------------------------------|---------------------------------|
| Microcontroller | [Arduino Uno R3](https://www.amazon.com/ELEGOO-Board-ATmega328P-ATMEGA16U2-Compliant/dp/B01EWOE0UU)                        | Low-level control & serial I/O  |
| Actuators       | [5× MG996R Servos](https://www.amazon.com/4-Pack-MG996R-Torque-Digital-Helicopter/dp/B07MFK266B) (4-pack + 1)              | High-torque joint & gripper actuation |
| Servo Driver    | [PCA9685 16-Channel PWM Driver](https://www.amazon.com/HUAREW-PCA9685-Interface-Compatible-Raspberry/dp/B0CRV3MK14)        | Dedicated PWM generation via I²C |
| Power Supply    | [Adjustable DC Module](https://www.amazon.com/Adjustable-Switching-Regulated-Adjustments-Jesverty/dp/B09YSJQWRG)           | Isolated 5–6 V servo power rail |
| Fasteners       | [M2.5/M3 Phillips Screws](https://www.amazon.com/NINDEJIN-Countersunk-Replacement-Electronic-Accessories/dp/B0B5CY5LY5)   | Structural assembly             |
| Fasteners       | [M2.5/M3 Threaded Heat-Set Inserts](https://www.amazon.com/Ktehloy-Threaded-Assortment-Printing-Components/dp/B0CLKDPN65)  | Durable screw receptacles in 3D prints |
| Supports        | Standard ball bearings                                                                                                      | Low-friction joint rotation     |

> **Note:** Purchase links are provided for convenience and may expire over time. The full 3D-printable assembly is available at `Hardware/CAD/Full Assembly.step`. A detailed parts list is also in `Hardware/BOM.csv`.

---

## System Architecture

```
┌──────────────────────┐       USB / Serial        ┌──────────────────────┐
│   Host Computer      │      (9600 baud)          │   Arduino Uno        │
│                      │ ─────────────────────────▶ │                      │
│  Python              │   "base j1 j2 g1 g2\n"   │  main.cpp            │
│  ├─ kinematics.py    │                           │  ├─ Serial parser    │
│  ├─ controller.py    │                           │  └─ motors.cpp       │
│  └─ fun_path.py      │                           │      ├─ PCA9685 I²C  │
│                      │                           │      └─ Servo PWM    │
└──────────────────────┘                           └──────────┬───────────┘
                                                              │ I²C
                                                   ┌──────────▼───────────┐
                                                   │  PCA9685 PWM Driver  │
                                                   │  (16 channels)       │
                                                   └──┬──┬──┬──┬──┬──────┘
                                                      │  │  │  │  │
                                                   Servo1─5 (MG996R)
```

The host computer runs the IK solver and sends joint angles over serial. The Arduino parses incoming commands and forwards pulse-width values to the PCA9685, which generates 50 Hz PWM signals for each servo.

---

## Inverse Kinematics

The IK solver lives in `RobotArmKinematics/kinematics.py` and converts a Cartesian target $(x, y, z)$ into three joint angles. This is the core piece of math that makes the arm useful—without it, you would need to manually compute angles for every desired position.

### Arm Geometry

| Parameter          | Value   |
|--------------------|---------|
| Base link height   | 50 mm   |
| Link 1 (shoulder → elbow) | 110 mm  |
| Link 2 (elbow → end-effector) | 71 mm   |
| Max theoretical reach | 181 mm  |
| General servo offset | 60°    |

### Algorithm

The solver performs the following steps. All intermediate calculations use radians; outputs are converted to integer degrees.

1. **Subtract base height.** The target $y$ coordinate is reduced by the base link height (50 mm) to shift the origin to the shoulder joint.

2. **Project onto the XZ plane.** Compute the horizontal radius:

$$r = \sqrt{x^2 + z^2}$$

3. **Enforce physical constraints.** Reject the target if $z < 0$ (below the ground plane).

4. **Base rotation angle.** Using the Law of Cosines on the projected circle, compute the base angle relative to the positive X-axis. The angle is then normalized to $[0°, 180°]$ to match servo limits.

5. **Joint 1 (shoulder).** Compute the planar distance from the shoulder to the target:

$$a_3 = \sqrt{x_{\text{proj}}^2 + y_{\text{proj}}^2}$$

Then solve using:

$$\theta_1 = \text{atan2}(y_{\text{proj}},\; x_{\text{proj}}) + \arccos\!\left(\frac{L_1^2 + a_3^2 - L_2^2}{2 \cdot L_1 \cdot a_3}\right) + \text{offset}$$

6. **Joint 2 (elbow).** Apply the Law of Cosines to the triangle formed by $L_1$, $L_2$, and $a_3$:

$$\theta_2 = \arccos\!\left(\frac{L_1^2 + L_2^2 - a_3^2}{2 \cdot L_1 \cdot L_2}\right) - 90° + \text{offset}$$

7. **Clamp and validate.** All three angles are checked against $[0°, 180°]$. If any fall outside this range, the target is deemed unreachable and the function returns `False`.

### Workspace Analysis

The script `RobotArmKinematics/workspace_test.py` performs a brute-force reachability scan over a 3D grid (default step size: 10 mm) and exports all valid $(x, y, z) \rightarrow (\theta_{\text{base}}, \theta_1, \theta_2)$ mappings to `workspace_results.json`. This is useful for visualizing the arm's true working envelope and identifying dead zones.

---

## Software

Hubert's software is split into two layers, mirroring the physical separation between the host computer and the onboard microcontroller.

### Host-Side — Python

The Python programs run on the host computer and handle kinematics, user input, and serial transmission.

| File                | Purpose                                                                                           |
|---------------------|---------------------------------------------------------------------------------------------------|
| `kinematics.py`     | Core IK solver — `IK()` function and `distance_formula()` helper.                                |
| `controller.py`     | **Real-time Pygame controller.** Arrow keys for XY, F/B for Z, Space to toggle gripper, R to home. Includes a rate-limited command loop (50 ms) and a visual GUI overlay. |
| `fun_path.py`       | Scripted path demonstration — traces a semi-circle at two heights with smooth transitions. Used in the demo video. |
| `communication.py`  | Minimal serial test — sends a homing command to verify connectivity.                              |
| `workspace_test.py` | Brute-force 3D reachability scan, exports results to JSON.                                        |

**Controller key bindings:**

| Key          | Action                |
|--------------|-----------------------|
| ↑ / ↓        | Move Y (vertical)     |
| ← / →        | Move X (horizontal)   |
| F / B        | Move Z (depth)        |
| Space        | Toggle gripper        |
| R            | Reset to home         |
| Esc          | Quit                  |

### Firmware — C++ / PlatformIO

The firmware runs on the Arduino Uno and is managed with [PlatformIO](https://platformio.org/).

| File               | Purpose                                                                                         |
|--------------------|-------------------------------------------------------------------------------------------------|
| `main.cpp`         | Serial read loop — accumulates bytes, tokenizes on spaces, parses 5 integers, dispatches to motor driver. |
| `motors.cpp`       | PCA9685 initialization, angle → pulse mapping with per-servo calibration offsets, homing routine, analog feedback reading. |
| `motors.h`         | Constants (pulse range 145–560, 50 Hz, channel addresses, per-servo offsets) and function declarations. |
| `communication.cpp`| Legacy text-command parser (SERVO, HOME, HELP, ANGLE). Currently disabled — preserved for future re-integration. |

**Homing behavior:** On startup, all servos are driven to 90° (mid-range). Joints 1 and 2 additionally receive a 60° general offset to position the arm upright, maximizing the usable range of motion.

---

## Getting Started

### Prerequisites

- **Hardware:** Assemble per the BOM above. Wire the PCA9685 to the Arduino via I²C (SDA → A4, SCL → A5). Power servos from the adjustable DC supply (set to 5–6 V), **not** from the Arduino's 5 V rail.
- **Firmware toolchain:** [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
- **Python 3.8+** with `pip`.

### Flash the Firmware

```bash
# From the repository root
pio run --target upload
```

### Install Python Dependencies

```bash
pip install numpy pyserial pygame
```

### Run the Controller

```bash
cd RobotArmKinematics
python controller.py
```

> **Important:** Set the `PORT` variable at the top of the Python script to match your Arduino's COM port (e.g., `COM6` on Windows, `/dev/ttyACM0` on Linux).

---

## Serial Protocol

Commands are sent as **5 space-separated integers** (each 0–180) followed by a newline:

```
90 150 150 50 130\n
```

The five values correspond to: base rotation, joint 1, joint 2, gripper finger A, gripper finger B. The Arduino's 30-byte input buffer supports commands up to 24 characters.

For a detailed walkthrough of the byte-level parsing, tokenization, and execution pipeline, see [PROTOCOL.md](PROTOCOL.md).

---

## Repository Structure

```
robot-arm-3dof/
├── platformio.ini              # PlatformIO build configuration
├── README.md
├── PROTOCOL.md                 # Detailed serial protocol documentation
├── Hardware/
│   ├── BOM.csv                 # Bill of materials with purchase links
│   └── CAD/
│       └── Full Assembly.step  # Complete 3D CAD assembly
├── include/
│   └── motors.h                # Servo constants, channel map, declarations
├── src/
│   ├── main.cpp                # Serial reader and command dispatcher
│   ├── motors.cpp              # PCA9685 driver, homing, offset calibration
│   └── communication.cpp       # Legacy command parser (disabled)
├── RobotArmKinematics/
│   ├── kinematics.py           # Inverse kinematics solver
│   ├── controller.py           # Pygame real-time controller
│   ├── fun_path.py             # Scripted path demonstration
│   ├── communication.py        # Minimal serial connectivity test
│   ├── workspace_test.py       # 3D reachability analysis
│   ├── workspace_results.json  # Exported reachability data
│   └── requirements.txt        # Python dependencies
├── lib/                        # PlatformIO library folder
└── test/                       # PlatformIO test folder
```

---

## Future Work

- **Closed-loop control.** The analog feedback pins (A0–A5) are already wired; implementing PID-based position verification would improve accuracy under load.
- **Computer vision integration.** Object detection via a camera feed to enable autonomous pick-and-place tasks.
- **Re-enable command parser.** Restore the text-based command interface in `communication.cpp` for interactive debugging alongside the IK pipeline.
