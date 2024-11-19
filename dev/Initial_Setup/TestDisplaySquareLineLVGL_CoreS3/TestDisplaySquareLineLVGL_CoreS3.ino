/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <M5CoreS3.h>
#include <ui.h>

void setup() {
  auto cfg = M5.config();
  CoreS3.begin(cfg);

  Serial.begin(115200);

  ui_setup();

  Serial.println("Setup done");
}

void loop() {
  //lvgl ui ticker
  ui_ticker();

  CoreS3.update();

  //lvgl task handler
  lv_task_handler(); /* let the GUI do its work */
}
