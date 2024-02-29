// _____________Startup Setup Function_____________

void  systemSetup() {
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  
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
  client.setTimeout(20000);

  // begin filesystem
  if (!fs.begin()) {
    return;
  }
  // set filename dayofyear_hourofday_minuteofday.txt
  String filename = "/" + String(timeinfo.tm_yday) + "_" + String(timeinfo.tm_hour) + "_" + String(timeinfo.tm_min) + ".txt" ;

  // set file system to save every log automatically
  LOG_ATTACH_FS_AUTO(fs, filename, FILE_WRITE);

  LOG_INFO("DEBUG LOG FILE  -  Set Log Level in ESP32");

  // set output pin mode
  LOG_TRACE("Set Output Pin Modes");
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

  //turn unused relays off
  LOG_TRACE("Turn off unused relays");
  relay16Off();
  relay15Off();

  // set input pin modes
  LOG_TRACE("Set Input Pin Modes");
  pinMode(ButtonTurn_PIN, INPUT_PULLUP);
  pinMode(Door_PIN, INPUT_PULLUP);
  pinMode(ButtonOpen_PIN, INPUT_PULLUP);

  LOG_TRACE("Set Button Debounce Intervals");
  buttonTurnSwitch.setPushDebounceInterval(debounceTurnButton);
  doorSwitch.setPushDebounceInterval(debounceDoor);
  buttonOpenSwitch.setPushDebounceInterval(debounceOpenButton);

  LOG_INFO("Setup Finished - Ready to use");
}