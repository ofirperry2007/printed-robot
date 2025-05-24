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

#include "clicli.h"               // מוסיף את הקובץ של הפקודות האישיות שלנו
#include "Arduino.h"              // קובץ שמאפשר להשתמש בפונקציות של ארדואינו
#include "HardwareSerial.h"       // מוסיף תמיכה בתקשורת סיריאלית (USB)

const unsigned int MAX_MESSAGE_LENGTH = 64;  // כמה תווים אפשר לקבל בפקודה אחת

// בנאי של המחלקה, שומר את המנועים שאנחנו שולטים עליהם
clicli::clicli(ev3lego &mylego) : mylego(mylego), number(7) {
    // לא עושים כלום בבנאי עצמו כרגע
}

void clicli::begin() {
  // פונקציה ריקה, אפשר להכניס דברים שנרצה שיקרו בהתחלה
}

void clicli::run() {

 static char message[MAX_MESSAGE_LENGTH];  // איפה שומרים את הפקודה שהמשתמש כותב
 static unsigned int message_pos = 0;      // באיזה תו אנחנו נמצאים

while (Serial.available() > 0) {           // כל עוד יש מידע חדש מהמשתמש

  char inByte = Serial.read();             // קרא תו חדש מהמסך
  Serial.print("Got byte: ");
  Serial.println(inByte);                 // הדפס מה שקיבלת

  // אם זה לא Enter ועדיין לא הגענו לסוף
  if (inByte != '\n' && inByte != '\r' && message_pos < MAX_MESSAGE_LENGTH - 1) {
    message[message_pos++] = inByte;      // שים את התו בתוך ההודעה
  } else if (message_pos > 0) {           // אם כן נגמרה ההודעה
    message[message_pos] = '\0';         // סיים את המחרוזת
    Serial.println(message);              // תראה מה המשתמש שלח

    int command[5];                       // כאן נשים את המספרים שהמשתמש שלח
    int argindex = 0;                     // מונה כמה פרמטרים קיבלנו
    char cmd = '\0';                     // מה סוג הפקודה (האות הראשונה)
    char delim[] = " ";                  // מפריד לפי רווחים
    char tmpmsg[MAX_MESSAGE_LENGTH];     // עותק של ההודעה שנפרק

    strcpy(tmpmsg, message);             // עושים העתק כדי לא להרוס את המקור
    message_pos = 0;                     // מתחילים מחדש לקלוט הודעה חדשה

    char *ptr = strtok(tmpmsg, delim);   // לוקחים מילה ראשונה
    while (ptr != NULL) {
      if (argindex == 0) {
        cmd = ptr[0];                    // האות הראשונה היא סוג הפקודה
      } else {
        command[argindex] = atoi(ptr);  // הופכים טקסט למספר
      }
      argindex++;
      ptr = strtok(NULL, delim);        // ממשיכים למילה הבאה
    }

    Serial.print("Parsed command: ");
    Serial.println(cmd);                // מראים איזו פקודה נקלטה
    Serial.print("Args: ");
    for (int i = 1; i < argindex; i++) {
      Serial.print(command[i]);         // מראים את כל המספרים שהיו בה
      Serial.print(" ");
    }
    Serial.println();

    if (argindex < 2) {
      Serial.println("Not enough arguments");  // אם חסר מידע - לא ממשיכים
      return;
    }

    // לפי סוג הפקודה - עושים פעולה
    switch (cmd) {
      case 'h': // הדלק פין (HIGH)
        pinMode(command[1], OUTPUT);
        digitalWrite(command[1], HIGH);
        Serial.print("Pin ");
        Serial.print(command[1]);
        Serial.println(" is SET");
        break;

      case 'l': // כבה פין (LOW)
        pinMode(command[1], OUTPUT);
        digitalWrite(command[1], LOW);
        Serial.print("Pin ");
        Serial.print(command[1]);
        Serial.println(" is RESET");
        break;

      case 'r': // קרא ערך דיגיטלי מפין
        pinMode(command[1], INPUT);
        Serial.print("Pin ");
        Serial.print(command[1]);
        Serial.print(" Value = ");
        Serial.println(digitalRead(command[1]));
        break;

      case 'e': // קרא ערך אנלוגי מפין
        pinMode(command[1], INPUT);
        Serial.print("Pin ");
        Serial.print(command[1]);
        Serial.print(" Value = ");
        Serial.println(analogRead(command[1]));
        break;

      case 'v': // הרץ מנוע מסוים במהירות
        mylego.motgo(command[1], command[2]);
        Serial.print("Motor ");
        Serial.print(command[2]);
        Serial.print(" set to speed ");
        Serial.println(command[1]);
        break;

      case 'p': // תנועה מדויקת עם PID
        mylego.godegreesp_until(command[1], command[2], command[3], command[4], 0);
        Serial.print("Going ");
        Serial.print(command[1]);
        Serial.println(" degrees (custom PID)");
        break;

      /*case 'q': // פקודה שבוטלה
        mylego.godegrees(command[1], command[2]);
        Serial.print("Going ");
        Serial.print(command[1]);
        Serial.println(" degrees");
        break; */
    }
    message_pos = 0; // מכינים מקום לפקודה הבאה
  }
  delay(60); // מחכים קצת כדי לא להציף
}

}
