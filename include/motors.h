#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Servo min and max pulse lengths
#define SERVOMIN  145  // Minimum pulse length (out of 4096)
#define SERVOMAX  560  // Maximum pulse length (out of 4096)
#define SERVO_FREQ 50  // Servo frequency (50Hz for most analog servos) -> Is a constant suggested by library
#define SERVO_ANGLE_OFFSET 60 // The two main joint servos are offset by 60 degrees for more ROM
#define NUM_MOTORS 5  // Number of motors in the system

// Custom PWM channel addresses for each servo (0-15 on PCA9685)
#define SERVO1_ADDRESS 0
#define SERVO2_ADDRESS 1
#define SERVO3_ADDRESS 2
#define SERVO4_ADDRESS 4 // the last two motors control the gripper
#define SERVO5_ADDRESS 6 // the last two motors control the gripper

// Individual servo angle offsets to account for any hardware misalignment
#define SERVO1_ANGLE_OFFSET 10
#define SERVO2_ANGLE_OFFSET -10
#define SERVO3_ANGLE_OFFSET -5
#define SERVO4_ANGLE_OFFSET 10
#define SERVO5_ANGLE_OFFSET -10

extern int NUM_SERVOS;
extern Adafruit_PWMServoDriver pwm;

void setupMotors(int servoAmount);
bool ServosSetAngles(int args[]);
bool ServoSetAngle(String args);
void HomeServos();
float PrintServoAngle(String index);
