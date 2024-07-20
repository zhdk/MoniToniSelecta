static void onSpinButtonPressed(lv_event_t *event) {
  //lv_obj_t *button = lv_event_get_target(event);
  //lv_state_t buttonState = button->state;
  //button_state_spin = lv_obj_has_state(button, LV_STATE_PRESSED);
  Serial.println(lv_event_get_target(event));
}
/*
static void onOpenButtonPressed(lv_event_t *event) {
  // lv_obj_t *button = lv_event_get_target(event);
  // lv_state_t buttonState = button->state;
  // button_state_open = lv_obj_has_state(button, LV_STATE_PRESSED);
  Serial.println(event->target);
}
*/