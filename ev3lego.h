#ifndef EV3LEGO_H
#define EV3LEGO_H
 class ev3lego {

  public:
   ev3lego(int encoder1, int encoder2, int encoder3, int encoder4, int in10, int in20, int enA0, int wheel, int in11, int in21, int enB1); // R = 0 , L = 1 , W = 2
   void begin();   //must be called from setup()
   void run();   //must be called from loop()
   long ang();
   void godegrees(int angle, int times);
   void godegreesp(int angle, int times, int kp, int ki, int kd);
   double PIDcalc(double inp, int sp, int kp, int ki, int kd);
   void motgo(int speed, int motnum);
   double gomm(int distance, int times);
   double gommp(int distance, int times, int kp, int ki, int kd);
   int getYAW(); 



  private:
   int _encoder1, _encoder2, _encoder3, _encoder4, _in10, _in20, _enA0, _wheel, _in11, _in21, _enB1;
   double _kp, _ki, _kd;
   bool integralflag = false;
 };
#endif
