#include "Arduino.h"             // כולל את ספריית ארדואינו – פונקציות כמו digitalWrite, millis וכו'
#include "ev3lego.h"             // כולל את ההגדרות והפונקציות של מחלקת ev3lego (שולטת ברובוט)

volatile long degreesA = 0;     // שומר כמה צעדים (דירגות) מנוע A עשה
volatile long degreesB = 0;     // שומר כמה צעדים מנוע B עשה

int encoderPin1, encoderPin2;   // מספרי פינים של חיישני אנקודר של מנוע A
int encoderPin3, encoderPin4;   // מספרי פינים של חיישני אנקודר של מנוע B

unsigned long previousTime = 0; // הזמן האחרון שבו נמדדה שגיאה – עבור PID
double lastError = 0;           // שגיאה קודמת – להפרש בשגיאה בזמן עבור PID
double cumError = 0;            // צבירה של שגיאות – החלק ה־I של PID
bool integralFlag = false;      // דגל – האם לצבור שגיאות או לא

ev3lego::ev3lego(int encoder1, int encoder2, int encoder3, int encoder4,
                 int in10, int in20, int enA0,
                 int in11, int in21, int enB1,
                 int in12, int in22, int enC2,
                 int wheel) {
  _encoder1 = encoder1;       // שומר את הפין של אנקודר A1
  _encoder2 = encoder2;       // שומר את הפין של אנקודר A2
  _encoder3 = encoder3;       // שומר את הפין של אנקודר B1
  _encoder4 = encoder4;       // שומר את הפין של אנקודר B2
  _in10 = in10;               // פין כיוון למנוע A
  _in20 = in20;               // פין כיוון נוסף למנוע A
  _enA0 = enA0;               // פין מהירות למנוע A
  _in11 = in11;               // פין כיוון למנוע B
  _in21 = in21;               // פין כיוון נוסף למנוע B
  _enB1 = enB1;               // פין מהירות למנוע B
  _in12 = in12;               // פין כיוון למנוע C
  _in22 = in22;               // פין כיוון נוסף למנוע C
  _enC2 = enC2;               // פין מהירות למנוע C
  _wheel = wheel;             // שומר את גודל הגלגל
}

void countDegreesA() {
  if (digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
    degreesA++;                // אם שני הפינים שווים – המנוע הסתובב קדימה
  } else {
    degreesA--;                // אחרת – אחורה
  }
}

void countDegreesB() {
  if (digitalRead(encoderPin3) == digitalRead(encoderPin4)) {
    degreesB++;                // קדימה
  } else {
    degreesB--;                // אחורה
  }
}

void ev3lego::begin() {                        // מכין את הרובוט לפעולה
  Serial.begin(115200);                        // מפעיל תקשורת עם המחשב
  Serial.println("\nEV3 motor initialized!");   // מדפיס הודעת התחלה

  pinMode(_encoder1, INPUT);                   // הגדרת פיני חיישני אנקודר כקלט
  pinMode(_encoder2, INPUT);
  pinMode(_encoder3, INPUT_PULLUP);
  pinMode(_encoder4, INPUT_PULLUP);

  pinMode(_in10, OUTPUT);                      // פיני כיוון ומהירות למנועים
  pinMode(_in20, OUTPUT);
  pinMode(_enA0, OUTPUT);
  pinMode(_in11, OUTPUT);
  pinMode(_in21, OUTPUT);
  pinMode(_enB1, OUTPUT);
  pinMode(_in12, OUTPUT);
  pinMode(_in22, OUTPUT);
  pinMode(_enC2, OUTPUT);

  encoderPin1 = _encoder1;                     // שמירה של הפינים לשימוש בפונקציות הספירה
  encoderPin2 = _encoder2;
  encoderPin3 = _encoder3;
  encoderPin4 = _encoder4;

  degreesA = 0;                                // מאפס את הספירה
  degreesB = 0;

  attachInterrupt(digitalPinToInterrupt(_encoder1), countDegreesA, CHANGE); // קובע תגובה לשינוי מצב חיישן A
  attachInterrupt(digitalPinToInterrupt(_encoder3), countDegreesB, CHANGE); // כנ"ל לחיישן B

  previousTime = millis();                      // שומר את הזמן הנוכחי כדי להתחיל למדוד לפונקציית PID
  Serial.println("Setup finished");             // מדפיס הודעה שההגדרות הסתיימו
}
long ev3lego::ang(int motnum) {                  // פונקציה שמחזירה את הזווית של מנוע לפי מספר (0 או 1)
  switch (motnum) {                              // בודקים איזה מספר מנוע קיבלנו
    case 0: return degreesA;                     // אם זה מנוע מספר 0 – נחזיר את הזווית של מנוע A
    case 1: return degreesB;                     // אם זה מנוע מספר 1 – נחזיר את הזווית של מנוע B
    default: return 0;                           // אם קיבלנו מספר אחר – נחזיר 0 (לא קיים)
  }
}

void ev3lego::printDegrees() {                   // פונקציה שמדפיסה את זוויות המנועים
  Serial.print("degreesA: ");                    // מדפיס את הטקסט "degreesA: "
  Serial.println(degreesA);                      // מדפיס את המספר של זווית מנוע A בשורה נפרדת
  Serial.print(" degreesB: ");                   // מדפיס את הטקסט "degreesB: "
  Serial.println(degreesB);                      // מדפיס את המספר של זווית מנוע B בשורה נפרדת
}

double ev3lego::PIDcalc(double inp, int sp, int kp, int ki, int kd) {
  unsigned long currentTime = millis();          // שומר את הזמן הנוכחי במילישניות מאז שה-Arduino התחיל לפעול
  double elapsedTime = (currentTime - previousTime) / 1000.0; // מחשב כמה זמן עבר מאז הפעם הקודמת, בשניות
  double error = inp - sp;                       // מחשב את ה"שגיאה" – כמה אנחנו רחוקים מהמטרה

  if (error * lastError < 0) {                   // אם השגיאה שינתה כיוון (לדוגמה מ+ ל-)
    integralFlag = true;                         // מדליקים דגל שמסמן שהשגיאה עברה דרך 0
    cumError = 0;                                // מאפסים את השגיאה המצטברת כדי לא להוסיף ערכים מיותרים
  } else {
    integralFlag = false;                        // אם השגיאה לא עברה דרך 0 – מכבים את הדגל
  }

  if (!integralFlag) {                           // רק אם השגיאה לא שינתה כיוון
    cumError += error * elapsedTime;             // מוסיפים לשגיאה המצטברת את השגיאה הנוכחית כפול הזמן שחלף
  }

  if (elapsedTime > 0) {                         // אם באמת עבר זמן (ולא התחלה של המדידה)
    double rateError = (error - lastError) / elapsedTime; // מחשבים את קצב השינוי של השגיאה (נגזרת)
    double out =                                  
      kp * error +                                // חלק פרופורציונלי (P): מתקן לפי כמה אנחנו רחוקים מהמטרה
      ki * cumError +                             // חלק אינטגרלי (I): מצטבר עם הזמן כדי לשפר תיקון איטי
      kd * rateError;                             // חלק נגזרי (D): בודק כמה מהר השגיאה משתנה כדי לא "לעוף" יותר מדי

    lastError = error;                            // מעדכנים את השגיאה האחרונה לזו הנוכחית
    previousTime = currentTime;                   // מעדכנים את הזמן האחרון למדידה הנוכחית

    out = constrain(out, -254, 254);              // מגבילים את הפלט לטווח -254 עד 254 כדי שלא יהיה מוגזם
    return out;                                   // מחזירים את הפלט של הבקר
  }

  return 0;                                       // אם לא עבר זמן – פשוט מחזירים 0
}
void ev3lego::motgo(int speed, int motnum) {     // פונקציה שמפעילה מנוע לפי מהירות ומספר מנוע (0, 1 או 2)
  if (speed == 0) {                               // אם המהירות היא 0 – עוצרים את המנוע
    switch (motnum) {
      case 0:                                     // מנוע A
        digitalWrite(_in10, LOW);                 // מפסיקים זרם קדימה
        digitalWrite(_in20, LOW);                 // מפסיקים זרם אחורה
        analogWrite(_enA0, 0);                    // לא שולחים זרם למנוע
        break;
      case 1:                                     // מנוע B
        digitalWrite(_in11, LOW);
        digitalWrite(_in21, LOW);
        analogWrite(_enB1, 0);
        break;
      case 2:                                     // מנוע C (נוסף)
        digitalWrite(_in12, LOW);
        digitalWrite(_in22, LOW);
        analogWrite(_enC2, 0);
        break;
    }
    return;                                       // יוצאים מהפונקציה (לא מפעילים את שאר הקוד)
  }

  switch (motnum) {                               // אחרת – מפעילים את המנוע בכיוון ובמהירות הנכונים
    case 0:
      digitalWrite(_in10, speed > 0);             // אם המהירות חיובית – קדימה, אחרת – אחורה
      digitalWrite(_in20, speed <= 0);
      analogWrite(_enA0, abs(speed));             // שולחים זרם לפי ערך מוחלט של המהירות
      break;
    case 1:
      digitalWrite(_in11, speed <= 0);
      digitalWrite(_in21, speed > 0);
      analogWrite(_enB1, abs(speed));
      break;
    case 2:
      digitalWrite(_in12, speed > 0);
      digitalWrite(_in22, speed <= 0);
      analogWrite(_enC2, abs(speed));
      break;
  }
}

void ev3lego::godegreesp_until(int targetAngle, int kp, int ki, int kd, bool rotateInPlace) {
  int errorMargin = 3;                            // טווח טעות שמותר (כמה מעלות אפשר לטעות)
  cumError = 0;                                   // איפוס של השגיאה המצטברת
  lastError = 0;                                  // איפוס של השגיאה האחרונה
  integralFlag = false;                           // מכבים את דגל ה-integral
  previousTime = millis();                        // מתחילים לספור זמן

  while (true) {                                  // לולאה שרצה עד שנעצור אותה מבפנים
    int currentA = degreesA;                      // שומרים את הזווית הנוכחית של מנוע A
    int currentB = degreesB;                      // שומרים את הזווית הנוכחית של מנוע B
    int avgDegrees = (currentA + currentB) / 2;   // ממוצע הזוויות – למקרה של תנועה ישרה

    int error = rotateInPlace ?                   // אם רוצים להסתובב במקום – מחשבים שגיאה לפי ההפרש בין המנועים
      targetAngle - abs(currentA - currentB) :    // סיבוב במקום
      targetAngle - avgDegrees;                   // תנועה ישרה

    if (abs(error) <= errorMargin) break;         // אם אנחנו מספיק קרובים למטרה – יוצאים מהלולאה

    int speed = PIDcalc(                           // מחשבים מה המהירות הנכונה לתקן את השגיאה
      rotateInPlace ? abs(currentA - currentB) : avgDegrees,
      targetAngle, kp, ki, kd);

    speed = constrain(speed, -254, 254);          // מגבילים את המהירות שלא תהיה מוגזמת

    if (rotateInPlace) {                           // אם אנחנו בסיבוב במקום
      motgo(-speed, 0);                            // מנוע A אחורה
      motgo(speed, 1);                             // מנוע B קדימה
    } else {                                       // תנועה ישרה
      motgo(speed, 0);                             // שני המנועים קדימה
      motgo(speed, 1);
    }

    delay(10);                                     // מחכים 10ms לפני שממשיכים כדי לא להציף את הלולאה
  }
motgo(0, 0);                                  // עוצר את מנוע 0
motgo(0, 1);                                  // עוצר את מנוע 1
}

void ev3lego::driveWithPID(int targetDegrees, bool rotateInPlace, int kp, int ki, int kd) {
  resetEncoders();                            // מאפס את האנקודרים כדי להתחיל למדוד מהתחלה
  godegreesp_until(targetDegrees, kp, ki, kd, rotateInPlace); // נוסע לזווית יעד עם בקר PID
}

void ev3lego::resetEncoders() {
  degreesA = 0;                               // מאפס את מונה הזווית למנוע A
  degreesB = 0;                               // מאפס את מונה הזווית למנוע B
}

void ev3lego::goStraightWithPID(int targetTicks, int kp, int ki, int kd) {
  resetEncoders();                            // מאפס את האנקודרים כדי להתחיל מחדש

  while (true) {                              // לולאה שרצה עד שמגיעים למטרה
    long ticksA = ang(0);  // קורא את הזווית הנוכחית של מנוע A
    long ticksB = ang(1);  // קורא את הזווית הנוכחית של מנוע B
    long avgTicks = (ticksA + ticksB) / 2;    // מחשב ממוצע של שני המנועים

    if (abs(avgTicks) >= targetTicks) break; // אם הגענו או עברנו את היעד – יוצאים מהלולאה

    double power = PIDcalc(avgTicks, targetTicks, kp, ki, kd); // מחשב כוח לתיקון באמצעות PID

    motgo(power, 0);  // מפעיל את מנוע A עם הכוח המחושב
    motgo(power, 1);  // מפעיל את מנוע B עם הכוח המחושב

    Serial.print("Ticks A: "); Serial.print(ticksA);         // מדפיס נתונים ל-Serial Monitor
    Serial.print(" | Ticks B: "); Serial.print(ticksB);
    Serial.print(" | Avg: "); Serial.println(avgTicks);

    delay(10);                                 // מחכה 10 מילישניות כדי לא להעמיס
  }

  motgo(0, 0);  // עוצר את מנוע A
  motgo(0, 1);  // עוצר את מנוע B

  Serial.println("Finished driving straight."); // מדפיס סיום
}
