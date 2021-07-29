#include <Arduino.h>
#include "Servo.h"

Servo serv;

void setup() {
  Serial.begin(9600);
  serv.attach(9);
  serv.write(145);
}

void loop() {
  
  if (Serial.available() > 0)
  {
    int state = Serial.parseInt();
      
    if ((state >= 1) && (state <= 7))
    {
      serv.write(145 - (state) * 17);
    } else serv.write(145);
    //if (state >= 20 && state <= 160)
    //{
    //  serv.write(state); 
    //}
  }
}