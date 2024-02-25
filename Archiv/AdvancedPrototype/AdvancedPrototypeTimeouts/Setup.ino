void  systemSetup() {
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();

  // Make sure that the driver chip output is set to LOW when powering on
  control = new ShiftRegister74HC595_NonTemplate(24, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  assert(control);
  control->setAllLow();

  // Connect WIFI
  Serial.print("Connecting to ");
  Serial.println(SSID);
  // WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // Use WiFiClientSecure class to create TLS connection
  client.setInsecure();

  // client.setTimeout(10000);

  pinMode(ButtonTurn_PIN, INPUT_PULLUP);
  pinMode(Door_PIN, INPUT_PULLUP);
  pinMode(ButtonOpen_PIN, INPUT_PULLUP);

  pinMode(Error_GND_PIN, OUTPUT);
  pinMode(Error_PIN, OUTPUT);
  

  buttonTurnSwitch.setPushDebounceInterval(debounceTurnButton);
  doorSwitch.setPushDebounceInterval(debounceDoor);
  buttonOpenSwitch.setPushDebounceInterval(debounceOpenButton);

  Serial.println("");
  Serial.println("-------------------STARTING------------------");
  Serial.println("");
}