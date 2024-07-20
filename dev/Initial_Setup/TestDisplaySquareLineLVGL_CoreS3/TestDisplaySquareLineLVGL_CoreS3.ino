/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <M5CoreS3.h>
#include <ui.h>

#define TFT_HOR_RES   240
#define TFT_VER_RES   320

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
//uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void *draw_buf_1;
unsigned long lastTickMillis = 0;
static int button_state_spin = 0;
static int button_state_open = 0;

//IMPLEMENT ALL STATES
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  auto t = CoreS3.Touch.getDetail();
  if (t.state != 0) {
    data->point.x = TFT_HOR_RES - t.y;
    data->point.y = t.x;
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

  Serial.begin( 115200 );

  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_init();

  lv_display_t * disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);

  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();

  lv_obj_add_event_cb(ui_ButtonSpin, onSpinButtonPressed, LV_EVENT_PRESSED, NULL);
  //lv_obj_add_event_cb(ui_ButtonOpen, onOpenButtonPressed, LV_EVENT_PRESSED, NULL);

  lv_screen_load(ui_StartUpScreen);
  lv_obj_add_state(ui_StartUpPanel, LV_STATE_CHECKED);
  lv_obj_remove_flag(ui_SetupLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_StartUpErrorLabel, LV_OBJ_FLAG_HIDDEN);

  lv_screen_load(ui_MainScreen);

  /*
  lv_screen_load(ui_ValidationScreen);
  lv_obj_remove_flag(ui_ValidationStatusLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_AccessDeniedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  serverRequest_Animation(ui_ValidationStatusPanel, 0);
  */

  //lv_screen_load(ui_EndScreen);
  

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

  Serial.println(button_state_spin);
  Serial.println(button_state_open);
}
