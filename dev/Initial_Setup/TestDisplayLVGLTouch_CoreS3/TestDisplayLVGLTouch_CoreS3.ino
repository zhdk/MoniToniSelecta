/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <examples/lv_examples.h>
#include <TFT_eSPI.h>
#include <M5CoreS3.h>

#define TFT_HOR_RES   320
#define TFT_VER_RES   240

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
//uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void *draw_buf_1;
unsigned long lastTickMillis = 0;

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  auto t = CoreS3.Touch.getDetail();
  if (t.state != 0) {
    data->point.x = t.x;
    data->point.y = TFT_VER_RES - t.y;
    Serial.printf("state:%d ,x:%d, y:%d\n", t.state, data->point.x, data->point.y);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup()
{
  auto cfg = M5.config();
  CoreS3.begin(cfg);

  CoreS3.begin(cfg);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.begin( 115200 );
  Serial.println( LVGL_Arduino );

  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_init();

  lv_display_t * disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  //lv_obj_t *label = lv_label_create( lv_scr_act() );
  //lv_label_set_text( label, "Hello Arduino, I'm LVGL!" );
  //lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

  lv_example_button_2();

  Serial.println( "Setup done" );
}

void loop()
{
  // LVGL Tick Interface
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();

  CoreS3.update();

  lv_task_handler(); /* let the GUI do its work */
  delay(5); /* let this time pass */
}
