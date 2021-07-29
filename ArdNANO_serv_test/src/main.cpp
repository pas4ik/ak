#include <Arduino.h>
#include "Servo.h"
#include "SoftwareSerial.h"

#define SERVO_GO    73
#define SERVO_STOP  88
#define START_BYTE  'S'
#define START       1
#define STOP        2
#define PIN_GERKON  PIN6
#define TIMEOUT_STOP_WO_GERK  3000 // ms

Servo serv;
SoftwareSerial swserial1(2, 3);
uint8_t run = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // LED
  pinMode(PIN_GERKON, INPUT_PULLUP); // input zero gerkon
  swserial1.begin(9600);    // interface for radio control
  serv.attach(7);           // servo pin
  serv.write(SERVO_STOP);
}

void loop() {
  static int rx_temp = 0, com = 0;
  static uint8_t UART_Rx_buf[10], rx_cnt = 0;
  static unsigned long time_com_stop = 0;
  
  if (swserial1.available() > 0)
  {
    rx_temp = swserial1.read();
    if (rx_cnt == 0)
    {
      if (rx_temp == START_BYTE)
      UART_Rx_buf[rx_cnt++] = (uint8_t)rx_temp;
    } else
    {
      UART_Rx_buf[rx_cnt++] = (uint8_t)rx_temp;
      if (rx_cnt >= 3)
      {
        if ((UART_Rx_buf[0] == START_BYTE) && (UART_Rx_buf[1] == 'T') && (UART_Rx_buf[2] == 'A'))
        {
          com = START;
        } else
        if ((UART_Rx_buf[0] == START_BYTE) && (UART_Rx_buf[1] == 'T') && (UART_Rx_buf[2] == 'O'))
        {
          com = STOP;
        }
        rx_cnt = 0;
      }
    }
  }

  if (com == START)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    serv.write(SERVO_GO);
    run = 1;
    com = 0;
    time_com_stop = 0;
  } else
  if (com == STOP)
  {
    if (time_com_stop == 0) time_com_stop = millis();

    if (digitalRead(PIN_GERKON) == 0)
    {
      digitalWrite(LED_BUILTIN, LOW);
      serv.write(SERVO_STOP);
      run = 0;
      com = 0;
      //swserial1.write('g');
    } else
    if (millis() > time_com_stop + TIMEOUT_STOP_WO_GERK)
    {
      digitalWrite(LED_BUILTIN, LOW);
      serv.write(SERVO_STOP);
      run = 0;
      com = 0;
    }
  }

  if (run == 1)
  {

  } else
  if (run == 0)
  {
    
  }
  
}