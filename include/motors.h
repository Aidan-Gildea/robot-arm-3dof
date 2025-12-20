#pragma once
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

// Servo min and max pulse lengths
#define SERVOMIN  145  // Minimum pulse length (out of 4096)
#define SERVOMAX  560  // Maximum pulse length (out of 4096)
#define SERVO_FREQ 50  // Servo frequency (50Hz for most analog servos)

#define SERVO_ANGLE_OFFSET 60

int NUM_SERVOS;
Adafruit_PWMServoDriver pwm;

void setupMotors(int servoAmount)
{
    pwm = Adafruit_PWMServoDriver();
    pwm.begin(); //ABSOLUTELY NECESSARY, DO NOT FORGET NEXT TIME
    pwm.setPWMFreq(SERVO_FREQ);
    NUM_SERVOS = servoAmount;

    if(servoAmount > 0)
    {
      pinMode(A0, INPUT);
      if(servoAmount > 1)
      {
        pinMode(A1, INPUT);
        if(servoAmount > 2)
        {
          pinMode(A2, INPUT);
          if(servoAmount > 3)
          {
            pinMode(A3, INPUT);
            if(servoAmount > 4)
            {
              pinMode(A4, INPUT);
              if(servoAmount > 5)
              {
                pinMode(A5, INPUT);
                if(servoAmount > 6)
                {
                  Serial.println("Too many servos, used all analog pins");
                }
              }
            }
          }
        }
      }
    }
    
    delay(100);
}

//later on I want to improve this function by taking in an angle from the rotary encoder and using it to find how to time to slow down speed. 
bool ServosSetAngles(int args[])
{

  int angle1 = args[0];
  int angle2 = args[1];
  int angle3 = args[2];

  Serial.println("Setting servos to angles: " + String(angle1) + ", " + String(angle2) + ", " + String(angle3));

  if (angle1 < 0 || angle1 > 180 || angle2 < 0 || angle2 > 180 || angle3 < 0 || angle3 > 180) {
    Serial.println("Invalid angle(s)");
    return false;
  }

  pwm.setPWM(0, 0, map(angle1, 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(1, 0, map(angle2, 0, 180, SERVOMIN, SERVOMAX));
  pwm.setPWM(2, 0, map(angle3, 0, 180, SERVOMIN, SERVOMAX));

  return true;
}

// bool ServosSetPath(String args)
// {
//   args.trim();
//   Serial.println("Received path:");
//   Serial.println(args);

//   while (args.length() > 0) {
//     int spacer1 = args.indexOf(' ');
//     if (spacer1 == -1) return false;

//     int spacer2 = args.indexOf(' ', spacer1 + 1);
//     if (spacer2 == -1) return false;

//     int spacer3 = args.indexOf(' ', spacer2 + 1);

//     String firstString;
//     if (spacer3 == -1) {
//       // Last group of 3, take the rest of the string
//       firstString = args;
//       args = "";
//     } else {
//       firstString = args.substring(0, spacer3);
//       args = args.substring(spacer3 + 1);
//     }

//     firstString.trim();
//     Serial.print("Setting angles: ");
//     Serial.println(firstString);
//     ServosSetAngles(firstString);
//     delay(250); // optional delay between movements
//   }

//   Serial.println("Path complete");
//   return true;
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
  byte SERVOOFFSET = (SERVOMIN - SERVOMAX) / 3; //60 degrees offset for homing
  Serial.println("Homing all servos to 90 degrees");
  int degrees = (SERVOMAX - SERVOMIN) / 2 + SERVOMIN;
  pwm.setPWM(0, 0, degrees);
  for(int i = 1; i < 3; i++)
  {
    pwm.setPWM(i, 0, degrees + SERVOOFFSET);
    delay(100);
  }
  
}

float PrintServoAngle(String index) ///always returns integer angle, I wonder why that is :/
{
  Serial.println("Angle is: ");
  int servoIndex = index.toInt();
  switch(servoIndex) //I know this is trash but it works whatever
  {
    case 0: return(map(analogRead(A0), 104, 662, 0, 18000)/100);
    
    case 1: return(map(analogRead(A1), 102, 664, 0, 18000)/100);
    
    case 2: return(map(analogRead(A2), 104, 662, 0, 18000)/100);
    
    case 3: return(map(analogRead(A3), 104, 662, 0, 18000)/100);

    case 4: return(map(analogRead(A4), 104, 662, 0, 18000)/100);
    
    case 5: return(map(analogRead(A5), 104, 662, 0, 18000)/100);

    default: return -1;
  }
}
