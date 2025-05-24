#include "ev3lego.h"        // כולל את הספרייה שמנהלת את המנועים והאנקודרים
#include "clicli.h"         // כולל ספרייה להתנהגות שורת פקודה (אולי לשליטה חיצונית)

const int buttonPin = 10;  // מגדיר שכפתור מחובר לפין 10

ev3lego roboPaint(
    2,   // encoder1 (yellow) – אנקודר במנוע a  
    A4,  // encoder2 (blue) – אנקודר במנוע a 
    3,   // encoder3 (yellow) – אנקודר במנוע b  
    A5,  // encoder4 (blue) – אנקודר במנוע b  
    8,   // in10 – פין כיוון למנוע ראשון
    7,   // in20 – פין כיוון נוסף למנוע ראשון
    9,   // enA0 – פין הפעלה (PWM) למנוע ראשון מהירות 
    12,  // in11 – פין כיוון למנוע שני
    13,  // in21 – פין כיוון נוסף למנוע שני
    11,  // enB1 – פין הפעלה למנוע שני מהירות 
    4,   // in12 – פין כיוון למנוע שלישי
    5,   // in22 – פין כיוון נוסף למנוע שלישי
    6,   // enC2 – פין הפעלה למנוע שלישי מהירות
    65   // wheel size in mm – גודל גלגל במילימטרים
);

void setup() {
  Serial.begin(115200);               // מתחיל תקשורת סיריאלית במהירות 115200 ביט
  roboPaint.begin();                  // מאתחל את המערכת של הרובוט
  pinMode(buttonPin, INPUT_PULLUP);  // מגדיר את הכפתור כך שיזהה לחיצה
  Serial.println("Press button on pin 10 ");  // מדפיס הודעה למסך
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {  // בודק אם לחצו על הכפתור
    Serial.println("start");            // מדפיס "start" למסך
    roboPaint.motgo(-255, 2);           // מפעיל את מנוע 2 לכיוון קדימה (כננת שואבת)
    delay(8000);                        // מחכה 8 שניות בזמן שהכננת עובדת
    roboPaint.goStraightWithPID(2000, 10, 2, 5);  // נוסע ישר 2000 דפיקות PID
    roboPaint.motgo(250, 2);            // מסובב את מנוע 2 אחורה לשחרור לחץ
    delay(4500);                        // מחכה 4.5 שניות לשחרור מלא
    roboPaint.motgo(0, 2);              // עוצר את מנוע 2 (כננת)
    Serial.println("done.");           // מדפיס "done." למסך
    delay(1000);                        // מונע לחיצה כפולה – מחכה שנייה
  }
}
