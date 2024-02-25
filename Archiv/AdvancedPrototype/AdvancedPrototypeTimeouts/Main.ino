void mainLoop() {
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  updateInputs();
  vending(vendingState);
}