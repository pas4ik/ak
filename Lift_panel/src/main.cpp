#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>

#define N_STEPS_MAX 15  // max steps of scenario
#define ON  1
#define OFF 0

void LED_ctrl(uint8_t n_led, uint8_t act);

PCF8574 pcf20(0x20), pcf21(0x21);

const int scenario[N_STEPS_MAX][1+14+1]=
{// time, actions ("+x" - turn led X on, "-x" - turn led X off), 0-end
  {  1, +12, +14, 0},  // floor 12 & arrow down on
  { 30, -12, +11, 0},  // floor 12 off, 11 on
  { 60, -11, +10, 0},
  { 90, -10,  +9, 0},
  {120,  -9,  +8, 0},
  {150,  -8,  +7, 0},
  {180,  -7,  +6, 0},
  {210,  -6,  +5, 0},
  {240,  -5,  +4, 0},
  {270,  -4,  +3, 0},
  {300,  -3,  +2, 0},
  {330,  -2,  +1, +13, 0},  // floor 1 & arrow up on
  {360, +14, 0},            // arrow down on
  {660,  -1, -13, -14, 0}   // all off
};

void setup() {
  Serial.begin(9600);
  pcf20.begin();
  pcf21.begin();
  
  pinMode(2, INPUT_PULLUP);     // start input
  pinMode(3, INPUT_PULLUP);     // stop input
  pinMode(LED_BUILTIN, OUTPUT); // onboard led
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
  static unsigned long oldMillis;
  static unsigned int but1_ON, but1_old, but1_cnt,
                      but2_ON, but2_old, but2_cnt,
                      scen_time = 0;
  static uint8_t run = 0, st_led_cnt = 0, scen_step;
  
  unsigned int currentMillis = millis(), j;

  if (digitalRead(2) == 0)  // debounce start but
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

  if (digitalRead(3) == 0)  // debounce stop but
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

  if (currentMillis - oldMillis >= 100) // timeout 100 ms
  {
    oldMillis = currentMillis;
    
    if (run == 0)
    {
      // if but start
      if ((but1_ON == 1) && (but2_ON == 0))
      {
        digitalWrite(LED_BUILTIN, HIGH);
        run = 1;
        scen_time = 0;
        scen_step = 0;
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
        scen_step = 0;
        for (j = 1; j <= 16; j++) LED_ctrl(j, 0); // all off
      }
    }
    if (run)
    {
      if (scen_time < 32000) scen_time++;
      
      if (scen_time >= (uint16_t)(scenario[scen_step][0]))
      {
        if (scenario[scen_step][0] == 0) run = 0;
        j = 0;
        while (scenario[scen_step][++j] != 0)
        {
          if (scenario[scen_step][j] > 0) LED_ctrl(scenario[scen_step][j], ON);
          else if (scenario[scen_step][j] < 0) LED_ctrl((uint8_t)(-1*scenario[scen_step][j]), OFF);
        }
        if (run != 0) scen_step++;
      }
      
      if (run == 0)
      {
        for (j = 1; j <= 16; j++) LED_ctrl(j, 0); // all off
      }
    } else
    {
      // onboard led flashing
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
    }
  }
}