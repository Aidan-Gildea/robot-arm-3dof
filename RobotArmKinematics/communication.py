import time
import serial
import kinematics as k


homing_message = "90 150 150 90 90\n"  # all motors homed, the only thing is that 
closed_gripper_message = "90 150 150 0 180\n"  # Format the message as base,joint1,joint2

# link lengths in mm
base_link_length = 50
l1, l2 = 110, 71

general_offset = 60 # offset to be added to all angles to account for servo homing offsets, must also be set in firmware side

# when choosing the com port, must make sure to choose the one the arduino is connected to to
# on windows machine: device manager -> ports (COM & LPT) -> look for the arduino uno port

ser = serial.Serial('COM6', 9600, timeout=1)
time.sleep(2)

# note that you need to offset the second and third angle by 60 if you are sending data manually. 

ser.write(closed_gripper_message.encode('ascii'))

# location = [90,90,90]  # x, y, z coordinates of the target point in mm


# angles = k.IK(base_link_length, l1, l2, location[0], location[1], location[2], general_offset)  # get the angles from the IK function
# if angles == False:
#     print("Target point is out of reach.")
# else:
#     message = f"{angles[0]} {angles[1]} {angles[2]}\n"  # Format the message as base,joint1,joint2
#     ser.write(message.encode('ascii'))
