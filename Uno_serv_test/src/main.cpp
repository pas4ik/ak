#include <Arduino.h>
#include "Servo.h"

Servo serv;

void setup() {
  Serial.begin(9600);
  serv.attach(9);
  serv.write(40);
}

void loop() {
  
  if (Serial.available() > 0)
  {
    int state = Serial.parseInt();
      
    if ((state >= 1) && (state <= 7))
    {
      serv.write(75 + (state - 1) * 12);
    } else serv.write(40);
    //if (state >= 40 && state <= 150)
    //{
    //  serv.write(state); 
    //}
  }
}