#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

#define N_STEPS_MAX 15

void LED_ctrl(uint8_t n_led, uint8_t act);

PCF8574 pcf20(0x20), pcf21(0x21);

const int scenario[N_STEPS_MAX][1+14+1]=
{// time, action ("+x" - turn led X on, "-x" - turn led X off)
  { 10, +1, 0},
  { 20, +2, -1,0},
  { 30, +3, -2,0},
  { 40, +4, -3,0},
  { 50, +5, -4,0},
  { 60, +6, -5,0},
  { 70, +7, -6,0},
  { 80, +8, -7,0},
  { 90, +9, -8,0},
  {100,+10, -9,0},
  {110,+11,0}
};

void setup() {
  Serial.begin(9600);
  pcf20.begin();
  pcf21.begin();
  
  pinMode(7, INPUT_PULLUP); // start input
  pinMode(8, INPUT_PULLUP); // stop input
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("start");
  Serial.print(scenario[1][0]);
  Serial.print(' ');
  Serial.print(scenario[1][1]);
  Serial.print(' ');
  Serial.print(scenario[1][2]);

  LED_ctrl(1, HIGH);
  _delay_ms(1000);
  LED_ctrl(1,LOW);
  //pcf20.write(0, );
  _delay_ms(1000);
  LED_ctrl(2,HIGH);
}

void LED_ctrl(uint8_t n_led, uint8_t act)
{
  if ((n_led >= 1) && (n_led <= 8))
  {
    pcf20.write(n_led-1, act?0:1);
  } else
  if ((n_led >= 9) && (n_led <= 16))
  {
    pcf21.write(n_led-9, act?0:1);
  }
}

void loop() {
  static unsigned long blinkMillis;
  static unsigned int but1_ON, but1_old, but1_cnt,
                      but2_ON, but2_old, but2_cnt,
                      scen_time = 0;
  static uint8_t run = 0, st_led_cnt = 0, scen_step;
  
  unsigned int currentMillis = millis(), i, j;

  if (digitalRead(7) == 0)
  {
      if (but1_old == 1)
      {
          if (but1_cnt < 2000) but1_cnt++;
          if (but1_cnt > 500) but1_ON = 1;
      } else but1_old = 1;
  } else
  {
    but1_old = 0;
    but1_cnt = 0;
    but1_ON = 0;
  }

  if (digitalRead(8) == 0)
  {
      if (but2_old == 1)
      {
          if (but2_cnt < 2000) but2_cnt++;
          if (but2_cnt > 500) but2_ON = 1;
      } else but2_old = 1;
  } else
  {
    but2_old = 0;
    but2_cnt = 0;
    but2_ON = 0;
  }

  if (currentMillis - blinkMillis >= 100)
  {
    blinkMillis = currentMillis;
    
    if (run == 0)
    {
      // if but start
      if ((but1_ON == 1) && (but2_ON == 0))
      {
        digitalWrite(LED_BUILTIN, HIGH);
        run = 1;
        scen_time = 0;
      } else
      // led flashing
      if (digitalRead(LED_BUILTIN) == 0)
      {
        if (++st_led_cnt >= 9)
        {
          st_led_cnt = 0;
          digitalWrite(LED_BUILTIN, HIGH);
        }
      } else
      {
        if (++st_led_cnt >= 1)
        {
          st_led_cnt = 0;
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
    } else
    if (run == 1)
    {
      // if but stop
      if ((but1_ON == 0) && (but2_ON == 1))
      {
        digitalWrite(LED_BUILTIN, LOW);
        run = 0;
        scen_time = 0;
        for (j = 1; j <= 16; j++) LED_ctrl(j, 0); // all off
      }
    }
    if (run)
    {
      if (scen_time < 32000) scen_time++;
      
      if (scen_time >= (uint16_t)(scenario[scen_step][0]))
      {
        if (scenario[scen_step][0] == 0) run = 0;
        Serial.print(scenario[scen_step][0]);
        Serial.print(' ');
        j = 0;
        while (scenario[scen_step][++j] != 0)
        {
          if (scenario[scen_step][j] > 0) LED_ctrl(scenario[scen_step][j], 1);
          else if (scenario[scen_step][j] < 0) LED_ctrl((uint8_t)(-1*scenario[scen_step][j]), 0);
          Serial.print(scenario[scen_step][j]);
          Serial.print(' ');
        }
        Serial.println("");
      }
      
      if (run == 0)
      {
        for (j = 1; j <= 16; j++) LED_ctrl(j, 0); // all off
      } else scen_step++;
    } else
    {

    }
  }
}