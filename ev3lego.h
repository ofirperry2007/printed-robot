#ifndef EV3LEGO_H
#define EV3LEGO_H
 class ev3lego {

  public:
   ev3lego(int encoder1, int encoder2, int in1R, int in2R, int enaR, int in1L, int in2L, int enaL, int wheel);
   void begin();   //must be called from setup()
   void run();   //must be called from loop()
   long ang();
   void godegrees(int angle, int times);
   void godegreesp(int angle, int times, int kp, int ki, int kd);
   double PIDcalc(double inp, int sp, int kp, int ki, int kd);
   void motgo(int speed, char motnum);
   double gomm(int distance, int times);
   double gommp(int distance, int times, int kp, int ki, int kd);
   int getYAW(); 



  private:
   int _encoder1, _encoder2, _in1R, _in2R, _enaR, _in1L, _in2L, _enaL, _wheel;
   double _kp, _ki, _kd;
   bool integralflag = false;
 };
#endif
