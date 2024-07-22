#include "M5CoreS3.h"
#include <M5Unified.h>

// Define the GPIO pin for the button
#define BUTTON_PIN 8

// m5::Button customButton(BUTTON_PIN);

void setup()  
{
  auto cfg = M5.config();
  CoreS3.begin(cfg);

  pinMode(8, INPUT_PULLUP);

}

void loop() // run over and over
{ 
  for(int i=0;i<1000;i++){  
    Serial.println(digitalRead(8));
    delay(10);
  }
  // // Update the button state
  //   CoreS3.update();
  //   // Update the button state
  //   customButton.read();

  //   // Check for button click
  //   if (customButton.wasPressed()) {
  //       Serial.println("Button was pressed!");
  //   }

  //   // Check for button long press
  //   if (customButton.pressedFor(1000)) { // 1000 milliseconds = 1 second
  //       Serial.println("Button was long pressed!");
  //   }

   
}
