#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <communication.h>

void setup() {
  //beginning serial on Baud 9600
  Serial.begin(9600);

  //motors setupfor 1 servo
  setupMotors(3);
  delay(500);
  HelpCommand();
  pinMode(A0, INPUT);
  HomeServos();
}


void loop() 
{
  //Serial.print("Hello");
  String command = ReadSerialLine();
  //Serial.println(analogRead(A0)); //0 is 104-103 and 180 is 663 or so. 
  if(command.length() > 0)
  {
    StringParse(command);
  }
}
