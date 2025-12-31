#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Servo min and max pulse lengths
#define SERVOMIN  145  // Minimum pulse length (out of 4096)
#define SERVOMAX  560  // Maximum pulse length (out of 4096)
#define SERVO_FREQ 50  // Servo frequency (50Hz for most analog servos)

#define SERVO_ANGLE_OFFSET 60 // note that the servos home to 150 degrees for more ROM

// should probably go in .README
// when plugging in feedback pins, make sure they are in order A0, A1, A2, etc.


int NUM_SERVOS;
Adafruit_PWMServoDriver pwm;

void setupMotors(int servoAmount)
{
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

  int angle1 = args[0];
  int angle2 = args[1];
  int angle3 = args[2];

  // optional message for debugging
  // Serial.println("Setting servos to angles: " + String(angle1) + ", " + String(angle2) + ", " + String(angle3));

  if (angle1 < 0 || angle1 > 180 || angle2 < 0 || angle2 > 180 || angle3 < 0 || angle3 > 180) {
    // optional message for debugging
    // Serial.println("Invalid angle(s)");
    return false;
  }

  pwm.setPWM(0, 0, map(angle1, 0, 180, SERVOMIN, SERVOMAX)); 
  pwm.setPWM(1, 0, map(angle2, 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(2, 0, map(angle3, 0, 180, SERVOMIN, SERVOMAX));

  return true;
}


// }

bool ServoSetAngle(String args)
{
  args.trim();
  int space = args.indexOf(' ');
  if(space == -1)
  {
    Serial.println("Incorrect Inputs for SERVO");
  }
  else
  {
    Serial.println("Moving SERVO " + args);
  }

  byte servoIndex = args.substring(0, space).toInt();
  float angle = args.substring(space + 1).toFloat();
  Serial.println();


  if(servoIndex >= NUM_SERVOS || servoIndex < 0) return false;
  if(angle > 180 || angle < 0) return false;
  int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servoIndex, 0, pulseLength);
  return true;

}

void HomeServos()
{
  int SERVOOFFSET = (SERVOMAX - SERVOMIN) / (3); //60 degrees offset for homing
  
  // optional message for debugging
  Serial.println("Homing all servos to straight position. ");
  
  int degrees = (SERVOMAX - SERVOMIN) / 2 + SERVOMIN; // 90 degrees

  pwm.setPWM(0, 0, degrees);
  for(int i = 1; i < NUM_SERVOS; i++)
  {
    pwm.setPWM(i, 0, degrees - SERVOOFFSET);
    delay(100);
  }
  
}

float PrintServoAngle(String index) ///always returns integer angle, I wonder why that is :/
{
  Serial.println("Angle is: ");
  int servoIndex = index.toInt();

  if(servoIndex >= NUM_SERVOS || servoIndex < 0) return -1;

  return (map(analogRead(A0+servoIndex), 104, 662, 0, 18000)/100.00);

}
