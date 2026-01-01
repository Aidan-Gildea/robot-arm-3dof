import sys
import numpy as np


print(sys.executable)


#Parameter descriptions:
#l1 = length of first arm segment
#l2 = length of second arm segment
#x,y,z = target coordinates
#general_offset = offset to be added to all angles to account for servo homing offsets
#j1_offset, j2_offset, j3_offset = individual joint offsets



def IK(base_link_length, l1, l2, x, y, z, general_offset): # 3DOF inverse kinematics 
    try:
        # base link length subtracted from y coordinate to account for height offset of base. The point of reference is at the point of rotation of joint 1, which is 50 mm below joint 2. 
        position = np.array([x,y - base_link_length,z]) # create a np matrix of the three different values as position
        
        
        # restrict working space of xz plane for easier calculations
        if(z < 0): return False


        # calculate the radius of the base circle
        r = float(np.sqrt(position[0] ** 2 + position[2] ** 2)) 

        # find the projected position on the xy plane in quadrant 1. Note how the point will always end up in either quadrant 1 or 4
        projected_pos = np.array([r, y, 0])

        # use distance formula to find the distance between the two points projected from the circle
        d = distance_formula(position[0], position[2], projected_pos[0], projected_pos[2])

        # law of sines to calculate the base angle
        
        # if the distance is greater than twice the radius, return false, as the base cannot reach that far
        if d > 2 * r:
            return False
        
        # if the radius is 0, then the base angle is 0
        base_angle = 0
        if r != 0:
            base_angle = np.arccos(1-(d**2 / 2 / r**2))


        # note that this is degrees from the positive x axis, which is 180 degrees on my robot (due to hardware restrictions). 

        # do this to normalize the angle to my robot
        base_angle = np.pi - base_angle
        
        # if the base has to turn more than 180 degrees or less than 0, return false, very similar to the first restriction placed
        if base_angle > np.pi or base_angle < 0:
            return False
        
        #---------------- switching back to the xy plane



        # ----------------- Note that all calculations are done in radians
        # projected_pos[1] = position[1]

        # all based on diagram in notes
        a3 = np.sqrt((projected_pos[0]**2 + projected_pos[1]**2))

        y3 = np.arctan2(projected_pos[1], projected_pos[0]) # arctan of y / x will get the little angle inbetween. 
        y2 = np.arccos((l1**2 + a3**2 - l2**2) / (2*l1*a3))
        joint1_angle = y3 + y2 + np.radians(general_offset)
        
        # restrict the first joint angle to be between 0 and 180 degrees
        if joint1_angle > np.pi or joint1_angle < 0:
            return False

        #----------------- now we can calculate the second angle
        C = np.arccos((l1**2 + l2**2 - a3**2) / (2*l1*l2))
        joint2_angle = C - np.pi/2 + np.radians(general_offset)
        
        # restrict the second joint angle to be between 0 and 180 degrees
        if joint2_angle > np.pi or joint2_angle < 0:
            return False
        
        # the values to return are theta1 and theta2
        # i can do this because python is dynamically typed
        joint1_angle = np.degrees(joint1_angle)
        joint2_angle = np.degrees(joint2_angle)
        base_angle = np.degrees(base_angle)

        return int(base_angle), int(joint1_angle), int(joint2_angle) #base, then joint 0 then joint 1 :)
    except (ValueError, ZeroDivisionError):
        return False


def distance_formula(x1, y1, x2, y2):
     return float(np.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2))


