/***************************************
 cv++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++cv +     Lego cable pinout:
        green   - 5v
        red     - GND
        blue    - encoderin1
        yellow  - encoderin2
        black/white - motor terminals
        
notes: 
  - conenct encoder1 to pin 2 or 3 (IRQ pin)
  - clicli is a usefull tool to test each of the commands in ev3lego
  try clicking in the Serial terminal: v 200
  v is activating motgo method at the argument speed
  customize the clicli.cpp file to get much more!
   R = 0 , L = 1 , W = 2
***************************************/
#include "ev3lego.h"
#include "clicli.h"

ev3lego roboPaint(
    2,  // encoder1(yellow), must be either pin 2 or 3
    A4, // encoder 0 (blue)
    3, // encoder3(yellow)
    A5, // encoder4(blue)
    8,  // in10 (L298N) direction
    7,  // in20 direction
    9, // enA0 speed
    12,  // in11
    13,  // in21
    11,  // enB1
    4,   // in12
    5,   // in22
    6,   // enc2
    65); // wheel size  

clicli mycli(roboPaint);

void setup() {
 Serial.begin(115200);
delay(5000);
Serial.println("CLI is LIVE");

 roboPaint.begin();
  mycli.begin();

  delay(5000);
  roboPaint.goSquare();

  
}


 void loop() {
  roboPaint.run();
  mycli.run(); 
  
  
}

  //mymotor.godegreesp(1440, 2000, 1, 1, 0);
  //Serial.println(mymotor.gommp(PI*65, 2000, 1, 1, 0));
  
  
