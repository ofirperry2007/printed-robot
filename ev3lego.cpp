/*

          LEGO EV3 and NXT motor hacked

    A Simple Interface for managing the encoder
      utilizing arduino's interrupts 
      working with L298N motor driver.

       lego cable pinout:
        green   - 5v
        red     - GND
        blue    - encoderin1 
        yellow  - encoderin2
        black/white - motor terminals  - connect to a motor driver!        
             
*/
#include "ev3lego.h"
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include "HardwareSerial.h"
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

volatile long degrees;
int encoderPin1;  // Static variables to hold encoder pin values
int encoderPin2;
int encoderPin3;
int encoderPin4;
int pv, error;
unsigned long previousTime;
double lastError;
double cumError, rateError;
unsigned long timer = 0;
bool integralflag = false;  // הגדרת המשתנה integralflag

ev3lego::ev3lego(int encoder1, int encoder2, int encoder3, int encoder4, int in10, int in20, int enA0, int wheel, int in11, int in21, int enB1) {
  _encoder1 = encoder1;
  _encoder2 = encoder2;
  _encoder3 = encoder3; 
  _encoder4 = encoder4;
  _in10 = in10; // R = 0 , L = 1 , W = 2
  _in20 = in20;
  _enA0 = enA0; // speed
  _in11 = in11;
  _in21 = in21;
  _enB1 = enB1;
  _wheel = wheel;
}

void countDegrees() {
  if(digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
    degrees++;
  } else {
    degrees--;
  }
  //Serial.println(degrees);
}

void ev3lego::begin() {
  Serial.begin(115200);
  Serial.println("\nenjoy using lego motor!");
    
 Serial.print("in10 = ");
 Serial.println(_in10);

 Serial.print("in20 = ");
 Serial.println(_in20);

 Serial.print("enA0 = ");
 Serial.println(_enA0);

 Serial.print("in11 = ");
 Serial.println(_in11);

 Serial.print("in21 = ");
 Serial.println(_in21);

 Serial.print("enB1 = ");
 Serial.println(_enB1);

  pinMode(_encoder1, INPUT);
  pinMode(_encoder2, INPUT);
  pinMode(_encoder3, INPUT);
  pinMode(_encoder4, INPUT);
  pinMode(_in10, OUTPUT);
  pinMode(_in20, OUTPUT);
  pinMode(_enA0, OUTPUT);  
  pinMode(_enB1, OUTPUT); 
  pinMode(_in11, OUTPUT);
  pinMode(_in21, OUTPUT); 

  // Assign the static variables to the instance values
  encoderPin1 = _encoder1;
  encoderPin2 = _encoder2;
  encoderPin3 = _encoder3;
  encoderPin4 = _encoder4;
  
  degrees = 0;

  attachInterrupt(digitalPinToInterrupt(_encoder1), countDegrees, CHANGE);
  previousTime = millis();
   Wire.begin();
     byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  if (status != 0) {
    Serial.println("MPU6050 connection failed!");
    return;
  }
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); 
  Serial.println("Done!"); 
/*
  lcd.init();  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("printed robot");
  lcd.setCursor(1, 1);
  lcd.print("Setup finished");
  lcd.setBacklight(0);
  */

 Serial.println("Setup finished");
} 

void ev3lego::run() {
  static long deg = 0;
  if(deg != degrees){
    deg = degrees;
  }
      mpu.update();
}

long ev3lego::ang() {
  return degrees;
}

double ev3lego::PIDcalc(double inp, int sp, int kp, int ki, int kd){
  unsigned long currentTime = millis();  // Get current time
  double elapsedTime = (currentTime - previousTime) / 1000.0;  // Compute elapsed time in seconds

  // Ensure elapsed time is computed as a floating-point number
  //Serial.print("Elapsed time = ");
  //Serial.println(elapsedTime, 6);  // Debugging to print elapsed time
  //Serial.print("I = ");
  //Serial.println(cumError);
  double error = inp - sp;  // Determine error
  // Detect if the error has changed directionprintl
  if (error * lastError < 0) {  // If error changes sign (direction), reset integral
    integralflag = true;  // Set flag to indicate error has changed direction
    cumError = 0;         // Reset cumulative error when direction changes
    Serial.println("Error changed direction, resetting integral accumulator.");
  } else {
    integralflag = false;  // Continue accumulating integral error
  }
  if (!integralflag) {
    cumError += error * elapsedTime;  // Compute integral (accumulate error over time)
  }

  if (elapsedTime > 0) {  // Compute derivative deltaError/deltaTime, avoid dividing by 0
    double rateError = (error - lastError) / elapsedTime;

    // PID output
    double out = kp * error + ki * cumError + kd * rateError;

    // Update for next iteration
    lastError = error;              // Remember current error
    previousTime = currentTime;     // Remember current time

    // Limit the output for smoother operation
    if (out > 254) { out = 254; }
    if (out < -254) { out = -254; }
    Serial.print("degrees = ");  Serial.println(degrees);
    Serial.print("out value = ");  Serial.println(out);
    return out;  // Return the PID output value
  }

  return 0;  // Return 0 if elapsedTime is zero
}

void ev3lego::motgo(int speed, int motnum) {
    /*Serial.print("Speed = ");
    Serial.println(speed);
    Serial.print("Motnum = ");
    Serial.println(motnum); */

    switch (motnum) {
        case 0:  //  Right Motor (R)
           // Serial.println("Right Motor (R) Selected");
            digitalWrite(_in20, HIGH);
            digitalWrite(_in10, LOW);
            analogWrite(_enA0, abs(speed));
            break;

        case 1:  //  Left Motor (L)
            //Serial.println("Left Motor (L) Selected");
            digitalWrite(_in11, HIGH);
            digitalWrite(_in21, LOW);
            analogWrite(_enB1, abs(speed));
            break;

      /*case 2:  //  NEW: Winch Motor (W)
            Serial.println("Winch Motor (W) Selected");
            digitalWrite(_enB1, HIGH);  // Enable winch motor
            digitalWrite(_enB1, LOW);  // Enable winch motor
            analogWrite(_wheel, abs(speed));  // Apply speed to the winch
            break;*/

        default:
            Serial.println("Invalid motor selection");
            return;
    }

    // Stop motors if speed = 0
    if (speed == 0) {
        digitalWrite(_in10, LOW);
        digitalWrite(_in20, LOW);
        digitalWrite(_in11, LOW);
        digitalWrite(_in21, LOW);
        digitalWrite(_enB1, LOW);  // Stop winch
    }
}

void ev3lego::godegrees(int angle, int times){ //output: -254<x<+254
  for(int i = 0; i < times; i++){ 
    int motspeed = PIDcalc(angle, degrees,1.5, 1, 0.1);//sp, pv. pv is the global variable degrees
    if(motspeed > 254){motspeed = 254;}
    if(motspeed < -254){motspeed = -254;}
    motgo(motspeed, 0);
    motgo(motspeed, 1);
    Serial.print("motspeed = ");
    Serial.println(motspeed);
    
  }
    Serial.print("degrees = ");
    Serial.println(degrees);
}

void ev3lego::godegreesp(int angle, int times, int kp, int ki, int kd){
  for(int i = 0; i < times; i++){ 
      int motspeed = PIDcalc(angle, degrees, kp, ki, kd);//sp, pv. pv is the global variable degrees
      if(motspeed > 254){motspeed = 254;}
      if(motspeed < -254){motspeed = -254;}
      motgo(motspeed, 0);
      motgo(motspeed, 1);
    }
}
/*
double ev3lego::gomm(int distance, int times){
  int deg = (distance / (_wheel * PI)) * 360;
  godegrees(deg, times);
  int distcovered = (degrees * PI);
  return distcovered; 
}

double ev3lego::gommp(int distance, int times, int kp, int ki, int kd){
  int deg = (distance / (_wheel * PI)) * 360;
  godegreesp(deg, times, kp, ki, kd);
  int distcovered = (degrees * PI);
  return distcovered;
  }

int ev3lego::getYAW(){
  return int(mpu.getAngleZ());
} 

int ev3lego::robotgo(int speedR, int speedL){
 
}

int ev3lego::gotoang(int sp){
  for (int counter = 0; counter < 1000; ++counter) {
   int pv = getYAW();
   error = (sp - pv);



   }
} */
