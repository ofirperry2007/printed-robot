#ifndef EV3LEGO_H                          // הגדרה שמונעת טעינה כפולה של הקובץ הזה
#define EV3LEGO_H

#include <Arduino.h>                   // כולל את הספרייה של ארדואינו – חובה בשביל הפונקציות כמו digitalWrite וכו'

class ev3lego {                        // הגדרת המחלקה ev3lego – כל מה שקשור למנועים ופקודות
public:
  ev3lego(                             // בנאי – הפונקציה שמתבצעת כשמייצרים אובייקט חדש מהמחלקה
    int encoder1, int encoder2, int encoder3, int encoder4, // כניסות של חיישני האנקודר למנועים
    int in10, int in20, int enA0,                      // פינים של מנוע A
    int in11, int in21, int enB1,                      // פינים של מנוע B
    int in12, int in22, int enC2,                      // פינים של מנוע C (שלישי)
    int wheel);                                        // גודל גלגל – אולי בשביל חישובי מרחק

  void begin();                                        // פונקציה להפעלה ראשונית – רצה ב-setup
  long ang(int motnum);                               // מחזירה את הזווית של מנוע לפי מספר (0/1)
  void motgo(int speed, int motnum);                  // מפעילה מנוע לפי מהירות ומספר מנוע
  void printDegrees();                                // מדפיסה את ערכי הזווית של המנועים ל-Serial
  double PIDcalc(double inp, int sp, int kp, int ki, int kd); // מחשבת את הבקר PID
  void godegreesp_until(int targetAngle, int kp, int ki, int kd, bool rotateInPlace); // נוסע לזווית מסוימת עם PID
  void driveWithPID(int targetDegrees, bool rotateInPlace, int kp, int ki, int kd);   // פונקציה דומה כנראה לנסיעה מותאמת
  void resetEncoders();                               // מאפסת את ערכי האנקודרים לזווית אפס

  void goStraightWithPID(int targetTicks, int kp, int ki, int kd);  // נסיעה ישרה לפי PID לפי "טיקים"

private:
  int _encoder1, _encoder2, _encoder3, _encoder4;      // משתנים פרטיים לשמירת פינים של אנקודרים
  int _in10, _in20, _enA0;                             // פינים פרטיים של מנוע A
  int _in11, _in21, _enB1;                             // פינים פרטיים של מנוע B
  int _in12, _in22, _enC2;                             // פינים פרטיים של מנוע C
  int _wheel;                                          // גודל הגלגל או משהו שקשור לחישוב תנועה
};

#endif                                                // סוף ההגדרה שמונעת טעינה כפולה
