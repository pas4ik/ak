#include <Arduino.h>
#include "OSCBoards.h"
#include "OSCBundle.h"
#include <SPI.h>
#include "EthernetENC.h"

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 16, 2);

unsigned int localPort = 8001;      // local port to listen on
char packetBuffer[500];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\n";        // a string to send back
EthernetUDP Udp;  // An EthernetUDP instance to let us send and receive packets over UDP

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // pin 13 LED - PB5/SCK used in SPI! so no control
  Serial.begin(9600);
  Ethernet.init(10);  // CS pin
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    delay(1000);
    if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    }
  }

  Udp.begin(localPort);
  Serial.print("Listen on port: ");
  Serial.println(localPort, DEC);
}

void loop() {
  static unsigned char LED_st = 0;
  static unsigned long LED_time = 0, LED_time_old = 0;
  static unsigned long UDP_time = 0, UDP_time_old = 0;

  LED_time = millis();
  if (LED_time > LED_time_old + 500)
  {
    LED_time_old = LED_time;
    if (LED_st == 0)
    {
      LED_st = 1;
      //digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED is on");
    } else
    {
      LED_st = 0;
      //digitalWrite(LED_BUILTIN, LOW);
      Serial.println("LED if off");
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