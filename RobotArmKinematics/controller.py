"""
Robot Arm Controller using Pygame
---------------------------------
Controls:
  Arrow Up/Down   : Move Y axis (vertical)
  Arrow Left/Right: Move X axis (horizontal)
  F / B           : Move Z axis (forward/backward)
  Space           : Toggle gripper (open/close)
  R               : Reset to home position
  ESC             : Quit
"""

# pyright: reportMissingModuleSource=false
# pylint: disable=no-member

import pygame
import time
import kinematics as k

try:
    from serial import Serial, SerialException  # type: ignore
except ImportError:
    print("pyserial not found. Install with: pip install pyserial")
    raise

# --- CONFIGURATION ---
PORT = 'COM6'
BAUD = 9600

# Link lengths in mm
BASE_LINK_LENGTH = 50
L1, L2 = 110, 71
GENERAL_OFFSET = 60

# Movement step size (mm per key press/hold)
STEP_SIZE = 1 

# Starting position (home)
HOME_X, HOME_Y, HOME_Z = 0, 150, 50

# Gripper angles (open and closed)
GRIPPER_OPEN_ANGLES = (90, 90)      # servo4, servo5 angles when open
GRIPPER_CLOSED_ANGLES = (0, 180)    # servo4, servo5 angles when closed

# Position limits (mm)
X_MIN, X_MAX = -150, 150
Y_MIN, Y_MAX = -100, 200
Z_MIN, Z_MAX = 0, 150


class RobotState:
    """Holds the current state of the robot arm."""
    def __init__(self):
        self.x = HOME_X
        self.y = HOME_Y
        self.z = HOME_Z
        self.gripper_closed = True
    
    def reset(self):
        """Reset to home position."""
        self.x = HOME_X
        self.y = HOME_Y
        self.z = HOME_Z
        self.gripper_closed = True


def send_command(ser, state: RobotState):
    """
    Calculate IK and send command to robot arm.
    Returns True if successful, False if position unreachable.
    """
    angles = k.IK(BASE_LINK_LENGTH, L1, L2, state.x, state.y, state.z, GENERAL_OFFSET)
    
    if angles:
        base, j1, j2 = angles
        grip1, grip2 = GRIPPER_CLOSED_ANGLES if state.gripper_closed else GRIPPER_OPEN_ANGLES
        
        message = f"{base} {j1} {j2} {grip1} {grip2}\n"
        ser.write(message.encode('ascii'))
        return True
    return False


def clamp(value, min_val, max_val):
    """Clamp value between min and max."""
    return max(min_val, min(max_val, value))


def main():
    state = RobotState()
    
    # Initialize Pygame
    pygame.init()
    screen = pygame.display.set_mode((400, 300))
    pygame.display.set_caption("Robot Arm Controller")
    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 24)
    
    # Connect to Arduino
    ser = None
    try:
        print(f"Connecting to {PORT}...")
        ser = Serial(PORT, BAUD, timeout=1)
        time.sleep(2)  # Wait for Arduino reset
        print("Connected!")
        
        # Send initial home position
        send_command(ser, state)
        
    except SerialException as e:
        print(f"Error: Could not open {PORT}. {e}")
        pygame.quit()
        return
    
    running = True
    last_send_time = 0
    send_interval = 0.05  # Minimum time between commands (50ms)
    
    print("\n--- Controls ---")
    print("Arrow Up/Down   : Y axis (vertical)")
    print("Arrow Left/Right: X axis (horizontal)")
    print("F / B           : Z axis (forward/backward)")
    print("Space           : Toggle gripper")
    print("R               : Reset to home")
    print("ESC             : Quit\n")
    
    while running:
        position_changed = False
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key == pygame.K_SPACE:
                    # Toggle gripper
                    state.gripper_closed = not state.gripper_closed
                    position_changed = True
                    print(f"Gripper: {'CLOSED' if state.gripper_closed else 'OPEN'}")
                elif event.key == pygame.K_r:
                    # Reset to home
                    state.reset()
                    position_changed = True
                    print("Reset to home position")
        
        # Handle continuous key presses for movement
        keys = pygame.key.get_pressed()
        
        # Y axis (Up/Down arrows)
        if keys[pygame.K_UP]:
            state.y = clamp(state.y + STEP_SIZE, Y_MIN, Y_MAX)
            position_changed = True
        if keys[pygame.K_DOWN]:
            state.y = clamp(state.y - STEP_SIZE, Y_MIN, Y_MAX)
            position_changed = True
            
        # X axis (Left/Right arrows)
        if keys[pygame.K_RIGHT]:
            state.x = clamp(state.x + STEP_SIZE, X_MIN, X_MAX)
            position_changed = True
        if keys[pygame.K_LEFT]:
            state.x = clamp(state.x - STEP_SIZE, X_MIN, X_MAX)
            position_changed = True
            
        # Z axis (F/B keys)
        if keys[pygame.K_f]:
            state.z = clamp(state.z + STEP_SIZE, Z_MIN, Z_MAX)
            position_changed = True
        if keys[pygame.K_b]:
            state.z = clamp(state.z - STEP_SIZE, Z_MIN, Z_MAX)
            position_changed = True
        
        # Send command if position changed (with rate limiting)
        current_time = time.time()
        if position_changed and (current_time - last_send_time) >= send_interval:
            success = send_command(ser, state)
            if not success:
                print(f"Warning: Position ({state.x}, {state.y}, {state.z}) unreachable")
            last_send_time = current_time
        
        # --- Draw UI ---
        screen.fill((30, 30, 40))
        
        # Title
        title = font.render("Robot Arm Controller", True, (255, 255, 255))
        screen.blit(title, (120, 20))
        
        # Position display
        pos_text = font.render(f"Position: X={state.x:6.1f}  Y={state.y:6.1f}  Z={state.z:6.1f}", True, (200, 200, 200))
        screen.blit(pos_text, (50, 60))
        
        # Gripper status
        grip_color = (255, 100, 100) if state.gripper_closed else (100, 255, 100)
        grip_text = font.render(f"Gripper: {'CLOSED' if state.gripper_closed else 'OPEN'}", True, grip_color)
        screen.blit(grip_text, (140, 90))
        
        # Controls help
        controls = [
            "Controls:",
            "  ↑/↓  : Y axis (up/down)",
            "  ←/→  : X axis (left/right)",
            "  F/B  : Z axis (forward/back)",
            "  Space: Toggle gripper",
            "  R    : Reset to home",
            "  ESC  : Quit"
        ]
        for i, line in enumerate(controls):
            text = font.render(line, True, (150, 150, 150))
            screen.blit(text, (50, 130 + i * 22))
        
        pygame.display.flip()
        clock.tick(60)  # 60 FPS for smooth response
    
    # Cleanup
    if ser and ser.is_open:
        ser.close()
        print("Serial connection closed.")
    pygame.quit()
    print("Controller stopped.")


if __name__ == "__main__":
    main()
