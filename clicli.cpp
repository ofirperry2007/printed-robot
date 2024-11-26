/*

                    CLI Software for Arduino

               A Simple Command Line Interface 
  Example functions:              
              Feature                |  CLI Usage
___________________________________________________
 Digial Write HIGH to a specific pin |  h (pin)
 Digial Write LOW to a specific pin  |  l (pin)
 Shoot command                       |  s (angle)
 Digital Read                        |  r (pin)
 Analog Read                         |  e (pin) 

               
      by Gal Arbel
      Oct 2022
*/


#include "clicli.h"
#include "Arduino.h"
#include "HardwareSerial.h"
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

const unsigned int MAX_MESSAGE_LENGTH = 64;

clicli::clicli(ev3lego &mylego) : mylego(mylego), number(7) {
    // Constructor body, if needed
}

void clicli::begin() {
  //
}
void clicli::run() {

// CLI - Messages from Terminal
  while (Serial.available() > 0) { 
   char message[MAX_MESSAGE_LENGTH];
   static unsigned int message_pos = 0;
   char inByte = Serial.read();   //Read the next available byte in the serial receive buffer
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
     {
     message[message_pos] = inByte;  //Add the incoming byte to our message
     message_pos++;
     }
     //Full message received...
     else
     {
      message[message_pos] = '\0';     //Add null character to string
      Serial.println(message);     //echo the message to terminal
        
      int command[4];
      int argindex = 0;
      char cmd;
      char delim[] = " ";
       char tmpmsg[MAX_MESSAGE_LENGTH];
       strcpy(tmpmsg,message);
       message_pos = 0;
       message[message_pos] = '\0';     //Add null character to string

        char *ptr = strtok(tmpmsg, delim);
        while(ptr != NULL)
         {
          //Serial.printf("'%s'\n", ptr);
          if (argindex == 0) {
            cmd = ptr[0];
          }
          command[argindex] = atoi(ptr);   
          //Serial.println(command[argindex]);
          argindex++;  
          ptr = strtok(NULL, delim);
         } 

      switch (cmd) {
       case 'h': //Set port to HIGH
        pinMode(command[1],OUTPUT);
        digitalWrite(command[1],HIGH);
        Serial.print("Pin "); 
        Serial.print(command[1]);   
        Serial.println(" is SET");   
        delay(1000);
        break;
        
       case 'l': // Set port to LOW
        pinMode(command[1],OUTPUT);
        digitalWrite(command[1],LOW);
        Serial.print("Pin "); 
        Serial.print(command[1]);   
        Serial.println(" is RESET");   
        delay(1000);
        break;
       
       case 'r': // digital read
        pinMode(command[1],INPUT);
        Serial.print("Pin "); 
        Serial.print(command[1]);   
        Serial.print(" Value = "); 
        Serial.println(digitalRead(command[1]));   
        delay(1000);
        break;
       
       case 'a':
        
        break;
       

      case 'e': // analog read
        pinMode(command[1],INPUT);
        Serial.print("Pin "); 
        Serial.print(command[1]);   
        Serial.print(" Value = "); 
        Serial.println(analogRead(command[1]));   
        delay(1000);
        break;

      case 'g'://print debug info
        mylego.godegreesp(command[1], 2000, command[2],command[3], command[4]);
        mylego.motgo(0);
        break;

      case 'v':
        mylego.motgo(command[1]);
        break;
      
      case 'z':
        mylego.gomm(command[1], 1000);
        break;

      case 'p':
       mylego.godegreesp(command[1],command[2],command[3],command[4],0);
       Serial.print(command[1]);
       mylego.motgo(0);
       break;
       
      case 'q':
       mylego.godegrees(command[1],command[2]);
       Serial.print(command[1]);
       mylego.motgo(0);
       break;

       message_pos = 0;     //Reset for the next message
      }
   }
   delay (60); 
 } 
}
