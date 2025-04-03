#ifndef EV3LEGO_H
#define EV3LEGO_H

class ev3lego {
  public:
    ev3lego(int encoder1, int encoder2, int encoder3, int encoder4, int in10, int in20, int enA0, int wheel, int in11, int in21, int enB1); 
    void begin();   // חייב לרוץ ב-setup()
    void run();     // חייב לרוץ ב-loop()
    long ang();     // מחזיר את זווית האנקודר
    void godegrees(int angle, int times);
    void godegreesp(int angle, int times, int kp, int ki, int kd);
    double PIDcalc(double inp, int sp, int kp, int ki, int kd);
    void motgo(int speed, int motnum);

  private:
    int _encoder1, _encoder2, _encoder3, _encoder4; 
    int _in10, _in20, _enA0, _wheel, _in11, _in21, _enB1;
    bool integralflag = false;
};

#endif
