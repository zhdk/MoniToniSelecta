/*
    Description: Use Pbhub to read the analog input value of the slave device,
   or drive multiple sets of RGB LEDs.
*/

#define NUMPIXELS 174

// LED Hub Port
#define CONNECTEDHUBPORT 0

// LED MAPPING
#define LED_LEVEL_1_START 0
#define LED_LEVEL_1_END 15
#define LED_LEVEL_2_START 16
#define LED_LEVEL_2_END 33
#define LED_LEVEL_3_START 34
#define LED_LEVEL_3_END 50
#define LED_LEVEL_4_START 51
#define LED_LEVEL_4_END 67
#define LED_LEVEL_5_START 68
#define LED_LEVEL_5_END 84
#define LED_LEVEL_6_START 85
#define LED_LEVEL_6_END 101
#define LED_LEVEL_7_START 102
#define LED_LEVEL_7_END 118
#define LED_LEVEL_8_START 119
#define LED_LEVEL_8_END 135
#define LED_LEVEL_9_START 136
#define LED_LEVEL_9_END 152
#define LED_LEVEL_10_START 153
#define LED_LEVEL_10_END 173

#include <Wire.h>
#include <M5CoreS3.h>

#include "porthub.h"

const int LEDMAPPING_LEVELS[10][2] = {
  {LED_LEVEL_1_START, LED_LEVEL_1_END},
  {LED_LEVEL_2_START, LED_LEVEL_2_END},
  {LED_LEVEL_3_START, LED_LEVEL_3_END},
  {LED_LEVEL_4_START, LED_LEVEL_4_END},
  {LED_LEVEL_5_START, LED_LEVEL_5_END},
  {LED_LEVEL_6_START, LED_LEVEL_6_END},
  {LED_LEVEL_7_START, LED_LEVEL_7_END},
  {LED_LEVEL_8_START, LED_LEVEL_8_END},
  {LED_LEVEL_9_START, LED_LEVEL_9_END},
  {LED_LEVEL_10_START, LED_LEVEL_10_END}
};

PortHub porthub;
uint8_t HUB_ADDR[6] = {HUB1_ADDR, HUB2_ADDR, HUB3_ADDR,
                       HUB4_ADDR, HUB5_ADDR, HUB6_ADDR};

void setup() {

    auto cfg = M5.config();
    CoreS3.begin(cfg);
    porthub.begin();
    CoreS3.Display.clear(BLACK);
    CoreS3.Display.setTextColor(WHITE);
    CoreS3.Display.setTextSize(4);
    CoreS3.Display.printf("LEDSTRIP TEST");
}

void loop() {
    porthub.hub_wire_setBrightness(HUB_ADDR[CONNECTEDHUBPORT], 1);
    porthub.hub_wire_length(HUB_ADDR[CONNECTEDHUBPORT], NUMPIXELS);
    for (int j = 0; j < 10; j++) {
        porthub.hub_wire_fill_color(HUB_ADDR[CONNECTEDHUBPORT], LEDMAPPING_LEVELS[j][0], LEDMAPPING_LEVELS[j][1] - LEDMAPPING_LEVELS[j][0] + 1, 250, 250, 250);
        delay(1000);
    }
    delay(5000);
    porthub.hub_wire_fill_color(HUB_ADDR[CONNECTEDHUBPORT], 0, NUMPIXELS, 0, 0, 0);
    delay(2000);
}