// _____________Main Loop_____________

void mainLoop() {
  //check wifi connection
  while (WiFi.status() != WL_CONNECTED)
  {
    errorOn();
    delay(500);
    errorOff();
    Serial.print(".");
  }

  //update hardware inputs
  updateInputs();

  //run state machine
  vending(vendingState);

  //update time
  now = time(nullptr);
  gmtime_r(&now, &timeinfo);
  globalHour = timeinfo.tm_hour;
  globalMinute = timeinfo.tm_min;
  /*
  Serial.print("Hour ");
  Serial.println(globalHour);
  Serial.print("Minute ");
  Serial.println(globalMinute);
  */

  //restart esp daily
  if (globalHour == 1 && globalMinute == 45 && millis() >= 20000 ) {
    Serial.println("Daily Reset");
    ESP.restart();
  }
}