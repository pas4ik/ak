#include <Arduino.h>
#include "Servo.h"
#include "SoftwareSerial.h"

#define SERVO_GO    70
#define SERVO_STOP  88
#define START_BYTE  'S'
#define START       1
#define STOP        2

Servo serv;
SoftwareSerial swserial1(2, 3);
uint8_t run = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  swserial1.begin(9600); // interface for radio control
  serv.attach(7);     // servo pin
  swserial1.print("Hello");
}

void loop() {
  int rx_temp = 0, com;
  uint8_t UART_Rx_buf[10], rx_cnt = 0;
//  serv.write(SERVO_GO);
//  delay(1000);
  serv.write(SERVO_STOP);
//  delay(1000);
  
  if (swserial1.available() > 0)
  {
    rx_temp = swserial1.read();
    if (rx_cnt == 0)
    {
      if (rx_temp == START_BYTE)
      UART_Rx_buf[rx_cnt++] = (uint8_t)rx_temp;
      swserial1.print(byte(rx_temp));
    } else
    {
      UART_Rx_buf[rx_cnt++] = (uint8_t)rx_temp;
      swserial1.print(byte(rx_temp));
      if (rx_cnt == 3)
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
  } else
  if (com == STOP)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (run == 1)
  {

  } else
  {
    
  }
  
}