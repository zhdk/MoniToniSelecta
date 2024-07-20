/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include <examples/lv_examples.h>
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
static lv_style_t style_bg;
static lv_style_t style;
static lv_style_t style_pr;


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


void ui_lv_button_style(void)
{
  /*Init the style for the default state*/
  lv_style_init(&style);

  lv_style_set_radius(&style, 16);

  lv_style_set_bg_opa(&style, LV_OPA_100);
  lv_style_set_bg_color(&style, lv_color_hex(0xffffff));
  lv_style_set_bg_grad_color(&style, lv_color_hex(0x9b9b9b));
  lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

  lv_style_set_border_opa(&style, LV_OPA_40);
  lv_style_set_border_width(&style, 0);
  lv_style_set_border_color(&style, lv_color_hex(0xd0021b));

  lv_style_set_shadow_width(&style, 0);
  lv_style_set_shadow_color(&style, lv_color_hex(0xd0021b));
  lv_style_set_shadow_offset_y(&style, 8);

  lv_style_set_outline_opa(&style, LV_OPA_COVER);
  lv_style_set_outline_color(&style, lv_color_hex(0xffffff));

  lv_style_set_text_color(&style, lv_color_hex(0x000000));
  lv_style_set_pad_all(&style, 10);
  //lv_style_set_size(&style, lv_pct(80), lv_pct(40));
  lv_style_set_text_font(&style, &lv_font_montserrat_48);

  /*Init the pressed style*/
  lv_style_init(&style_pr);

  /*Add a large outline when pressed*/
  lv_style_set_outline_width(&style_pr, 30);
  lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

  lv_style_set_translate_y(&style_pr, 5);
  lv_style_set_shadow_offset_y(&style_pr, 3);
  lv_style_set_bg_color(&style_pr, lv_color_hex(0xb80000));
  lv_style_set_bg_grad_color(&style_pr, lv_color_hex(0xb80000));

  /*Add a transition to the outline*/
  static lv_style_transition_dsc_t trans;
  static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
  lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

  lv_style_set_transition(&style_pr, &trans);
}

void ui_lv_button() {
  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_remove_style_all(btn1);                          /*Remove the style coming from the theme*/
  lv_obj_add_style(btn1, &style, 0);
  lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
  lv_obj_center(btn1);

  lv_obj_t * label1 = lv_label_create(btn1);
  lv_label_set_text(label1, "Turn");
  lv_obj_center(label1);


  lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  lv_obj_remove_style_all(btn2);                          /*Remove the style coming from the theme*/
  lv_obj_add_style(btn2, &style, 0);
  lv_obj_add_style(btn2, &style_pr, LV_STATE_PRESSED);
  lv_obj_center(btn2);

  lv_obj_t * label2 = lv_label_create(btn2);
  lv_label_set_text(label2, "Open");
  lv_obj_center(label2);
}


void ui_lv_button_col(void) {
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_hex(0x000000));

  static lv_obj_t * cont_col = lv_obj_create(lv_screen_active());
  lv_obj_add_style(cont_col, &style_bg, 0);
  lv_obj_remove_flag(cont_col, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_size(cont_col, lv_pct(100), lv_pct(100));
  lv_obj_align(cont_col, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(cont_col, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER);


  lv_obj_t * btn1 = lv_button_create(cont_col);
  lv_obj_remove_style_all(btn1);
  lv_obj_add_style(btn1, &style, 0);
  lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
  lv_obj_set_size(btn1, LV_PCT(100), LV_PCT(45));

  lv_obj_t * label1 = lv_label_create(btn1);
  lv_label_set_text(label1, "Spin");
  lv_obj_center(label1);

  
  lv_obj_t * btn2 = lv_button_create(cont_col);
  lv_obj_remove_style_all(btn2);
  lv_obj_add_style(btn2, &style, 0);
  lv_obj_add_style(btn2, &style_pr, LV_STATE_PRESSED);
  lv_obj_set_size(btn2, LV_PCT(100), LV_PCT(45));
  

  lv_obj_t * label2 = lv_label_create(btn2);
  lv_label_set_text(label2, "Open");
  lv_obj_center(label2);
}
void setup()
{
  auto cfg = M5.config();
  CoreS3.begin(cfg);

  //CoreS3.begin(cfg);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.begin( 115200 );
  Serial.println( LVGL_Arduino );

  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_init();

  lv_display_t * disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);

  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  //lv_example_button_2();
  //ui_lv_button_style();
  //ui_lv_button_col();
  //ui_lv_button();

  ui_init();

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
