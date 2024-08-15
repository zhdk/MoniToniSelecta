/*
    Description: Control RGB LED to run rainbow light show
    Please install library before compiling:
    FastLED: https://github.com/FastLED/FastLED
*/
#include <M5CoreS3.h>

#include "FastLED.h"

#define Neopixel_PIN 9
#define NUM_LEDS     30

CRGB leds[NUM_LEDS];
uint8_t gHue                              = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle        = 0;

void setup() {
    CoreS3.begin();
    CoreS3.Power.begin();

    CoreS3.Display.clear(BLACK);
    CoreS3.Display.setTextColor(YELLOW);
    CoreS3.Display.setTextSize(2);
    CoreS3.Display.setCursor(40, 0);
    CoreS3.Display.println("Neopixel Example");
    CoreS3.Display.setTextColor(WHITE);
    CoreS3.Display.setCursor(0, 25);
    CoreS3.Display.println("Display rainbow effect");

    // Neopixel initialization
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(10);
    xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2,
                            NULL, 1);
}

void loop() {
}

void FastLEDshowESP32() {
    if (userTaskHandle == 0) {
        userTaskHandle = xTaskGetCurrentTaskHandle();
        xTaskNotifyGive(FastLEDshowTaskHandle);
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200);
        ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        userTaskHandle = 0;
    }
}

void FastLEDshowTask(void *pvParameters) {
    for (;;) {
        fill_rainbow(leds, NUM_LEDS, gHue, 7);  // rainbow effect
        FastLED.show();  // must be executed for neopixel becoming effective
        EVERY_N_MILLISECONDS(20) {
            gHue++;
        }
    }
}
