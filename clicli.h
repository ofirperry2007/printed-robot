#ifndef CLICLI_H
#define CLICLI_H

#include "ev3lego.h"

class clicli {
  private:
    ev3lego &mylego;  // הפניה לאובייקט EV3LEGO לניהול המנועים
    int number;

  public: 
    clicli(ev3lego &mylego);  // בנאי: מקבל הפניה לאובייקט ev3lego
    void begin();   // יש לקרוא לה בפונקציה setup()
    void run();     // יש לקרוא לה בפונקציה loop()
};

#endif  
