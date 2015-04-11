/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *udMotor = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *lrMotor = AFMS.getStepper(200, 2);

Servo buttonPresser;

void setup() {
  Serial.begin(9600);

  AFMS.begin();
  
  udMotor->setSpeed(5000);
  lrMotor->setSpeed(5000);
  buttonPresser.attach(9);
}

void loop() {
  if (Serial.available()) {
    byte instruction = Serial.read();
    switch (instruction) {
      case 'u':
        udMotor->step(200, FORWARD, DOUBLE);
        break;
      case 'd':
        udMotor->step(200, BACKWARD, DOUBLE);
        break;
      case 'l':  
        lrMotor->step(200, FORWARD, DOUBLE); 
        break;
      case 'r':
        lrMotor->step(200, BACKWARD, DOUBLE);
        break;
      case 'b':
        Serial.println(buttonPresser.read());
        buttonPresser.write(0);
        Serial.println(buttonPresser.read());
        delay(2000);
        Serial.println(buttonPresser.read());
        buttonPresser.write(180);
        Serial.println(buttonPresser.read());
    }
  }
}