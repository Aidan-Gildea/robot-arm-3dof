# Serial Communication Protocol

The robot arm receives servo angle commands via serial communication at **9600 baud** over USB.

---

## Command Format

Send **5 space-separated integers** (each 0–180) followed by a newline character:

```
base joint1 joint2 gripper1 gripper2\n
```

**Example — home position:**

```
90 150 150 90 90
```

Each integer maps to a servo channel on the PCA9685 driver:

| Position | Servo     | PCA9685 Channel | Role            |
|----------|-----------|-----------------|-----------------|
| 1        | Base      | 0               | Base rotation   |
| 2        | Joint 1   | 1               | Shoulder        |
| 3        | Joint 2   | 2               | Elbow           |
| 4        | Gripper A | 4               | Gripper finger  |
| 5        | Gripper B | 6               | Gripper finger  |

---

## How the Byte Reading Works

The serial reader in `main.cpp` processes incoming bytes one at a time to parse angle commands.

### 1. Byte-by-Byte Accumulation

```cpp
static byte currIndex = 0;   // Persists across loop() calls
char inByte = Serial.read();  // Read one byte at a time
```

Each character is stored in `inputLine[]` until a newline (`\n`) is received:

| Byte # | Character | Buffer State |
|--------|-----------|--------------|
| 1      | `'9'`     | `"9"`        |
| 2      | `'0'`     | `"90"`       |
| 3      | `' '`     | `"90 "`      |
| 4      | `'4'`     | `"90 4"`     |
| ...    | ...       | ...          |
| n      | `'\n'`    | Process!     |

### 2. Tokenization

When a newline is received, spaces are replaced with null terminators (`\0`):

```
Before: "90 45 60 30 120\0"
After:  "90\045\060\030\0120\0"
```

This creates **5 separate C-strings** in contiguous memory.

### 3. Parsing

A pointer walks through the buffer, converting each token to an integer:

```cpp
char* ptr = inputLine;
for (int i = 0; i < 5; i++) {
    angles[i] = atoi(ptr);        // Convert string to int
    ptr += strlen(ptr) + 1;       // Jump to next token
}
```

### 4. Execution

The parsed angles are passed to `ServosSetAngles()`, which:

1. **Validates** each angle is within `[0, 180]`.
2. **Maps** each angle to a PWM pulse length using the formula:
   ```
   pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX)
   ```
   where `SERVOMIN = 145` and `SERVOMAX = 560` (out of 4096 ticks at 50 Hz).
3. **Applies** per-servo calibration offsets to compensate for hardware misalignment.
4. **Writes** the final pulse to the corresponding PCA9685 channel via I²C.

---

## Buffer Limits

| Parameter          | Value                                  |
|--------------------|----------------------------------------|
| Max input length   | 30 bytes                               |
| Max valid command   | `"180 180 180 180 180\n"` = 24 chars  |
| Baud rate          | 9600                                   |
| Line terminator    | `\n` (newline)                         |

---

## Per-Servo Calibration Offsets

Each servo has an individual angular offset defined in `motors.h` to correct for mechanical misalignment:

| Servo   | Offset |
|---------|--------|
| Base    | +10°   |
| Joint 1 | −10°   |
| Joint 2 | −5°    |
| Gripper A | +10° |
| Gripper B | −10° |

These offsets are converted from degrees to pulse-length deltas and applied after the angle-to-pulse mapping. Additionally, joints 1 and 2 receive a **general offset of 60°** (converted to pulse length) during the homing routine to increase their effective range of motion.
