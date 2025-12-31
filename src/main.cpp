#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
// #include <communication.cpp> 
#include "motors.h"

void setup() {

  //beginning serial on Baud 9600
  Serial.begin(9600);

  //motors setup for 3 servo
  setupMotors(3);

  delay(500);
  
  // home servos on startup - not needed every time
  //HomeServos();
}


const byte MAX_LEN = 15; // max length in bytes of an input line
char inputLine[MAX_LEN]; // char array to hold the current input line
int angles[3]; // array to hold parsed angles

void loop() // serial reader
{
  static byte currIndex = 0;
  while(Serial.available() > 0)
  {
    char inByte = Serial.read();
    if(inByte != '\n' && currIndex < MAX_LEN - 1) 
    {
      inputLine[currIndex++] = inByte;
    } 
    else
    {
      inputLine[currIndex] = '\0'; // Null-terminate the string
      for(int i = 0; i < currIndex; i++)
      {
        if(inputLine[i] == ' ')
        {
          inputLine[i] = '\0'; // Replace space with null terminator
        }
      }


      char* ptr = inputLine;
      for (int i = 0; i < 3; i++) {
          angles[i] = atoi(ptr);
          ptr += strlen(ptr) + 1;  // jump past current token and null terminator
      }

      // !!!! do not want to have the IK and this robot both printing onto serial at the same time
      // Serial.print("Angles: ");
      // for (int i = 0; i < 3; i++) {
      //     Serial.print(angles[i]);
      //     if (i < 2) Serial.print(", ");
      // }


      ServosSetAngles(angles);

      for(int i = 0; i < 3; i++)
      {
        angles[i] = 0; // Reset angles after processing
      }

      currIndex = 0; // Reset index for next input
    }
  }

  

}
