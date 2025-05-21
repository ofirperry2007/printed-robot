#ifndef EV3LEGO_H
#define EV3LEGO_H

class ev3lego {
  
  public:
   ev3lego(int encoder1, int encoder2, int encoder3, int encoder4,
           int in10, int in2
   ev3lego(int encoder1, int encoder2, int encoder3, int encoder4,
           int in10, int in20, int enA0,
           int in11, int in21, int enB1,
           int in12, int in22, int enC2,
           int wheel);

   void begin();
   void run();

   long ang(int motnum);                // קריאת ערך אנקודר למנוע A/B
   void resetEncoders();               // איפוס כל האנקודרים

   void godegrees(int angle, int motnum); // תנועה במעלות עבור מנוע בודד
   void godegreesp_until(int angle, int kp, int ki, int kd, bool rotateInPlace = false); // PID תנועה לפי זווית עם

   double PIDcalc(double inp, int sp, int kp, int ki, int kd); // חישוב PID
   void motgo(int speed, int motnum);  // הפעלת מנוע

   double gomm(int distance, int times); // תנועה לפי מרחק
   double gommp(int distance, int times, int kp, int ki, int kd); // תנועה לפי מרחק עם PID

   void blink(int sec);                // הבהוב נורית
   void stopWinch();                   // עצירת מנוף
   void goSquare();                    // נסיעה בריבוע

   void goStraightPID(int degreesTarget, int kp, int ki, int kd); // תנועה ישרה עם PID
   void rotate90PID(int degreesTarget, int kp, int ki, int kd);   // סיבוב של 90 מעלות עם PID

  private:
   int _encoder1, _encoder2, _encoder3, _encoder4;
   int _in10, _in20, _enA0;
   int _in11, _in21, _enB1;
   int _in12, _in22, _enC2;
   int _wheel;

   double _kp, _ki, _kd;
   bool integralflag = false;
};

#endif // סיום ההצהרה של ה-#ifndef
