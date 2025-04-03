/*  
          LEGO EV3 and NXT motor hacked

    A Simple Interface for managing the encoder
      utilizing Arduino's interrupts 
      working with L298N motor driver.

       LEGO cable pinout:
        green   - 5v
        red     - GND
        blue    - encoderin1 
        yellow  - encoderin2
        black/white - motor terminals (connect to a motor driver!)      
*/

#include "ev3lego.h"
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile long degrees = 0;
int encoderPin1, encoderPin2, encoderPin3, encoderPin4;
unsigned long previousTime;
double lastError = 0, cumError = 0;
bool integralflag = false;

ev3lego::ev3lego(int encoder1, int encoder2, int encoder3, int encoder4, int in10, int in20, int enA0, int wheel, int in11, int in21, int enB1) {
    _encoder1 = encoder1;
    _encoder2 = encoder2;
    _encoder3 = encoder3; 
    _encoder4 = encoder4;
    _in10 = in10;
    _in20 = in20;
    _enA0 = enA0;
    _in11 = in11;
    _in21 = in21;
    _enB1 = enB1;
    _wheel = wheel;
}

void countDegrees() {
    if (digitalRead(encoderPin1) == digitalRead(encoderPin2)) {
        degrees++;
    } else {
        degrees--;
    }
}

void ev3lego::begin() {
    Serial.begin(115200);
    Serial.println("\nLego motor initialized!");

    // Debug - print motor setup
Serial.print("in10 = "); Serial.println(_in10);
delay(500);
Serial.print("in20 = "); Serial.println(_in20);
delay(500);
Serial.print("enA0 = "); Serial.println(_enA0);
delay(500);
Serial.print("in11 = "); Serial.println(_in11);
delay(500);
Serial.print("in21 = "); Serial.println(_in21);
delay(500);
Serial.print("enB1 = "); Serial.println(_enB1);
delay(500);


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

    encoderPin1 = _encoder1;
    encoderPin2 = _encoder2;

    degrees = 0;
    attachInterrupt(digitalPinToInterrupt(_encoder1), countDegrees, CHANGE);
    previousTime = millis();

    Serial.println("Setup complete.");
}

void ev3lego::run() {
    static long lastDegrees = 0;
    if (lastDegrees != degrees) {
        lastDegrees = degrees;
    }
}

long ev3lego::ang() {
    return degrees;
}

double ev3lego::PIDcalc(double inp, int sp, int kp, int ki, int kd) {
    unsigned long currentTime = millis();
    double elapsedTime = (currentTime - previousTime) / 1000.0;

    double error = inp - sp;

    if (error * lastError < 0) {
        integralflag = true;
        cumError = 0;
    } else {
        integralflag = false;
    }

    if (!integralflag) {
        cumError += error * elapsedTime;
    }

    if (elapsedTime > 0) {
        double rateError = (error - lastError) / elapsedTime;
        double out = kp * error + ki * cumError + kd * rateError;

        if (out > 254) out = 254;
        if (out < -254) out = -254;

        lastError = error;
        previousTime = currentTime;

        Serial.print("Degrees = "); Serial.println(degrees);
        Serial.print("PID Output = "); Serial.println(out);

        return out;
    }

    return 0;
}

void ev3lego::motgo(int speed, int motnum) {
    switch (motnum) {
        case 0:  // Right Motor
            digitalWrite(_in20, HIGH);
            digitalWrite(_in10, LOW);
            analogWrite(_enA0, abs(speed));
            break;

        case 1:  // Left Motor
            digitalWrite(_in11, HIGH);
            digitalWrite(_in21, LOW);
            analogWrite(_enB1, abs(speed));
            break;

        default:
            Serial.println("Invalid motor selection");
            return;
    }

    if (speed == 0) {
        digitalWrite(_in10, LOW);
        digitalWrite(_in20, LOW);
        digitalWrite(_in11, LOW);
        digitalWrite(_in21, LOW);
        digitalWrite(_enB1, LOW);
    }
}

void ev3lego::godegrees(int angle, int times) {
    for (int i = 0; i < times; i++) { 
        int motspeed = PIDcalc(angle, degrees, 1.5, 1, 0.1);

        if (motspeed > 254) motspeed = 254;
        if (motspeed < -254) motspeed = -254;

        motgo(motspeed, 0);
        motgo(motspeed, 1);

        Serial.print("Motor Speed = ");
        Serial.println(motspeed);
    }

    Serial.print("Final Degrees = ");
    Serial.println(degrees);
}

void ev3lego::godegreesp(int angle, int times, int kp, int ki, int kd) {
    for (int i = 0; i < times; i++) { 
        int motspeed = PIDcalc(angle, degrees, kp, ki, kd);

        if (motspeed > 254) motspeed = 254;
        if (motspeed < -254) motspeed = -254;

        motgo(motspeed, 0);
        motgo(motspeed, 1);
    }
}
