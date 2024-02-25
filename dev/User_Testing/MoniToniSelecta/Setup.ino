void  systemSetup() {
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();

  // Make sure that the driver chip output is set to LOW when powering on
  /*
  control = new ShiftRegister74HC595_NonTemplate(24, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  assert(control);
  control->setAllLow();
  */
  
  // Connect WIFI
  Serial.print("Connecting to ");
  Serial.println(SSID);
  //WiFi.mode(WIFI_STA);
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
  now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  //struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  String currentTime = asctime(&timeinfo);
  Serial.print(currentTime);
  globalHour = timeinfo.tm_hour;
  globalMinute = timeinfo.tm_min;
  Serial.println(globalMinute);

  // Use WiFiClientSecure class to create TLS connection
  Serial.println("Set Client Insecure");
  client.setInsecure();
  // client.setTimeout(10000);

  //BLESetup("Selecta");  // Don't remove this line!

  Serial.println("Set Pin modes");
  pinMode(Error_PIN, OUTPUT);
  pinMode(Light_CH, OUTPUT);
  pinMode(Lock_CH, OUTPUT);
  pinMode(Sirene_CH, OUTPUT);
  pinMode(Motor_CH, OUTPUT);
  pinMode(Relay15_CH, OUTPUT);
  pinMode(Relay16_CH, OUTPUT);

  pinMode(Item_1_CH, OUTPUT);
  pinMode(Item_2_CH, OUTPUT);
  pinMode(Item_3_CH, OUTPUT);
  pinMode(Item_4_CH, OUTPUT);
  pinMode(Item_5_CH, OUTPUT);
  pinMode(Item_6_CH, OUTPUT);
  pinMode(Item_7_CH, OUTPUT);
  pinMode(Item_8_CH, OUTPUT);
  pinMode(Item_9_CH, OUTPUT);
  pinMode(Item_10_CH, OUTPUT); 
  Serial.println("did outputs"); 

  relay16Off();
  relay15Off();

  pinMode(ButtonTurn_PIN, INPUT_PULLUP);
  Serial.println("Did Turn");
  pinMode(Door_PIN, INPUT_PULLUP);
  Serial.println("Did Door");
  pinMode(ButtonOpen_PIN, INPUT_PULLUP);
  Serial.println("Did Open");
  

  Serial.println("Setup buttons");
  buttonTurnSwitch.setPushDebounceInterval(debounceTurnButton);
  doorSwitch.setPushDebounceInterval(debounceDoor);
  buttonOpenSwitch.setPushDebounceInterval(debounceOpenButton);

  Serial.println("");
  Serial.println("-------------------STARTING------------------");
  Serial.println("");
}