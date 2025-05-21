#ifndef CLICLI_H
#define CLICLI_H
#include "ev3lego.h"

 class clicli {
  private:
  ev3lego &mylego;//(ಥ﹏ಥ)
  int number;//(´;︵;`) ༼☯﹏☯༽
  
  public: 
   clicli(ev3lego &mylego);
   void begin();   //must be called from  void setup() --- > ( ´•̥̥̥o•̥̥̥`)(˃̣̣̥╭╮˂̣̣̥)‧º·˚
   void run();   //must be called from  void loop() --- >  (´;︵;`) (T⌓T) 

 };
#endif 
