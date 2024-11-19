#include <Adafruit_NeoPixel.h>
#include "M5CoreS3.h"
#define PIN 9
#define LED_COUNT 172
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  auto cfg = M5.config();
    CoreS3.begin(cfg);
  strip.begin();
  strip.setBrightness(50);

  strip.show(); // Initialize all pixels to 'off'
   CoreS3.Display.setTextColor(GREEN);
    CoreS3.Display.setTextDatum(middle_center);
    CoreS3.Display.setFont(&fonts::Orbitron_Light_24);
    CoreS3.Display.setTextSize(1);
    CoreS3.Display.drawString("Ltr553 Init Fail",
                                  CoreS3.Display.width() / 2,
                                  CoreS3.Display.height() / 2);
}

void loop() {
  colorSet(strip.Color(0, 255, 0), 0); // Red
  delay(1000);

  colorSet(strip.Color(255, 0, 0), 0); // Green
  delay(1000);

  colorSet(strip.Color(0, 0, 255), 0); // Blue
  delay(1000);

  colorSet(strip.Color(255, 255, 255), 0); // WHITE
  delay(2000);

  colorSet(strip.Color(127, 127, 127), 0); // half WHITE
  delay(2000);
}

void colorSet(uint32_t c, uint8_t wait) { // From NeoPixel Library
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
   strip.show();
   delay(wait);
}