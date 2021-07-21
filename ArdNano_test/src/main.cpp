#include <Arduino.h>
#include <OSCBundle.h>
#include <OSCBoards.h>

#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial);
#endif

void LEDcontrol(OSCMessage &msg)
{
    if (msg.isInt(0))
    {
         pinMode(LED_BUILTIN, OUTPUT);
         digitalWrite(LED_BUILTIN, (msg.getInt(0) > 0)? HIGH: LOW);
    }
    else if(msg.isString(0))
    {
         int length=msg.getDataLength(0);
         if(length<5)
         {
           char str[length];
           msg.getString(0,str,length);
           if((strcmp("on", str)==0)|| (strcmp("On",str)==0))
           {
                pinMode(LED_BUILTIN, OUTPUT); 
                digitalWrite(LED_BUILTIN, HIGH);
           }
           else if((strcmp("Of", str)==0)|| (strcmp("off",str)==0))
           {
                pinMode(LED_BUILTIN, OUTPUT); 
                digitalWrite(LED_BUILTIN, LOW);
           }
         }
    }
}

constexpr auto blinkRatesSize = 8;
int blinkRates[blinkRatesSize] = { 2, 5, 10, 12, 15, 17, 20, 22 };

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  SLIPSerial.begin(9600);   // set this as high as you can reliably run on your platform
}

void loop() {
  // put your main code here, to run repeatedly:
 // digitalWrite(LED_BUILTIN, HIGH);
 // delay(500);
 // digitalWrite(LED_BUILTIN, LOW);
 // delay(500);
//  OSCMessage msg("/a");
//  msg.add("bb");
  //msg.add("2");

//  SLIPSerial.beginPacket();  
//    msg.send(SLIPSerial); // send the bytes to the SLIP stream
//  SLIPSerial.endPacket(); // mark the end of the OSC Packet
//  msg.empty(); // free space occupied by message
  //msg.send(Serial);

 // delay(500);

 //the message wants an OSC address as first argument
    OSCMessage msg("/led");

    auto i = rand() % (blinkRatesSize - 1);
    auto blinkRateOn = blinkRates[i];

    auto j = rand() % (blinkRatesSize - 1);
    auto blinkRateOff = blinkRates[j];

    msg.add(blinkRateOn).add(blinkRateOff);

    //SLIPSerial.beginPacket();
    msg.send(SLIPSerial); // send the bytes to the SLIP stream
    //SLIPSerial.endPacket(); // mark the end of the OSC Packet
    //msg.empty(); // free space occupied by message

    delay(500);
}