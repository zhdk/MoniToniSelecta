#include "M5CoreS3.h"

void setup()  
{
  auto cfg = M5.config();
  CoreS3.begin(cfg);

  pinMode(8, INPUT_PULLUP);
}

void loop() // run over and over
{ 
  for(i=0;i<1000;i++){  
    Serial.println(digitalRead(8));
    delay(10);
  }
}
