void mainLoop() {
  while (WiFi.status() != WL_CONNECTED)
  {
    errorOn();
    delay(500);
    errorOff();
    Serial.print(".");
  }

  //runBLE();
  //updateTelemetry();
  updateInputs();
  vending(vendingState);
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
  if (globalHour == 1 && globalMinute == 46 && millis() >= 20000 ) {
    Serial.println("Daily Reset");
    ESP.restart();
  }
}