#include <Arduino.h>
#include <SPI.h>
#include "Servo.h"
#include "Ethernet2.h"
#include "EthernetUdp2.h"

Servo serv;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 16, 2);

unsigned int localPort = 8001;      // local port to listen on
char packetBuffer[50];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\n";        // a string to send back
EthernetUDP Udp;  // An EthernetUDP instance to let us send and receive packets over UDP

void setup() {
  Serial.begin(9600);
  
  serv.attach(9);
  serv.write(40);

  Ethernet.init(10);  // CS pin
  Ethernet.begin(mac, ip);
  delay(1000);

  Udp.begin(localPort);

  pinMode(8, INPUT_PULLUP);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(5, INPUT_PULLUP);

  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  pinMode(7, INPUT_PULLUP);

  pinMode(PIN_A0, OUTPUT);
  digitalWrite(PIN_A0, HIGH);
  pinMode(PIN_A1, OUTPUT);
  digitalWrite(PIN_A1, HIGH);
  
  pinMode(PIN_A4, OUTPUT);
  pinMode(PIN_A5, OUTPUT);
}

void loop() {
  static unsigned long  phone_time = 0, phone_time_old = 0;
  static unsigned long  UDP_time = 0, UDP_time_old = 0;
  static uint16_t       phone_timeout_num = 0;
  static String   content = "";
  char ch;
  static uint8_t  com = 0, phone_state = 0, phone_state1 = 0;
  const uint8_t   Phone_numb[] = {0,5,0,1,0,4,0,2,0};
  static uint8_t  phone_num_ind, phone_cnt_rings;
// test
/*
//  if (digitalRead(PIN5)) Serial.write('1');
//  else Serial.write('0');
  digitalWrite(PIN4, LOW);
  delay(100);
  digitalWrite(PIN4, HIGH);
  delay(100);
  while (digitalRead(PIN5) == LOW)
  {
    Serial.println("playing 1");
    delay(500);
  }
  Serial.println("stop");
  delay(1000);

  digitalWrite(PIN6, LOW);
  delay(100);
  digitalWrite(PIN6, HIGH);
  delay(200);
  while (digitalRead(PIN7) == LOW)
  {
    Serial.println("playing 1");
    delay(500);
  }
  Serial.println("stop");
  delay(1000);


  digitalWrite(PIN_A0, LOW);
  delay(1000);
  digitalWrite(PIN_A0, HIGH);
  delay(1000);
  digitalWrite(PIN_A1, LOW);
  delay(1000);
  digitalWrite(PIN_A1, HIGH);
  delay(1000);

  digitalWrite(PIN_A4, HIGH);
  delay(1000);
  digitalWrite(PIN_A4, LOW);
  delay(1000);

  digitalWrite(PIN_A5, HIGH);
  delay(1000);
  digitalWrite(PIN_A5, LOW);
  delay(1000);
*/

  if (Serial.available() > 0)
  {
    ch = Serial.read();
    content.concat(ch);

    if (content.equals("/phone"))
    {
      content = ""; // reset buf
      com = 1;
      phone_state = 1;
      phone_state1 = 0;
      phone_num_ind = 0;
      //Serial.println("is com phone");
      
    }
  }

  phone_time = millis();
  if ((com == 1) && (phone_time > phone_time_old + 100))
  {
    phone_time_old = phone_time;
    
    if (phone_state == 1)
    {
      if (phone_timeout_num++ > 15)
      {
        phone_timeout_num = 0;
        if (phone_num_ind < sizeof(Phone_numb))
        {
          phone_num_ind++;
          if (Phone_numb[phone_num_ind - 1] > 0)
          {
            Serial.println(Phone_numb[phone_num_ind - 1]);
            Serial.println(75 + ((Phone_numb[phone_num_ind - 1] - 1) * 12));
            serv.write(75 + ((Phone_numb[phone_num_ind - 1] - 1)* 12));
          } else
          {
            Serial.println(40);
            serv.write(40);
          }
        } else
        {
          serv.write(40);
          phone_state = 2;
          phone_cnt_rings = 0;
        }
      }
    } else
    if (phone_state == 2)
    {
      if (digitalRead(5) == HIGH)  // если плеер1 не занят
      {
        if (phone_cnt_rings < 5)
        {
          digitalWrite(4, LOW);
          delay(100);
          digitalWrite(4, HIGH);
          delay(100);
          phone_cnt_rings++;
        } else
        {
          // если телефон прозвонил 5 раз, а трубку так и не сняли
          phone_state = 10;
        }
      } else
      {
        // если снимут трубку в то время, когда телефон звонит
        // как остановить звонок - пока не знаю - ждем завершения
        if (digitalRead(8) == LOW)
        {
          while(digitalRead(5) == LOW){};
          Serial.println("trubka");
          phone_state = 3;
        }
      }


      //com = 0;
      //Serial.println("end");
    } else
    if (phone_state == 3)
    {
      if (digitalRead(7) == HIGH) // если плеер2 не занят
      {
        digitalWrite(6, LOW);
        delay(100);
        digitalWrite(6, HIGH);
        delay(100);
      } else
      {
        // голос в трубке
        phone_state = 4;
      }
      
    } else
    if (phone_state == 4)
    {
      if (digitalRead(7) == HIGH)
      {
        // голос в трубке закончился
        phone_state = 10;
      }
      
    } else
    if (phone_state == 10)
    {
      com = 0;
      Serial.println("end");
    }



  }


  UDP_time = millis();
  if (UDP_time > UDP_time_old + 100)
  {
    UDP_time_old = UDP_time;

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }   
      Serial.print(", port ");
      Serial.println(Udp.remotePort());

      // read the packet into packetBufffer
      Udp.read(packetBuffer, 500);
      Serial.println("Contents:");
      Serial.println(packetBuffer);

      // send a reply to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
  }

}