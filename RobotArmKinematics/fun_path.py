import time
import serial
import numpy as np
import kinematics as k

# --- CONFIGURATION ---
PORT = 'COM6'
BAUD = 9600
base_link_length = 50
l1, l2 = 110, 71
general_offset = 60

# --- PATH PARAMETERS ---
HEIGHT = 120 
LOW_HEIGHT = -100
RADIUS = 100 
SAMPLES = 100 # Points per segment
STEP_DELAY = 0.025

def send_pos(ser, x, y, z):
    """
    Calculates IK and sends angles to the robot.
    Clamps Z to 0 if negative.
    """
    # Constraint: Z cannot be negative
    if z < 0:
        # print(f"Warning: Z={z:.2f} is negative. Clamping to 0.")
        z = 0
        
    angles = k.IK(base_link_length, l1, l2, x, y, z, general_offset)
    
    if angles:
        # Format: "Base J1 J2\n"
        message = f"{angles[0]} {angles[1]} {angles[2]}\n"
        # print(f"Moving to: [{x:.1f}, {y:.1f}, {z:.1f}] -> {message.strip()}")
        ser.write(message.encode('ascii'))
        time.sleep(STEP_DELAY)
    else:
        print(f"Skipping unreachable point: [{x:.1f}, {y:.1f}, {z:.1f}]")

def main():
    ser = None
    try:
        print(f"Connecting to {PORT}...")
        ser = serial.Serial(PORT, BAUD, timeout=1)
        time.sleep(2) # Wait for Arduino reset
        
        print("Starting Complex Path Sequence...")
        
        # --- 1. Circular Motion (Top) ---
        # Sweep from 0 to 180 degrees (Right to Left)
        # Note: We limit to 0-180 because Z cannot be negative.
        print("1. Top Semi-Circle (Y={})".format(HEIGHT))
        for rad in np.linspace(0, np.pi, SAMPLES):
            x = RADIUS * np.cos(rad)
            z = RADIUS * np.sin(rad)
            send_pos(ser, x, HEIGHT, z)
            
        # --- 2. Lower Y ---
        # Smoothly lower Y to -100 at the current position (Left side)
        print(f"2. Lowering Y to {LOW_HEIGHT}")
        # Current position is approx: x=-RADIUS, z=0
        for y in np.linspace(HEIGHT, LOW_HEIGHT, SAMPLES // 2):
            send_pos(ser, -RADIUS, y, 0)
            
        # --- 3. Circular Motion (Bottom) ---
        # Sweep back from 180 to 0 degrees (Left to Right) at low height
        # This brings us "directly below the original starting point"
        print(f"3. Bottom Semi-Circle (Y={LOW_HEIGHT})")
        for rad in np.linspace(np.pi, 0, SAMPLES):
            x = RADIUS * np.cos(rad)
            z = RADIUS * np.sin(rad)
            send_pos(ser, x, LOW_HEIGHT, z)
            
        # --- 4. Raise Y ---
        # Smoothly raise Y back to HEIGHT at the starting X,Z
        print(f"4. Raising Y to {HEIGHT}")
        # Current position is approx: x=RADIUS, z=0
        for y in np.linspace(LOW_HEIGHT, HEIGHT, SAMPLES // 2):
            send_pos(ser, RADIUS, y, 0)
            
        # --- 5. Return to Home ---
        print("5. Returning to Home Position (0, 181, 0)...")
        
        # Current position from Step 4 is (RADIUS, HEIGHT, 0) -> (100, 120, 0)
        start_x, start_y, start_z = RADIUS, HEIGHT, 0
        end_x, end_y, end_z = 0, 181, 0
        
        # Interpolate smoothly
        for t in np.linspace(0, 1, SAMPLES // 2):
            x = start_x + (end_x - start_x) * t
            y = start_y + (end_y - start_y) * t
            z = start_z + (end_z - start_z) * t
            send_pos(ser, x, y, z)

        print("Path sequence complete.")
        
    except serial.SerialException:
        print(f"Error: Could not open {PORT}. Is the Arduino connected or Serial Monitor open?")
    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial connection closed.")

if __name__ == "__main__":
    main()