/**
 * @file digital_write_read.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief
 * @version 0.1
 * @date 2024-03-05
 *
 *
 * @Hardwares: M5Core + PbHub / PbHub v1.1
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5UnitPbHub: https://github.com/m5stack/M5Unit-PbHub
 */

#include "M5UnitPbHub.h"
#include "M5CoreS3.h"

M5UnitPbHub pbhub;

void setup() {
    auto cfg = M5.config();
    CoreS3.begin(cfg);
    CoreS3.Display.setTextColor(GREEN);
    CoreS3.Display.setTextDatum(middle_center);
    CoreS3.Display.setFont(&fonts::Orbitron_Light_24);
    CoreS3.Display.setTextSize(1);
    CoreS3.Display.drawString("Ltr553 Init Success", CoreS3.Display.width() / 2,
                              CoreS3.Display.height() / 2);
    // Serial.begin(115200);
    if (!pbhub.begin(&Wire, UNIT_PBHUB_I2C_ADDR, 2, 1, 400000U)) {
        Serial.println("Couldn't find Pbhub");
        while (1) delay(1);
    }
}

void loop() {
  CoreS3.Display.clear();
  String v = String(pbhub.digitalRead(0, 0));

  CoreS3.Display.drawString(v, CoreS3.Display.width() / 2,
                              CoreS3.Display.height() / 2);


}
