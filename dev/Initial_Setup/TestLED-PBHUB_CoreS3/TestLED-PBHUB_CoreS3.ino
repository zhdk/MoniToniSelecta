/*
    Description: Use Pbhub to read the analog input value of the slave device,
   or drive multiple sets of RGB LEDs.
*/

#define NUMPIXELS 288

#include <Wire.h>
#include <M5CoreS3.h>

#include "porthub.h"

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
    for (int i = 0; i < 6; i++) {
        porthub.hub_wire_setBrightness(HUB_ADDR[i], 1);
        porthub.hub_wire_length(HUB_ADDR[i], NUMPIXELS);
        porthub.hub_wire_fill_color(HUB_ADDR[i], 0, NUMPIXELS, 250, 250, 250);
    }
    delay(1000);
}