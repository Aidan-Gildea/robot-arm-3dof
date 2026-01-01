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
# Based on your data, Y=100 to 130 is a "sweet spot" for reach
HEIGHT = -100 
# A radius of 100mm is well within your 181mm max reach
RADIUS = 100 
# How smooth the motion is (number of points in the circle)
SAMPLES = 360 
# Time to wait between movements (adjust based on your servo speed)
STEP_DELAY = 0.025

def main():
    try:
        # Initialize Serial
        print(f"Connecting to {PORT}...")
        ser = serial.Serial(PORT, BAUD, timeout=1)
        time.sleep(2) # Wait for Arduino to reset
        
        print("Starting 'Halo' Path Sweep...")
        
        # Create a circle path using angles from 0 to 360 degrees
        angles_deg = np.linspace(0, 360, SAMPLES)
        
        for deg in angles_deg:
            # Convert degree to radians for math
            rad = np.radians(deg)
            
            # Calculate X and Z coordinates for a circle
            x = RADIUS * np.cos(rad)
            z = RADIUS * np.sin(rad)
            y = HEIGHT
            
            # Use your IK function to get the joint angles
            angles = k.IK(base_link_length, l1, l2, x, y, z, general_offset)
            
            if angles:
                # Format the message exactly as your Arduino expects: "Base J1 J2\n"
                message = f"{angles[0]} {angles[1]} {angles[2]}\n"
                print(f"Moving to Coord: [{int(x)}, {y}, {int(z)}] -> Angles: {message.strip()}")
                
                ser.write(message.encode('ascii'))
                time.sleep(STEP_DELAY)
            else:
                print(f"Skipping point [{int(x)}, {y}, {int(z)}]: Out of reach.")

        print("Path sequence complete. Returning to home...")
        
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