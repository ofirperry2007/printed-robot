#include "Arduino.h"
#include "ev3lego.h"

volatile long degreesA = 0;  // זווית מנוע A
volatile long degreesB = 0;  // זווית מנוע B

int encoderPin1, encoderPin2; // פינים לאנקודר של מנוע A
int encoderPin3, encoderPin4; // פינים לאנקודר של מנוע B

unsigned long previousTime = 0;
double lastError = 0;
double cumError = 0;
bool integralFlag = false;

void ev3lego::run() {
    // לא מבוצעת פעולה כלשהי
}

ev3lego::ev3lego(int encoder1, int encoder2, int encoder3, int encoder4, int in10, int in20, int enA0, int in11, int in21, int enB1, int in12, int in22, int enC2, int wheel) {
  _encoder1 = encoder1;
  _encoder2 = encoder2;
  _encoder3 = encoder3; 
  _encoder4 = encoder4;
  _in10 = in10;
  _in20 = in20;
  _enA0 = enA0;
  _in11 = in11;
  _in21 = in21;
  _enB1 = enB1;
  _in12 = in12;
  _in22 = in22;
  _enC2 = enC2;
  _wheel = wheel;
}

void countDegreesA() {
  if (digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
    degreesA++;
  } else {
    degreesA--;
  }
  Serial.print("degrees A: ");
  Serial.println(degreesA);
}

void countDegreesB() {
  if (digitalRead(encoderPin3) == digitalRead(encoderPin4)) {
    degreesB++;
  } else {
    degreesB--;
  }
  Serial.print("degrees B: ");
  Serial.println(degreesB);
}

void ev3lego::begin() {
  Serial.begin(115200);
  Serial.println("\nEV3 motor initialized!");

  pinMode(_encoder1, INPUT);
  pinMode(_encoder2, INPUT);
  pinMode(_encoder3, INPUT_PULLUP);
  pinMode(_encoder4, INPUT_PULLUP);
  pinMode(_in10, OUTPUT);
  pinMode(_in20, OUTPUT);
  pinMode(_enA0, OUTPUT);
  pinMode(_in11, OUTPUT);
  pinMode(_in21, OUTPUT);
  pinMode(_enB1, OUTPUT);
  pinMode(_in12, OUTPUT);
  pinMode(_in22, OUTPUT);
  pinMode(_enC2, OUTPUT);

  encoderPin1 = _encoder1;
  encoderPin2 = _encoder2;
  encoderPin3 = _encoder3;
  encoderPin4 = _encoder4;

  degreesA = 0;
  degreesB = 0;

  // חיבור לאנקודרים, כל אחד עם פונקציה מתאימה
  attachInterrupt(digitalPinToInterrupt(_encoder1), countDegreesA, CHANGE);  // אנסור למנוע A
  attachInterrupt(digitalPinToInterrupt(_encoder3), countDegreesB, CHANGE);  // אנסור למנוע B
  
  previousTime = millis();

  Serial.println("Setup finished");
}

long ev3lego::ang(int motnum) {
  switch (motnum) {
    case 0: return degreesA; // מנוע A
    case 1: return degreesB; // מנוע B
    default: return 0;
  }
}

double ev3lego::PIDcalc(double inp, int sp, int kp, int ki, int kd) {
  unsigned long currentTime = millis();
  double elapsedTime = (currentTime - previousTime) / 1000.0;
  double error = inp - sp;

  if (error * lastError < 0) {
    integralFlag = true;
    cumError = 0;
    Serial.println("Error direction changed, resetting integral.");
  } else {
    integralFlag = false;
  }

  if (!integralFlag) {
    cumError += error * elapsedTime;
  }

  if (elapsedTime > 0) {
    double rateError = (error - lastError) / elapsedTime;
    double out = kp * error + ki * cumError + kd * rateError;

    lastError = error;
    previousTime = currentTime;

    if (out > 254) out = 254;
    if (out < -254) out = -254;

    Serial.print("degrees = ");
    Serial.println(degreesA);  // אם רוצים לעקוב אחרי מנוע A
    Serial.print("PID output = ");
    Serial.println(out);

    return out;
  }

  return 0;
}

void ev3lego::motgo(int speed, int motnum) {
  if (speed == 0) {
    switch (motnum) {
      case 0:
        digitalWrite(_in10, LOW);
        digitalWrite(_in20, LOW);
        analogWrite(_enA0, 0);
        break;
      case 1:
        digitalWrite(_in11, LOW);
        digitalWrite(_in21, LOW);
        analogWrite(_enB1, 0);
        break;
      case 2:
        digitalWrite(_in12, LOW);
        digitalWrite(_in22, LOW);
        analogWrite(_enC2, 0);
        break;
    }
    return;
  }

  switch (motnum) {
    case 0:
      if (speed > 0) {
        digitalWrite(_in10, HIGH);
        digitalWrite(_in20, LOW);
      } else {
        digitalWrite(_in10, LOW);
        digitalWrite(_in20, HIGH);
      }
      analogWrite(_enA0, abs(speed));
      break;

    case 1:
  if (speed > 0) {
    digitalWrite(_in11, LOW);     // היה HIGH
    digitalWrite(_in21, HIGH);    // היה LOW
  } else {
    digitalWrite(_in11, HIGH);    // היה LOW
    digitalWrite(_in21, LOW);     // היה HIGH
  }
  analogWrite(_enB1, abs(speed));
  break;


    case 2:
      if (speed > 0) {
        digitalWrite(_in12, HIGH);
        digitalWrite(_in22, LOW);
      } else {
        digitalWrite(_in12, LOW);
        digitalWrite(_in22, HIGH);
      }
      analogWrite(_enC2, abs(speed));
      break;
  }
}

void ev3lego::godegrees(int angle, int times) {
  for (int i = 0; i < times; i++) {
    int speed = PIDcalc(angle, degreesA, 1.5, 1, 0.1);  // מנוע A
    speed = constrain(speed, -254, 254);
    motgo(speed, 0);
    motgo(speed, 1);
    Serial.print("Motor speed = ");
    Serial.println(speed);
    delay(10);
  }

  Serial.print("Final degrees = ");
  Serial.println(degreesA);
}

void ev3lego::godegreesp_until(int targetAngle, int kp, int ki, int kd, bool rotateInPlace) {
  int errorMargin = 3;  // כמה מעלות סטייה מותרת
  cumError = 0;
  lastError = 0;
  integralFlag = false;
  previousTime = millis();

  while (true) {
    int currentA = degreesA;
    int currentB = degreesB;
    int avgDegrees = (currentA + currentB) / 2;

    // חישוב שגיאה בהתאם לסוג התנועה
    int error = 0;
    if (rotateInPlace) {
      error = targetAngle - abs(currentA - currentB);  // לבדוק אם הגענו לזווית הסיבוב
    } else {
      error = targetAngle - avgDegrees;  // חישוב השגיאה בתנועה ישרה
    }

    // אם השגיאה בתוך טווח המותר, עוצרים
    if (abs(error) <= errorMargin) {
      break;
    }

    // חישוב המהירות על פי PID
    int speed = PIDcalc(rotateInPlace ? abs(currentA - currentB) : avgDegrees, targetAngle, kp, ki, kd);
    speed = constrain(speed, -254, 254);  // קיבוע המהירות בתחום המותר

    // תנועה: אם סיבוב במקום, מנועים בזוויות הפוכות
    if (rotateInPlace) {
      motgo(speed, 0);     // ימני
      motgo(-speed, 1);    // שמאלי הפוך
    } else {
      motgo(speed, 0);     // שניהם באותו כיוון
      motgo(speed, 1);
    }

    delay(10);  // המתן לפני ביצוע קריאה מחדש
  }

  // עצירה מוחלטת של המנועים אחרי הגעה למטרה
  motgo(0, 0);
  motgo(0, 1);
}


double ev3lego::gomm(int distance, int times) {
  return 0;
}

double ev3lego::gommp(int distance, int times, int kp, int ki, int kd) {
  return 0;
}

void ev3lego::blink(int sec){ // מסובב את הכננת קדימה ואחורה לפי מספר שניות
  motgo(100, 2); //  Winch motor (W)
  delay(sec);
  motgo(0, 2); //  Winch motor (W)
  delay(sec);
  motgo(-100, 2); //  Winch motor (W)
  delay(sec);
}

void ev3lego::stopWinch(){ // stopping winch
  motgo(0, 2); //  Winch motor (W)
}

void ev3lego::resetEncoders() {
  noInterrupts();
  degreesA = 0;
  degreesB = 0;
  interrupts();
}

void ev3lego::goStraightPID(int degreesTarget, int kp, int ki, int kd) {
  resetEncoders();
  cumError = 0;
  lastError = 0;
  integralFlag = false;
  previousTime = millis();

  while (true) {
    int currentA = degreesA;
    int currentB = degreesB;
    int avg = (currentA + currentB) / 2;
    int error = degreesTarget - avg;
    if (abs(error) < 3) break;

    int speed = PIDcalc(avg, degreesTarget, kp, ki, kd);
    speed = constrain(speed, -200, 200);
    motgo(speed, 0);
    motgo(speed, 1);
    delay(10);
  }

  motgo(0, 0);
  motgo(0, 1);
}

void ev3lego::rotate90PID(int degreesTarget, int kp, int ki, int kd) {
  resetEncoders();
  cumError = 0;
  lastError = 0;
  integralFlag = false;
  previousTime = millis();

  while (true) {
    int diff = abs(degreesA - degreesB);
    int error = degreesTarget - diff;

    Serial.print("Rotate diff: ");
    Serial.println(diff);

    if (abs(error) < 3) break;

    int speed = PIDcalc(diff, degreesTarget, kp, ki, kd);
    speed = constrain(speed, -200, 200);

    // נסיעה הפוכה במנוע B
    motgo(speed, 0);    // מנוע ימני - קדימה
    motgo(-speed, 1);   // מנוע שמאלי - אחורה
    delay(10);
  }

  motgo(0, 0);
  motgo(0, 1);
}


void ev3lego::goSquare() {
  int wheelDiameterMM = 65;
  int baseWidthMM = 226;
  int sideLengthMM = 300;  // טווח ארוך יותר
  int kp = 3, ki = 1, kd = 1;

  double wheelCircumference = PI * wheelDiameterMM;
  int degreesPerSide = (360.0 * sideLengthMM) / wheelCircumference;

  // חישוב דרגת סיבוב של 90°
  int turnDegrees = (baseWidthMM * PI / 4.0) / wheelCircumference * 360.0;

  Serial.println("Starting square movement...");
  for (int i = 0; i < 4; i++) {
    goStraightPID(degreesPerSide, kp, ki, kd);
    delay(300);
    rotate90PID(turnDegrees, kp, ki, kd);
    delay(300);
  }
  Serial.println("Finished square.");
}
