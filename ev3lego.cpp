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

volatile long degrees;
static int encoderPin1;  // Static variables to hold encoder pin values
static int encoderPin2;
unsigned long previousTime;
double lastError;
double cumError, rateError;

ev3lego::ev3lego(int encoder1, int encoder2, int in1, int in2, int ena, int wheel) {
  _encoder1 = encoder1;
  _encoder2 = encoder2;
  _in1 = in1;
  _in2 = in2;
  _ena = ena;
  _wheel = wheel;
}

void countDegrees() {
  if(digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
    degrees++;
  } else {
    degrees--;
  }
  Serial.println(degrees);
}

void ev3lego::begin() {
  Serial.begin(115200);
  Serial.println("\nenjoy using lego motor!");
  
  pinMode(_encoder1, INPUT);
  pinMode(_encoder2, INPUT);
  pinMode(_in1, OUTPUT);
  pinMode(_in2, OUTPUT);
  pinMode(_ena, OUTPUT);  
  // Assign the static variables to the instance values
  encoderPin1 = _encoder1;
  encoderPin2 = _encoder2;
  
  degrees = 0;
  
  attachInterrupt(digitalPinToInterrupt(_encoder1), countDegrees, CHANGE);
  previousTime = millis();
  lcd.init();  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("printed robot");
  lcd.setCursor(1, 1);
  lcd.print("Setup finished");
  //lcd.setBacklight(0);
  Serial.println("Setup finished");
}


void ev3lego::run() {
  static long deg = 0;
  if(deg != degrees){
    deg = degrees;
  }
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
  // Detect if the error has changed direction
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


void ev3lego::motgo(int speed){
  if(speed > 0){
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
  } else if (speed < 0){
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
  } else {
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, HIGH);
  }
  analogWrite(_ena, abs(speed));
}

void ev3lego::godegrees(int angle, int times){ //output: -254<x<+254
  for(int i = 0; i < times; i++){ 
    int motspeed = PIDcalc(angle, degrees, 1, 1, 0);//sp, pv. pv is the global variable degrees
    if(motspeed > 254){motspeed = 254;}
    if(motspeed < -254){motspeed = -254;}
    motgo(motspeed);
  }
}

void ev3lego::godegreesp(int angle, int times, int kp, int ki, int kd){
  for(int i = 0; i < times; i++){ 
      int motspeed = PIDcalc(angle, degrees, kp, ki, kd);//sp, pv. pv is the global variable degrees
      if(motspeed > 254){motspeed = 254;}
      if(motspeed < -254){motspeed = -254;}
      motgo(motspeed);
    }
}

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
