#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "motors.h"


int NUM_SERVOS;
Adafruit_PWMServoDriver pwm;
// maps offsets from degrees to pulse lengths

const int GENERAL_PULSE_OFFSET = map(SERVO_ANGLE_OFFSET,0,180,0,SERVOMAX-SERVOMIN);

const int SERVO1_PULSE_OFFSET = map(SERVO1_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO2_PULSE_OFFSET = map(SERVO2_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO3_PULSE_OFFSET = map(SERVO3_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO4_PULSE_OFFSET = map(SERVO4_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO5_PULSE_OFFSET = map(SERVO5_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);

const int DEGREE_90_PULSE = SERVOMIN + (SERVOMAX - SERVOMIN) / 2; // 90 degrees pulse length

// Arrays for easy iteration

const int SERVO_ADDRESSES[NUM_MOTORS] = {SERVO1_ADDRESS, SERVO2_ADDRESS, SERVO3_ADDRESS, SERVO4_ADDRESS, SERVO5_ADDRESS};
const int SERVO_PULSE_OFFSETS[NUM_MOTORS] = {SERVO1_PULSE_OFFSET, SERVO2_PULSE_OFFSET, SERVO3_PULSE_OFFSET, SERVO4_PULSE_OFFSET, SERVO5_PULSE_OFFSET};


void setupMotors(int servoAmount)
{
  NUM_SERVOS = servoAmount;
  pwm = Adafruit_PWMServoDriver();
  pwm.begin(); // very important to call this function!!
  pwm.setPWMFreq(SERVO_FREQ);

  // setup for feedback pins
  for (int i = 0; i < servoAmount && i < 6; i++) {
      pinMode(A0 + i, INPUT); // A0, A1, etc. are sequential constants
  }
}

//later on I want to improve this function by taking in an angle from the rotary encoder and using it to find how to time to slow down speed. 
bool ServosSetAngles(int args[])
{
  // Check inputs are within bounds
  for(int i = 0; i < NUM_MOTORS; i++) {
    if (args[i] < 0 || args[i] > 180) {
      return false;
    }
  }

  // Map angles to pulse lengths and send to PCA9685 using custom addresses
  for(int i = 0; i < NUM_MOTORS; i++) {
    int pulse = map(args[i], 0, 180, SERVOMIN, SERVOMAX);
    // in this case pulse represents the angle converted to pulse length

    pwm.setPWM(SERVO_ADDRESSES[i], 0, pulse + SERVO_PULSE_OFFSETS[i]);
  }

  return true;
}


// }

// bool ServoSetAngle(String args)
// {
//   args.trim();
//   int space = args.indexOf(' ');
//   if(space == -1)
//   {
//     Serial.println("Incorrect Inputs for SERVO");
//   }
//   else
//   {
//     Serial.println("Moving SERVO " + args);
//   }

//   byte servoIndex = args.substring(0, space).toInt();
//   float angle = args.substring(space + 1).toFloat();
//   Serial.println();


//   if(servoIndex >= NUM_SERVOS || servoIndex < 0) return false;
//   if(angle > 180 || angle < 0) return false;
//   int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
//   pwm.setPWM(servoIndex, 0, pulseLength);
//   return true;

// }

void HomeServos()
{
  // optional message for debugging
  //Serial.println("Homing all servos to straight position. ");
  
  int degrees = (SERVOMAX - SERVOMIN) / 2 + SERVOMIN; // 90 degrees for all

  Serial.println(degrees);
  
  // Home first servo without general offset (base rotation)
  pwm.setPWM(SERVO_ADDRESSES[0], 0, DEGREE_90_PULSE + SERVO_PULSE_OFFSETS[0]); 
  
  // Home remaining servos with general offset
  for(int i = 1; i < 3; i++) {
    pwm.setPWM(SERVO_ADDRESSES[i], 0, DEGREE_90_PULSE + SERVO_PULSE_OFFSETS[i] + GENERAL_PULSE_OFFSET);
  }
  for(int i = 3; i < NUM_MOTORS; i++) {
    pwm.setPWM(SERVO_ADDRESSES[i], 0, DEGREE_90_PULSE + SERVO_PULSE_OFFSETS[i]); // since only the 1 and 2 servos need general pulse offset
  }
}

float PrintServoAngle(String index) ///always returns integer angle, I wonder why that is :/
{
  Serial.println("Angle is: ");
  int servoIndex = index.toInt();

  if(servoIndex >= NUM_SERVOS || servoIndex < 0) return -1;

  return (map(analogRead(A0+servoIndex), 104, 662, 0, 18000)/100.00);

}
