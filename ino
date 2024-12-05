
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
***************************************/
#include "ev3lego.h"
#include "clicli.h"



ev3lego mymotor(3, //encoder1(blue). must be either pin 2 or 3
                4, //encoder2(yellow)
                6, //in1 (L298N)direction
                7, //in2 direction
                10, //enA speed
                65); //wheel size
clicli mycli(mymotor);

void setup() {
  mymotor.begin();
  mycli.begin();
}

void loop() {
  mymotor.run();//must have it in the background to update motor position 
  Serial.println(mymotor.getYAW());
  mycli.run();
  //mymotor.godegreesp(1440, 2000, 1, 1, 0);
  //Serial.println(mymotor.gommp(PI*65, 2000, 1, 1, 0));
  
}
