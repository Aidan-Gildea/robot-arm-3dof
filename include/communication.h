#pragma once
#include <Arduino.h>
#include <motors.h>

/*
Have to a function for: 
ReadSerialLine > returns an empty string unless char = '\n'

CommandParser > Extract the main the command from a string,
put it into an array of broken strings

CommandDispatcher > Takes in the array from the command parser
Reads first index to get command and then 
uses rest of inputs to put into function

Basic architecture: CMD ARG1 ARG2 and any others. 

*/

void HelpCommand()
{
    Serial.println("List of Commands: ");
    Serial.println("SERVO index angle (moves a single servo)");
    Serial.println("ANGLE index (gets current angle of servo)");
    Serial.println("HOME (Resets all servos to 90 degrees)");
    Serial.println("HELP (provides list of commands)");
    Serial.println();
    Serial.println("Make sure a number/char is placed after space (Help 1)");
}

// void DispatchCommand(String command, String others)
// {
//     command.toUpperCase(); //also handling this in the other function but whatever
//     if(command == "SERVO")
//     {
//         ServoSetAngle(others);
//         return;
//     }
//     else if (command == "SERVOS")
//     {
//         ServosSetAngles(others);
//         return;
//     }
//     else if (command == "SPATH")
//     {

//         ServosSetPath(others);
//         return;
//     }
//     else if(command == "HELP")
//     {
//         HelpCommand();
//         return;
//     }
//     else if (command == "HOME")
//     {
//         HomeServos();
//         return;
//     }
//     else if(command == "ANGLE")
//     {
//         //this command is special because it is used for things outside of just commands for Serial
//         Serial.print(PrintServoAngle(others));
//         Serial.println();
//     }

//     else{
//         Serial.println("Invalid Command");
//     }
// }


// void StringParse(String line)
// {
//     line.trim();
//     if(line.length() == 0) return;

//     line.toUpperCase();

//     int spaceIndex = -1;
//     Serial.println("This is the line: ");
//     Serial.println(line);
//     for(int i = 0; i < line.length(); i++)
//     {
//         if(line[i] == ' ')
//         {
//             spaceIndex = i;
//             Serial.println("space index" );
//             Serial.print(spaceIndex);
//             break;
//         }
//     }
//     if(spaceIndex != -1) //for cases where there are no given perameters. Only putting here for convenience, few functions with no params. 
//     {
//         String command = line.substring(0, spaceIndex);
//         Serial.println(command);
//         String others = line.substring(spaceIndex + 1);
//         Serial.println(others);

//         DispatchCommand(command, others);
//     }
//     return;
    
// }


// String ReadSerialLine()
// {
//     static String command = "";
//     //Serial.print("hello");
//     while(Serial.available())
//     {
        
//         char c = Serial.read();
//         if(c =='\n')
//         {
//             String temp = command;
//             command = "";
//             Serial.println();
//             return temp;
//         }
//         else if(c=='\b')
//         {
//             command.remove(command.length() - 1);
//             Serial.print('\b');
//             Serial.print(" ");
//             Serial.print('\b');
//         }
//         else
//         {
//             command += c;
//             Serial.print(c);
//         }
//     }
//     return "";
// }


