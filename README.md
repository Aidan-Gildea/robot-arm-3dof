# Robot Arm 3-DOF

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
