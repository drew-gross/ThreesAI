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
  
  udMotor->setSpeed(50);
  lrMotor->setSpeed(50);
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
        delay(500);
        Serial.println(buttonPresser.read());
        buttonPresser.write(180);
        Serial.println(buttonPresser.read());
    }
  }
}