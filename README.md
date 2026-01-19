# 3-DOF Robotic Manipulator

**Inverse Kinematics–Driven Motion Planning with Python & Arduino**

---

## Project Overview

This project is a **3-Degree of Freedom (DOF) robotic arm** capable of precise end-effector positioning using Cartesian coordinate inputs. A custom **Inverse Kinematics (IK) solver written in Python and self-derived using algebra and geometry** converts target positions $(x, y, z)$ into joint angles, which are transmitted to an **Arduino Uno** for real-time execution.

Here is a short **demo video** of :
1. Robot Arm Path Tracing Demonstration: https://youtu.be/4sgCwkX76dg?si=9VvJAM_YSKhd0c1-
2. Brief Hardware Overview: https://youtu.be/wcLp37mRvIc?si=lRgipF_aaHsbhIL2
(These videos are updated as of 1/1/26)

---


## Bill of Materials

| Item            | Component                          | Function                              |
|-----------------|------------------------------------|---------------------------------------|
| Microcontroller | Arduino Uno R3                     | Low-level control and command parsing |
| Actuators       | 4× MG996R Servos                   | High-torque joint actuation           |
| Servo Driver    | PCA9685 (16-Channel)               | Dedicated PWM generation              |
| Power Supply    | Adjustable DC Module               | Isolated 5–6V servo power rail        |
| Fasteners       | M2.5 / M3 screws, heat-set inserts | Structural assembly                   |

---

## Kinematics & Motion Planning

All motion logic resides in `kinematics.py`. The arm geometry is defined by the following parameters:

- **Base Link Height:** 50 mm  
- **Link 1 (Humerus):** 110 mm  
- **Link 2 (Forearm):** 71 mm  

### Inverse Kinematics Logic

The solver enforces physical feasibility by clamping joint limits and applying servo homing offsets.

#### Key Computational Steps

1. **Coordinate Projection**  
   The target $(x, y, z)$ is projected onto the horizontal plane to decouple base rotation.

2. **Base Rotation Angle**  
   Computed from the positive X-axis using planar geometry.

3. **Joint Angle Resolution**  
   The Law of Cosines is applied to the triangle formed by the two arm links and the projected target distance.

---

## Software Implementation

### Python Communication Layer (`communication.py`)

This script acts as the system interface:

- Computes IK angles from Cartesian targets  
- Formats commands as space-separated integers  
- Transmits data via serial  

```python
# Example command flow
angles = k.IK(base_link_length, l1, l2, x, y, z, general_offset)
message = f"{angles[0]} {angles[1]} {angles[2]}\n"
ser.write(message.encode("ascii"))
```# Robot Arm 3-DOF

## Serial Communication Protocol

The robot arm receives servo angle commands via serial communication at **9600 baud**.

### Command Format

Send 5 space-separated angles (0-180) followed by a newline character:

```
90 90 90 90 90\n
```

### How the Byte Reading Works

The serial reader in `main.cpp` processes incoming bytes one at a time to parse angle commands.

#### 1. Byte-by-Byte Accumulation

```cpp
static byte currIndex = 0;  // Persists across loop() calls
char inByte = Serial.read(); // Read one byte at a time
```

Each character is stored in `inputLine[]` until a newline (`\n`) is received:

| Byte # | Character | Buffer State |
|--------|-----------|--------------|
| 1 | `'9'` | `"9"` |
| 2 | `'0'` | `"90"` |
| 3 | `' '` | `"90 "` |
| 4 | `'4'` | `"90 4"` |
| ... | ... | ... |
| n | `'\n'` | Process! |

#### 2. Tokenization

When newline is received, spaces are replaced with null terminators (`\0`):

```
Before: "90 45 60 30 120\0"
After:  "90\045\060\030\0120\0"
```

This creates 5 separate C-strings in contiguous memory.

#### 3. Parsing

A pointer walks through the buffer, converting each token to an integer:

```cpp
char* ptr = inputLine;
for (int i = 0; i < 5; i++) {
    angles[i] = atoi(ptr);        // Convert string to int
    ptr += strlen(ptr) + 1;       // Jump to next token
}
```

#### 4. Execution

The parsed angles are sent to `ServosSetAngles()`, which maps each angle (0-180°) to PWM pulse lengths and sends them to the PCA9685 driver.

### Buffer Limits

- **Max input length:** 30 bytes
- **Max command:** `"180 180 180 180 180\n"` = 24 characters
