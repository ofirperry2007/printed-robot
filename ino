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

ev3lego roboPaint(3, //encoder1(blue). must be either pin 2 or 3
                4, //encoder2(yellow)
                5,//blue- motorB     encoder3
                2,//yellow- motorB   encoder4
                6, //in10 (L298N)direction
                7, //in20 direction
                10, //enA0 speed
                65, //wheel size
                8, //in11
                9, //in21
                11); //enB1
clicli mycli(roboPaint);

void setup() {
  roboPaint.begin();
  mycli.begin();
  roboPaint.godegrees(90, 1000);
  roboPaint.motgo(0, 0); 
  //roboPaint.motgo(100, 2); //  Winch motor (W) 
}


void loop() {
  roboPaint.run();//must have it in the background to update motor position 
  //Serial.println(mymotor.getYAW());
  mycli.run();
  //mymotor.godegreesp(1440, 2000, 1, 1, 0);
  //Serial.println(mymotor.gommp(PI*65, 2000, 1, 1, 0));
  
}
