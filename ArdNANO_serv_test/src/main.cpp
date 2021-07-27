#include <Arduino.h>
#include "Servo.h"

Servo serv;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  serv.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  serv.write(0);
  delay(2000);
  serv.write(90);
  delay(2000);
  serv.write(180);
  delay(2000);
  serv.write(60);
  delay(2000);
}