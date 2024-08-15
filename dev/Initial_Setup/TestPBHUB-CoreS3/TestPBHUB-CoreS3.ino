/*
    Description: Use Pbhub to read the analog input value of the slave device,
   or drive multiple sets of RGB LEDs.
*/

#include <Wire.h>
#include <M5CoreS3.h>

#include "porthub.h"

#define X_OFFSET 10
#define Y_OFFSET 18

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
}

void loop() {
    CoreS3.Display.clear(BLACK);
    for (int i = 0; i < 6; i++) {
        CoreS3.Display.setCursor(8 * X_OFFSET, (i * 2) * Y_OFFSET);
        CoreS3.Display.printf("%d:%d", i + 1, porthub.hub_a_read_value(HUB_ADDR[i]));
    }
}