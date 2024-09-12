/*
 * @Hardwares: M5CoreS3
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5CoreS3: https://github.com/m5stack/M5CoreS3
 */

#include "M5CoreS3.h"
#include <Adafruit_NeoPixel.h>

#define PIN       9  //定义NeoPixel的控制引脚
#define NUMPIXELS 20   //定义NeoPixel控制灯灯数量

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(
    NUMPIXELS, PIN,
    NEO_GRB + NEO_KHZ800);  // set number of LEDs, pin number, LED type.
                          


void setup() {
    auto cfg = M5.config();
    CoreS3.begin(cfg);
    int textsize = CoreS3.Display.height() / 60;
    if (textsize == 0) {
        textsize = 1;
    }
    CoreS3.Display.setTextSize(textsize);
        pixels.begin();    // Init the NeoPixel library.
}

int i = 0, j = 1, k = 2;

void loop() {
    pixels.setPixelColor(i++, pixels.Color(100, 0, 0));  // Bright red
    pixels.setPixelColor(j++, pixels.Color(0, 100, 0));  // Bright green
    pixels.setPixelColor(k++, pixels.Color(0, 0, 100));  // Bright blue
    pixels.show();  // sends the updated color to the hardware.

    delay(100);
    if (i == NUMPIXELS)
        i = 0;
    else if (j == NUMPIXELS)
        j = 0;
    else if (k == NUMPIXELS)
        k = 0;
}