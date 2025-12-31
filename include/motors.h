#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Servo min and max pulse lengths
#define SERVOMIN  145  // Minimum pulse length (out of 4096)
#define SERVOMAX  560  // Maximum pulse length (out of 4096)
#define SERVO_FREQ 50  // Servo frequency (50Hz for most analog servos)
#define SERVO_ANGLE_OFFSET 60 // note that the servos home to 150 degrees for more ROM

// Individual servo angle offsets to account for any hardware misalignment
#define SERVO1_ANGLE_OFFSET 10
#define SERVO2_ANGLE_OFFSET -10
#define SERVO3_ANGLE_OFFSET -5

extern int NUM_SERVOS;
extern Adafruit_PWMServoDriver pwm;

void setupMotors(int servoAmount);
bool ServosSetAngles(int args[]);
bool ServoSetAngle(String args);
void HomeServos();
float PrintServoAngle(String index);
