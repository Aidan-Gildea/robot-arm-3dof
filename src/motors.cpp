#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "motors.h"


int NUM_SERVOS;
Adafruit_PWMServoDriver pwm;
const int GENERAL_PULSE_OFFSET = map(SERVO_ANGLE_OFFSET,0,180,0,SERVOMAX-SERVOMIN);
const int SERVO1_PULSE_OFFSET = map(SERVO1_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO2_PULSE_OFFSET = map(SERVO2_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int SERVO3_PULSE_OFFSET = map(SERVO3_ANGLE_OFFSET,-90,90,-(SERVOMAX-SERVOMIN)/2, (SERVOMAX-SERVOMIN)/2);
const int DEGREE_90_PULSE = SERVOMIN + (SERVOMAX - SERVOMIN) / 2; // 90 degrees pulse length


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
  for(int i = 0; i < 3; i++) {
    if (args[i] < 0 || args[i] > 180) {
    
      return false;
    }
  }

  // 2. Now map them to pulse lengths safely
  int pulse1 = map(args[0], 0, 180, SERVOMIN, SERVOMAX);
  int pulse2 = map(args[1], 0, 180, SERVOMIN, SERVOMAX);
  int pulse3 = map(args[2], 0, 180, SERVOMIN, SERVOMAX);

  // 3. Send to the PCA9685
  pwm.setPWM(0, 0, pulse1 + SERVO1_PULSE_OFFSET); 
  pwm.setPWM(1, 0, pulse2 + SERVO2_PULSE_OFFSET);
  pwm.setPWM(2, 0, pulse3 + SERVO3_PULSE_OFFSET);

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
  
  pwm.setPWM(0, 0, DEGREE_90_PULSE + SERVO1_PULSE_OFFSET); 
  pwm.setPWM(1, 0, DEGREE_90_PULSE + SERVO2_PULSE_OFFSET + GENERAL_PULSE_OFFSET);
  pwm.setPWM(2, 0, DEGREE_90_PULSE +  SERVO3_PULSE_OFFSET + GENERAL_PULSE_OFFSET);
  
}

float PrintServoAngle(String index) ///always returns integer angle, I wonder why that is :/
{
  Serial.println("Angle is: ");
  int servoIndex = index.toInt();

  if(servoIndex >= NUM_SERVOS || servoIndex < 0) return -1;

  return (map(analogRead(A0+servoIndex), 104, 662, 0, 18000)/100.00);

}
