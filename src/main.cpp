#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <communication.h>

void setup() {
  //beginning serial on Baud 9600
  Serial.begin(9600);

  //motors setupfor 1 servo
  setupMotors(3);
  delay(500);
  // HelpCommand();
  pinMode(A0, INPUT);
  HomeServos();
}

//testing testing

const byte MAX_LEN = 15;
char inputLine[MAX_LEN];
int angles[3];

void loop() 
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

      Serial.print("Angles: ");
      for (int i = 0; i < 3; i++) {
          Serial.print(angles[i]);
          if (i < 2) Serial.print(", ");
      }
      ServosSetAngles(angles);
      for(int i = 0; i < 3; i++)
      {
        angles[i] = 0; // Reset angles after processing
      }
      currIndex = 0; // Reset index for next input
    }
  }

  

}
