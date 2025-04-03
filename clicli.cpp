/*  
                    CLI Software for Arduino

               A Simple Command Line Interface 
  Example functions:              
              Feature                |  CLI Usage
___________________________________________________
 Digital Write HIGH to a specific pin |  h (pin)
 Digital Write LOW to a specific pin  |  l (pin)
 Shoot command                        |  s (angle)
 Digital Read                         |  r (pin)
 Analog Read                          |  e (pin) 

      by Gal Arbel
      Oct 2022
*/

#include "clicli.h"
#include "Arduino.h"

const unsigned int MAX_MESSAGE_LENGTH = 64;

// בנאי של המחלקה clicli
clicli::clicli(ev3lego &mylego) : mylego(mylego), number(7) {
    // גוף הבנאי, אם צריך
}

void clicli::begin() {
    Serial.begin(9600);
    Serial.println("CLI interface started.");
}

void clicli::run() {
    // CLI - קבלת פקודות מהטרמינל
    while (Serial.available() > 0) { 
        char message[MAX_MESSAGE_LENGTH];
        static unsigned int message_pos = 0;
        char inByte = Serial.read(); // קריאת תו מהבאפר הסיריאלי

        if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
            message[message_pos] = inByte; // הוספת התו להודעה
            message_pos++;
        } else {
            message[message_pos] = '\0'; // סיום המחרוזת
            Serial.print("Received: ");
            Serial.println(message); // הדפסת ההודעה חזרה לטרמינל

            int command[4] = {0};
            int argindex = 0;
            char cmd;
            char delim[] = " ";
            char tmpmsg[MAX_MESSAGE_LENGTH];
            
            strcpy(tmpmsg, message);
            message_pos = 0;
            message[message_pos] = '\0'; // איפוס ההודעה

            char *ptr = strtok(tmpmsg, delim);
            while (ptr != NULL) {
                if (argindex == 0) {
                    cmd = ptr[0];
                }
                command[argindex] = atoi(ptr);   
                argindex++;  
                ptr = strtok(NULL, delim);
            } 

            // עיבוד הפקודה שהתקבלה
            switch (cmd) {
                case 'h': // Set port to HIGH
                    pinMode(command[1], OUTPUT);
                    digitalWrite(command[1], HIGH);
                    Serial.print("Pin "); 
                    Serial.print(command[1]);   
                    Serial.println(" is SET");
                    break;
                
                case 'l': // Set port to LOW
                    pinMode(command[1], OUTPUT);
                    digitalWrite(command[1], LOW);
                    Serial.print("Pin "); 
                    Serial.print(command[1]);   
                    Serial.println(" is RESET");   
                    break;
                
                case 'r': // digital read
                    pinMode(command[1], INPUT);
                    Serial.print("Pin "); 
                    Serial.print(command[1]);   
                    Serial.print(" Value = "); 
                    Serial.println(digitalRead(command[1]));   
                    break;
                
                case 'e': // analog read
                    pinMode(command[1], INPUT);
                    Serial.print("Pin "); 
                    Serial.print(command[1]);   
                    Serial.print(" Value = "); 
                    Serial.println(analogRead(command[1]));   
                    break;

                case 'v': // שליטה על מנוע
                    mylego.motgo(command[1], command[2]);  
                    Serial.print("Motor speed set to ");
                    Serial.println(command[1]);
                    break;

                case 'p': // הפעלת פונקציה godegreesp
                    mylego.godegreesp(command[1], command[2], command[3], command[4], 0);
                    Serial.print("Executing godegreesp with angle: ");
                    Serial.println(command[1]);
                    break;
                
                case 'q': // הפעלת פונקציה godegrees
                    mylego.godegrees(command[1], command[2]);
                    Serial.print("Executing godegrees with angle: ");
                    Serial.println(command[1]);
                    break;

                default:
                    Serial.println("Unknown command.");
                    break;
            }
        }
    }
    delay(60); 
}
