#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf20(0x20);
PCF8574 pcf21(0x21);

const byte onboardLed = 13;
const byte PcfButtonLedPin = 0;

unsigned int blinkMillis;

void setup() {
  Serial.begin(115200);
  pcf20.begin();
  //pcf21.begin();
  
  pinMode(onboardLed, OUTPUT);
}

void loop() {
  //static bool state;
  unsigned int currentMillis = millis();
  
  //Lets blink the same output
  if(currentMillis - blinkMillis >= 500){
    //Update time
    blinkMillis = currentMillis;
    
    pcf20.toggle(0);
    //pcf21.toggle(1);
    //Serial.println(pcf20.read8(), BIN);
  }
}