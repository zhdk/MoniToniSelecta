#include "modbus_crc.h"
#include "M5CoreS3.h"

unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
unsigned int   checksumModbus;
unsigned char relayNumber,sendByteModbus;

void setup()  
{
  auto cfg = M5.config();
  CoreS3.begin(cfg);
  int textsize = CoreS3.Display.height() / 60;
  if (textsize == 0) {
      textsize = 1;
  }
  CoreS3.Display.setTextSize(textsize);


  Serial.begin(9600);
  Serial.println("*** Modbus RTU Relay Test Program ***\r\n");

  Serial2.begin(9600, SERIAL_8N1, 18, 17);
  //pinMode(RS485_E,OUTPUT);
  //digitalWrite(RS485_E,HIGH);   //send

}

void loop() // run over and over
{ 
  for(relayNumber=0;relayNumber<32; relayNumber++){
    CoreS3.Display.fillScreen(RED);
    cmdModbus[2] = 0;
    cmdModbus[3] = relayNumber;
    cmdModbus[4] = 0xFF;
    cmdModbus[5] = 0;
    checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
    cmdModbus[6] = checksumModbus & 0xFF;
    cmdModbus[7] = checksumModbus >> 8;
    for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
      Serial2.write(cmdModbus[sendByteModbus]);
    }
    Serial.print(relayNumber);
    Serial.println(" on");
    CoreS3.Display.fillScreen(WHITE);
    delay(1000);
  }
  
  for(relayNumber=0;relayNumber<32;relayNumber++){
    CoreS3.Display.fillScreen(BLUE);
    cmdModbus[2] = 0;
    cmdModbus[3] = relayNumber;
    cmdModbus[4] = 0;
    cmdModbus[5] = 0;
    checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
    cmdModbus[6] = checksumModbus & 0xFF;
    cmdModbus[7] = checksumModbus >> 8;
    for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
      Serial2.write(cmdModbus[sendByteModbus]);
    }
    Serial.print(relayNumber);
    Serial.println(" off");
    CoreS3.Display.fillScreen(WHITE);
    delay(1000);
  }


}
