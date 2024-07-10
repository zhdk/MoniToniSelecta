#include "modbus_crc.h"
#include "M5CoreS3.h"

unsigned char  cmd[8] = {0x01,0x05,0,0,0,0,0,0}; 
unsigned int   crc;
unsigned char i,j;

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
  for(i=0;i<32;i++){
    CoreS3.Display.fillScreen(RED);
    cmd[2] = 0;
    cmd[3] = i;
    cmd[4] = 0xFF;
    cmd[5] = 0;
    crc = ModbusCRC((unsigned char  *)cmd,6);
    cmd[6] = crc & 0xFF;
    cmd[7] = crc >> 8;
    for(j=0;j<8;j++){
      Serial2.write(cmd[j]);
    }
    Serial.print(i);
    Serial.println(" on");
    CoreS3.Display.fillScreen(WHITE);
    delay(100);
  }
  
  for(i=0;i<32;i++){
    CoreS3.Display.fillScreen(BLUE);
    cmd[2] = 0;
    cmd[3] = i;
    cmd[4] = 0;
    cmd[5] = 0;
    crc = ModbusCRC((unsigned char  *)cmd,6);
    cmd[6] = crc & 0xFF;
    cmd[7] = crc >> 8;
    for(j=0;j<8;j++){
      Serial2.write(cmd[j]);
    }
    Serial.print(i);
    Serial.println(" off");
    CoreS3.Display.fillScreen(WHITE);
    delay(100);
  }


}
