// _____________MoniToni Vending Machine Integration_____________
// Hardware Iteration: M5CoreS3 Microcontroller & Waveshare 32CH Modbus RTU Relay Board



// _____________definitions_____________
// Debugger
// define DEBUGLOG_ENABLE_FILE_LOGGER to enable file logger
#define DEBUGLOG_ENABLE_FILE_LOGGER

// set log level NONE, ERROR, WARN, INFO, DEBUG, TRACE
#define DEBUGLOG_DEFAULT_LOG_LEVEL_DEBUG

// set log level for file output NONE, ERROR, WARN, INFO, DEBUG, TRACE
#define DEBUGLOG_DEFAULT_FILE_LEVEL_TRACE

// define debug preamble
// default preamble with file & codeline information about debug source
// #define LOG_PREAMBLE LOG_SHORT_FILENAME, LOG_MACRO_APPEND_STR(L.__LINE__), __func__, ":"

// no preamble
#define LOG_PREAMBLE ""

// Uncommenting DEBUGLOG_DISABLE_LOG disables ASSERT and all log (Release Mode)
// PRINT and PRINTLN are always valid even in Release Mode
// #define DEBUGLOG_DISABLE_LOG


// SD Card
#define SD_SPI_SCK_PIN  36
#define SD_SPI_MISO_PIN 35
#define SD_SPI_MOSI_PIN 37
#define SD_SPI_CS_PIN   4

// SERIAL2 PINS M5CoreS3
#define RX_PIN_SERIAL2 18
#define TX_PIN_SERIAL2 17

// PINS IN
#define Door_PIN 8   //GPIO 8 - M5CoreS3
// #define ButtonTurn_PIN 48 
// #define ButtonOpen_PIN 21 

// RELAY CONFIG
#define Item_1_CH 1  //Relay 1
#define Item_2_CH 2  //Relay 2
#define Item_3_CH 3  //Relay 3
#define Item_4_CH 4  //Relay 4
#define Item_5_CH 5  //Relay 5
#define Item_6_CH 6  //Relay 6
#define Item_7_CH 7  //Relay 7
#define Item_8_CH 8  //Relay 8 
#define Item_9_CH 9  //Relay 9
#define Item_10_CH 10  //Relay 10
#define Light_CH 11  //Relay 11
#define Lock_CH 12  //Relay 12
#define Sirene_CH 13  //Relay 13
#define Motor_CH 14  //Relay 14
#define Relay15_CH 15  //Relay 15
#define Relay16_CH 16  //Relay 16
// #define Error_PIN 47 //Yellow


// Delays (in microseconds)
// #define CarrouselDELAY 100
#define CarrouselDELAY 0
#define UserInputDELAY 5000
#define SleepDELAY 120000
#define debounceTurnButton 50
#define debounceOpenButton 1
#define debounceDoor 100
#define openDoorDELAY 500
#define PurchaseTimeoutDELAY 60000
#define DoorOpenSireneDELAY 10000
#define ServerTimeout 3000
// #define MotorTurnDELAY 2500
#define MotorTurnDELAY 0
#define BluetoothSetupDelay 15000

// Proximity Sensor
#define ProximityThreshold 3

// Speaker Volume [0 - 255]
#define SpeakerVolume 50
// Speaker Frequency [in Hz]
#define SpeakerFrequency 1000

// #define BLINKS 30 //Error LED Blinks

// Graphics
// Screen Resolution
#define TFT_HOR_RES   240
#define TFT_VER_RES   320

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
//uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#define READINGDELAY 3000


// _____________Library Imports_____________

// M5CoreS3.h Libary by Tinyu-Zhao,M5Stack [1.0.0]
#include <M5CoreS3.h>

// Timer.h by Stefan Staub [1.2.1]
#include <Timer.h>

// ArduinoJson.h by Benoit Blanchon [7.0.3]
#include <ArduinoJson.h>

// SPI.h by Arduino [2.1.1] (Arduino IDE Version)
#include <SPI.h>

// WiFiClientSecure.h by Espressif Systems [2.0.11] (esp32 Boards Version)
#include <WiFiClientSecure.h>

// debounce.h by Aaron Kimball [0.2.0]
#include <debounce.h>

// SD.h by Arduino, SparkFun [1.2.4]
#include <SD.h>

// DebugLog.h by hideakitai [0.8.3]
#include <DebugLog.h>

// Modbus Checksum Calculation
#include "modbus_crc.h"

// lvgl UI library [CUSTOM CONFIG & MODIFIED TFT_eSPI DRIVER]
#include <lvgl.h> 

// TFT display library [CUSTOM CONFIG]
#include <TFT_eSPI.h>

// UI Files [CUSTOM UI Files]
#include <ui.h>

// Credentials File
#include "credentials.h"




// _____________variables_____________

// Logic Variables
volatile int vendingState = 1; // 0 = Sleep, 1 = Idle, 2 = Turn, 3 = Validate, 4 = Collect, 5 = Finished, 6 = Error

volatile int activeScreen = 0; //  0 = StartUpScreen, 1 = MainScreen, 2 = ValidationScreen, 3 = EndScreen, 4 = SleepScreen, 5 = ErrorScreen

volatile int item;

volatile bool doorOpenState = false;
volatile bool buttonTurnPushedState = false;
volatile bool buttonOpenPushedState = false;
volatile bool carrouselUnlockedState = false;
volatile bool motorOnState = false;
volatile bool itemUnlockedState = false;
volatile bool sireneOnState = false;
volatile bool lightOnState = false;
volatile bool errorOnState = false;
volatile bool transactionActive = false;
volatile bool requestActive = false;
volatile bool proximityTriggered = false;
volatile bool displayOn = false;

// Network Variables
const char *SSID = WIFI_SSID;        // Change this to your WiFi SSID
const char *PW = WIFI_PASS; // Change this to your WiFi password


const String host = "monitoni.zhdk.ch";
const int port = 443;

String url_permission = "/api/vending/permission";
String url_complete = "/api/vending/complete";
String url_close = "/api/vending/close";
String monitoni_terminal = TOKEN;

volatile bool permission = false;
volatile bool completed = false;
volatile bool closed = false;

volatile int globalHour;
volatile int globalMinute;
struct tm timeinfo;
time_t now;

void *draw_buf_1;                     // Draw Buffer for Graphics
unsigned long lastTickMillis = 0;   // Tick Variable for
lv_display_t * disp;                //display instance

uint16_t read_ps_value;   // Porximity Sensor Value


// unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
// unsigned int   checksumModbus;
// unsigned char relayNumber,sendByteModbus;


// _____________instances_____________

WiFiClientSecure client;

Timer timerButtonTurnPress;
Timer timerButtonOpenPress;
Timer timerSleep;
Timer timerPurchaseTimeout;
Timer timerServerTimeout;
Timer timerDoorOpen;

Ltr5xx_Init_Basic_Para device_init_base_para = LTR5XX_BASE_PARA_CONFIG_DEFAULT;

// static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
// static Button doorSwitch(1, doorSwitchHandler);
// static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);


// _____________handlerfunctions based on systemsetup in main_____________

// static void buttonTurnSwitchHandler(uint8_t btnId, uint8_t btnState) {
//   if (btnState == BTN_PRESSED) {
//     LOG_TRACE("LOGIC: buttonTurnPushedState set to true");
//     buttonTurnPushedState = true;
//     LOG_TRACE("TIMER: Button Turn Press stopped");
//     timerButtonTurnPress.stop();
//     return;
//   }

//   // btnState == BTN_OPEN.
//   LOG_TRACE("LOGIC: buttonTurnPushedState set to false");
//   buttonTurnPushedState = false;
//   LOG_TRACE("TIMER: Button Turn Press started");
//   timerButtonTurnPress.stop();
//   timerButtonTurnPress.start();
// }

static void doorSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_OPEN) {
    LOG_TRACE("LOGIC: doorOpenState set to true");
    doorOpenState = true;
    timerDoorOpen.stop();
    return;
  }

  // btnState == BTN_PRESSED.
  LOG_TRACE("LOGIC: doorOpenState set to false");
  doorOpenState = false;
  LOG_TRACE("TIMER: Door Open Timer started");
  timerDoorOpen.stop();
  timerDoorOpen.start();
}

// static void buttonOpenSwitchHandler(uint8_t btnId, uint8_t btnState) {
//   if (btnState == BTN_PRESSED) {
//     LOG_TRACE("LOGIC: buttonOpenPushedState set to true");
//     buttonOpenPushedState = true;
//     LOG_TRACE("TIMER: Button Open Press Timer stopped");
//     timerButtonOpenPress.stop();
//     return;
//   }

//   // btnState == BTN_OPEN.
//   LOG_TRACE("LOGIC: buttonOpenPushedState set to false");
//   buttonOpenPushedState = false;
//   LOG_TRACE("TIMER: Button Open Press Timer started");
//   timerButtonOpenPress.stop();
//   timerButtonOpenPress.start();
// }

// static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
static Button doorSwitch(1, doorSwitchHandler);
// static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);



// _____________Startup Setup Function_____________

void  systemSetup() {
  // Initialize M5Core S3
  CoreS3.begin();

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();

  //UI & Display Initialization
  ui_setup();

  // SD Card Initialization
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

  if (!SD.begin(SD_SPI_CS_PIN, SPI, 40000000)) {
    // Print a message if the SD card initialization fails orif the SD card does not exist.
    Serial.println("Card failed, or not present");
  }

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();

  //Initialized Serial port for RS485 Communication
  Serial2.begin(9600, SERIAL_8N1, RX_PIN_SERIAL2, TX_PIN_SERIAL2);
  
  // Connect WIFI
  Serial.print("Connecting to ");
  Serial.println(SSID);
  // WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    lv_label_set_text(ui_SetupLabel, "Trying to connect to WiFi");
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
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
  Serial.println(currentTime);
  globalHour = timeinfo.tm_hour;
  globalMinute = timeinfo.tm_min;


  // Use WiFiClientSecure class to create TLS connection
  Serial.println("Set Client Insecure");
  client.setInsecure();
  client.setTimeout(20000);


  // set filename dayofyear_hourofday_minuteofday.txt
  String filename = "/" + String(timeinfo.tm_yday) + "_" + String(timeinfo.tm_hour) + "_" + String(timeinfo.tm_min) + ".txt" ;
  // String filename = "/test.txt" ;

  

  // set file system to save every log automatically
  LOG_ATTACH_FS_AUTO(SD, filename, FILE_WRITE);

  LOG_INFO("DEBUG LOG FILE  -  Set Log Level in ESP32");


  LOG_TRACE("Setup Proximity Sensor");
  device_init_base_para.ps_led_pulse_freq   = LTR5XX_LED_PULSE_FREQ_40KHZ;
  device_init_base_para.ps_measurement_rate = LTR5XX_PS_MEASUREMENT_RATE_50MS;
  // active ps mode -- standby -> LTR5XX_PS_STAND_BY_MODE
  CoreS3.Ltr553.setPsMode(LTR5XX_PS_ACTIVE_MODE);

  LOG_TRACE("Setup Speaker");
  auto spk_cfg = CoreS3.Speaker.config();
  if (spk_cfg.use_dac || spk_cfg.buzzer) {
    /// Increasing the sample_rate will improve the sound quality
    /// instead of increasing the CPU load.
    spk_cfg.sample_rate =
      192000;  // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 ,
                // 96000 , 100000 , 128000 , 144000 , 192000 , 200000
  }
  CoreS3.Speaker.config(spk_cfg);
  CoreS3.Speaker.begin();
  if (!CoreS3.Speaker.isEnabled()) {
    LOG_TRACE("Speaker not found");  
  }
  CoreS3.Speaker.setVolume(SpeakerVolume);
  CoreS3.Speaker.setAllChannelVolume(255);

  //turn unused relays off
  LOG_TRACE("Turn off unused relays");
  sendModbusClose(15);
  sendModbusClose(16);
  // relay16Off();
  // relay15Off();

  // set input pin modes
  LOG_TRACE("Set Input Pin Modes");
  // pinMode(ButtonTurn_PIN, INPUT_PULLUP);
  pinMode(Door_PIN, INPUT_PULLUP);
  // pinMode(ButtonOpen_PIN, INPUT_PULLUP);


  LOG_TRACE("Set Button Debounce Intervals");
  // buttonTurnSwitch.setPushDebounceInterval(debounceTurnButton);
  doorSwitch.setPushDebounceInterval(debounceDoor);
  // buttonOpenSwitch.setPushDebounceInterval(debounceOpenButton);

  LOG_INFO("Setup Finished - Ready to use");

  // Setup Finished -- play sequence
  //play 2000Hz tone sound, 100 msec.
  CoreS3.Speaker.tone(2000, 200);
  CoreS3.delay(200);
  /// play 1000Hz tone sound, 100 msec.
  CoreS3.Speaker.tone(1000, 200);
  CoreS3.delay(200);
  //play 2000Hz tone sound, 100 msec.
  CoreS3.Speaker.tone(2000, 200);
  CoreS3.delay(200);
  /// play 1000Hz tone sound, 100 msec.
  CoreS3.Speaker.tone(1000, 200);
  CoreS3.delay(200);
  // Wait for the output to finish.
  while (CoreS3.Speaker.isPlaying()) {
    CoreS3.delay(1);
  }
  // stop output sound.
  CoreS3.Speaker.stop();
}



// _____________MODBUS Functions_____________

void sendModbusOpen(int relayNumber) {
  unsigned char relayNumberAddress = relayNumber - 1;
  unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
  unsigned int   checksumModbus;
  unsigned char sendByteModbus;

  cmdModbus[2] = 0;
  cmdModbus[3] = relayNumberAddress;
  cmdModbus[4] = 0xFF;
  cmdModbus[5] = 0;
  checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
  cmdModbus[6] = checksumModbus & 0xFF;
  cmdModbus[7] = checksumModbus >> 8;
  for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
    Serial2.write(cmdModbus[sendByteModbus]);
  }
  delay(100);
}

void sendModbusClose(int relayNumber) {
  unsigned char relayNumberAddress = relayNumber - 1;
  unsigned char  cmdModbus[8] = {0x01,0x05,0,0,0,0,0,0}; 
  unsigned int   checksumModbus;
  unsigned char sendByteModbus;

  cmdModbus[2] = 0;
  cmdModbus[3] = relayNumberAddress;
  cmdModbus[4] = 0;
  cmdModbus[5] = 0;
  checksumModbus = ModbusCRC((unsigned char  *)cmdModbus,6);
  cmdModbus[6] = checksumModbus & 0xFF;
  cmdModbus[7] = checksumModbus >> 8;
  for(sendByteModbus=0;sendByteModbus<8;sendByteModbus++){
    Serial2.write(cmdModbus[sendByteModbus]);
  }
  delay(100);
}

// select command to receive by setting argument cmd. 0 = receive address, 1 = receive baudrate, 
void receiveModbus(int cmd) {

}


// _____________Hardware Functions_____________

void carrouselLock()
{
  sendModbusClose(Lock_CH);
  LOG_TRACE("LOGIC: carrouselUnlockedState set to false");
  carrouselUnlockedState = false;
}

void carrouselUnlock()
{
  sendModbusOpen(Lock_CH);
  LOG_TRACE("HARDWARE: CarrouselDELAY");
  delay(CarrouselDELAY);
  LOG_TRACE("LOGIC: carrouselUnlockedState set to true");
  carrouselUnlockedState = true;
}


void motorOn()
{
  sendModbusOpen(Motor_CH);
  LOG_TRACE("LOGIC: motorOnState set to true");
  motorOnState = true;
}

void motorOff()
{
  sendModbusClose(Motor_CH);
  LOG_TRACE("LOGIC: motorOnState set to false");
  motorOnState = false;
}


void lightOn()
{
  sendModbusOpen(Light_CH);
  LOG_TRACE("LOGIC: lightOnState set to true");
  lightOnState = true;
}

void lightOff()
{
  sendModbusClose(Light_CH);
  LOG_TRACE("LOGIC: lightOnState set to false");
  lightOnState = false;
}


void sireneOn()
{
  CoreS3.Speaker.tone(SpeakerFrequency);
  LOG_TRACE("LOGIC: sireneOnState set to true");
  sireneOnState = true;
}

void sireneOff()
{
  CoreS3.Speaker.stop();
  LOG_TRACE("LOGIC: sireneOnState set to false");
  sireneOnState = false;
}


void itemUnlock(int item)
{
  LOG_TRACE("LOGIC: itemUnlockedState set to true");
  itemUnlockedState = true;
  LOG_TRACE("HARDWARE: Solenoid ,", item, " triggered");
  sendModbusOpen(item);
}

void itemLock(int item)
{
  LOG_TRACE("LOGIC: itemUnlockedState set to false");
  itemUnlockedState = false;
  LOG_TRACE("HARDWARE: Solenoid ,", item, " released");
  sendModbusClose(item);
}



/////////////////////////////////////Based on system Setup included in main file
// static Button buttonTurnSwitch(0, buttonTurnSwitchHandler);
// static Button doorSwitch(1, doorSwitchHandler);
// static Button buttonOpenSwitch(1, buttonOpenSwitchHandler);


static void updateInputs() {
  // update proximity sensor
  read_ps_value  = CoreS3.Ltr553.getPsValue();
  if (read_ps_value > ProximityThreshold) {
    proximityTriggered = true;
  }
  else {
    proximityTriggered = false;
  }

  // update() will call buttonHandler() if PIN transitions to a new state and stays there
  // for multiple reads over 25+ ms.
  if (vendingState == 4 || vendingState == 5) {
    doorSwitch.update(digitalRead(Door_PIN));
    return;
  }
  // buttonTurnSwitch.update(digitalRead(ButtonTurn_PIN));
  // buttonOpenSwitch.update(digitalRead(ButtonOpen_PIN));
}



// _____________MoniToni-Server HTTP Requests Functions_____________

bool permissionRequest()
{ 
  if (!requestActive) {
    //timerServerTimeout.stop();
    //timerServerTimeout.start();
  }
  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    // Serial.println("Connection failed");
    LOG_ERROR("ERROR: Connection failed - permission request");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_permission);
  client.println(String("GET " + url_permission + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    // Serial.println(String("Failed to send request"));
    client.stop();
    LOG_ERROR("ERROR: Failed to send request");
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0)
  {
    // Serial.print(String("Unexpected response: "));
    LOG_ERROR("Status: ", status);
    client.stop();
    LOG_ERROR("ERROR: Unexpected response");
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(String("Invalid response"));
    client.stop();
    return 0;
    LOG_ERROR("ERROR: Invalid response");
  }

  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error)
  {
    Serial.print(String("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    LOG_ERROR("ERROR: Deserialization failed");
    return 0;
  }

  // Extract Values
  permission = doc["HasPermission"].as<bool>();

  if (permission == true)
  {
    transactionActive = true;
    timerServerTimeout.stop(); 
    //timerPurchaseTimeout.stop();
    //timerPurchaseTimeout.start();
    Serial.println(F("Item:"));
    Serial.println(doc["Item"].as<int>());
    item = doc["Item"].as<int>();
    // Disconnect
    client.stop();
    return 1;
  }
  else {
    transactionActive = false;
    //timerPurchaseTimeout.stop();
    // Disconnect
    client.stop();
    LOG_INFO("INFO: No Permission");
    return 0;
  }
}

///////////////////////------ complete request ------///////////////////////

bool completeRequest()
{
  transactionActive = false;
  permission = false;

  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    // Serial.println("Connection failed");
    LOG_ERROR("ERROR: Connection failed - complete request");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_complete);
  client.println(String("GET " + url_complete + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    // Serial.println(String("Failed to send request"));
    LOG_ERROR("ERROR: Failed to send complete request");
    client.stop();
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  /*
  Serial.println("");
  Serial.println("Request Completed");
  Serial.println("Received this from Server: ");
  Serial.println(status);
  Serial.println("");
  Serial.println("");
  */
  if (strcmp(status, "HTTP/1.1 201 Created") != 0)
  {
    // Serial.print(String("Unexpected response: "));
    // Serial.println(status);
    LOG_ERROR("ERROR: Unexpected response");
    LOG_ERROR("ERROR: Status received: ", status);
    client.stop();
    // Serial.println("UNSUCCESSFUL Request");
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    // Serial.println(String("Invalid response"));
    // Serial.println("UNSUCCESSFUL Request");
    LOG_ERROR("ERROR: Invalid response for complete request");
    client.stop();
    return 0;
  }

  // Disconnect
  client.stop();

  Serial.println("Successful Request");
  LOG_TRACE("SUCCESS: Complete Request");

  return 1;
}



///////////////////////------ close request ------///////////////////////

bool closeRequest()
{
  transactionActive = false;
  permission = false;

  // Serial.print("Connecting to ");
  // Serial.println(host);
  if (!client.connect("monitoni.zhdk.ch", 443))
  {
    // Serial.println("Connection failed");
    LOG_ERROR("ERROR: Connection failed - close request");
    return 0;
  }

  // Send HTTP request
  // Serial.print("Requesting URL: ");
  // Serial.println(url_close);
  client.println(String("GET " + url_close + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0)
  {
    // Serial.println(String("Failed to send request"));
    LOG_ERROR("ERROR: Failed to send close request");
    client.stop();
    return 0;
  }
  else
  {
    // Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  /*
  Serial.println("");
  Serial.println("Request Completed");
  Serial.println("Received this from Server: ");
  Serial.println(status);
  Serial.println("");
  Serial.println("");
  */
  if (strcmp(status, "HTTP/1.1 201 Created") != 0)
  {
    // Serial.print(String("Unexpected response: "));
    LOG_ERROR("ERROR: Received Status: ", status);
    LOG_ERROR("ERROR: Unexpected response for close request");
    client.stop();
    // Serial.println("UNSUCCESSFUL Request");
    return 0;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    // Serial.println(String("Invalid response"));
    // Serial.println("UNSUCCESSFUL Request");
    LOG_ERROR("ERROR: Invalid response for close request");
    client.stop();
    return 0;
  }

  // Disconnect
  client.stop();

  // Serial.println("Successful Request");
  LOG_TRACE("SUCCESS: Close Request");

  return 1;
}



// _____________ui setup_____________

void ui_setup() {
  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  lv_init();

  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);

  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();

  lv_obj_add_event_cb(ui_ButtonSpin, onSpinButtonPressed, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(ui_ButtonSpin, onSpinButtonReleased, LV_EVENT_RELEASED, NULL);
  lv_obj_add_event_cb(ui_ButtonOpen, onOpenButtonPressed, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(ui_ButtonOpen, onOpenButtonReleased, LV_EVENT_RELEASED, NULL);

  lv_screen_load(ui_StartUpScreen);
  lv_obj_add_state(ui_StartUpPanel, LV_STATE_CHECKED);
  lv_obj_remove_flag(ui_SetupLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_StartUpErrorLabel, LV_OBJ_FLAG_HIDDEN);

  // lv_screen_load(ui_MainScreen);
  //
  /*
  lv_screen_load(ui_ValidationScreen);
  lv_obj_remove_flag(ui_ValidationStatusLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_AccessDeniedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  serverRequest_Animation(ui_ValidationStatusPanel, 0);
  */

  //lv_screen_load(ui_EndScreen);

  // ui updates
  //lvgl ui ticker
  ui_ticker();
  //lvgl task handler
  lv_task_handler(); /* let the GUI do its work */
}


// Touch Input Device Callback
//IMPLEMENT ALL STATES
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  auto t = CoreS3.Touch.getDetail();
  if (t.state != 0) {
    data->point.x = TFT_HOR_RES - t.y;
    data->point.y = t.x;
    // Serial.printf("state:%d ,x:%d, y:%d\n", t.state, data->point.x, data->point.y);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// LVGL Display Interface turn off function
void turnOffDisplay() {
  // switch to blank sleep screen
  displayOn = false;
}
// LVGL Display Interface turn on function
void turnOnDisplay() {
  // switch to main/idle screen
  displayOn = true;
}

// LVGL Tick Interface
void ui_ticker () {
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();
}

// _____________ui callbacks_____________

static void onSpinButtonPressed(lv_event_t *event) {
  // Serial.println("Spin Button Pressed");
  buttonTurnPushedState = true;
  // if (vendingState == 1) {
  //   vendingState = 2; // idle --> turn
  // }
}

static void onSpinButtonReleased(lv_event_t *event) {
  // Serial.println("Spin Button Released");
  buttonTurnPushedState = false;
  // vendingState = 1; // turn --> idle
}

static void onOpenButtonPressed(lv_event_t *event) {
  // Serial.println("Open Button Pressed");
  //SET STATE OF MACHINE
  // if (vendingState == 1) {
  //   buttonOpenPushedState = true;
  // //   vendingState = 3; // idle --> validate
  // }
  // else {
  //   buttonOpenPushedState = false;
  // }
}

static void onOpenButtonReleased(lv_event_t *event) {
  //Switch Machine State only once button is released
  if (vendingState == 1) {
    buttonOpenPushedState = true;
  //   vendingState = 3; // idle --> validate
  }
  else {
    buttonOpenPushedState = false;
  }
}







// _____________State Logic_____________


void vendingSleep() {
  //_GUI SCREEN OFF
  if (timerSleep.state() == RUNNING){
    LOG_TRACE("HARDWARE: Stop Sleep Timer");
    timerSleep.stop();
  }

  if (displayOn) {
    LOG_TRACE("HARDWARE: Turn Off Display");
    turnOffDisplay();
  }
  if (carrouselUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Carrousel");
    carrouselLock();
  }
  if (motorOnState) {
    LOG_TRACE("HARDWARE: Turn Off Motor");
    motorOff();
  }
  if (lightOnState) {
    LOG_TRACE("HARDWARE: Turn Off Light");
    lightOff();
  }
  if (sireneOnState) {
    LOG_TRACE("HARDWARE: Turn Off Sirene");
    sireneOff();
  }
  if (itemUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Items");
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }

  if (proximityTriggered) {
    LOG_TRACE("LOGIC: Proximity Sensor triggered");
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 1; // Idle
    LOG_INFO("STATE: Switching to Idle State");
    return;
  }
}


void vendingIdle() {
  if (timerSleep.read() > SleepDELAY) {
    LOG_TRACE("LOGIC: Sleep Timer greater than SleepDELAY");
    timerSleep.stop();
    LOG_DEBUG("TIMER: Sleep Timer stopped");
    vendingState = 0; // Sleep
    LOG_INFO("STATE: Switching to Sleep State");
    return;
  }


  if (!displayOn) {
    LOG_TRACE("HARDWARE: Turn On Display");
    turnOnDisplay();
  }
  if (activeScreen != 1) {
    lv_screen_load(ui_MainScreen);
    activeScreen = 1;
  }


  if (carrouselUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Carrousel");
    carrouselLock();
  }
  if (motorOnState) {
    LOG_TRACE("HARDWARE: Turn Off Motor");
    motorOff();
  }
  if (!lightOnState) {
    LOG_TRACE("HARDWARE: Turn On Light");
    lightOn();
  }
  if (sireneOnState) {
    LOG_TRACE("HARDWARE: Turn Off Sirene");
    sireneOff();
  }
  if (itemUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Items");
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }
  

  if (buttonTurnPushedState) {
    LOG_TRACE("LOGIC: Turn Button Pushed");
    vendingState = 2; // Turn
    LOG_INFO("STATE: Switching to Turn State");
    return;
  }

  if (buttonOpenPushedState) {
    LOG_TRACE("LOGIC: Open Button Pushed");
    vendingState = 3; // Validate
    timerServerTimeout.stop();
    timerServerTimeout.start();
    LOG_DEBUG("TIMER: Server Timeout Timer started");
    LOG_INFO("STATE: Switching to Validate State");
    buttonOpenPushedState = false;
    if (activeScreen != 2) {
      lv_screen_load(ui_ValidationScreen);
      activeScreen = 2;
    }
    return;
  }
}


void vendingTurn() {
  if (!carrouselUnlockedState) {
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: carrouselUnlockedState false");
    LOG_DEBUG("HARDWARE: Unlock carrousel");
    carrouselUnlock();
    return;
  }

  if (buttonTurnPushedState) {
    if (!motorOnState) {
      LOG_TRACE("LOGIC: Turn Button Pushed");
      LOG_TRACE("LOGIC: motorOnState false");
      LOG_DEBUG("HARDWARE: Turn Motor On");
      motorOn();
      return;
    }
    return;
  }
  else {
    LOG_DEBUG("HARDWARE: Lock carrousel");
    lv_obj_set_state(ui_ButtonSpin, LV_STATE_PRESSED, true);  //_GUI spinbutton add clicked state on mainscreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    if (carrouselUnlockedState) {
      carrouselLock();
    }
    delay(MotorTurnDELAY);
    LOG_TRACE("HARDWARE: MotorTurnDELAY");
    LOG_DEBUG("HARDWARE: Turn Motor Off");
    if (motorOnState) {
      motorOff();
    }
    lv_obj_set_state(ui_ButtonSpin, LV_STATE_PRESSED, false); //_GUI spinbutton remove clicked state on mainscreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    //carrouselLock(); -- NOTE: double and not necessary?
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 1; // Idle
    LOG_INFO("STATE: Switching to Idle State");
    return;
  }
}


void vendingValidate() {
  // Server Timeout
  if (timerServerTimeout.read() > ServerTimeout) {
    //_GUI SERVER ERROR MESSAGE ON ENDSCREEN
    lv_screen_load(ui_EndScreen);
    lv_obj_remove_flag(ui_ErrorTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_DeniedTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_CompleteTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui_ErrorLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ThankYouLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_DeniedLabel, LV_OBJ_FLAG_HIDDEN);
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    CoreS3.delay(READINGDELAY); //_GUI READINGDELAY
    LOG_TRACE("LOGIC: Server Timeout Timer larger then ServerTimeout");
    timerServerTimeout.stop();
    LOG_DEBUG("TIMER: Server Timeout Timer stopped");
    LOG_DEBUG("LOGIC: transactionActive set to false");
    transactionActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    requestActive = false;
    vendingState = 6; // Error
    LOG_INFO("STATE: Switching to Error State");
    return;
  }
  

  // Call permissionRequest-function when there is no acitve Transaction
  if (!transactionActive) {
    //_GUI Validation Status Label on ValidationScreen
    //_GUI hide UserActionPanel on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: transactionActive is false");
    LOG_INFO("LOGIC: Starting Permission Request");
    permissionRequest();
    LOG_DEBUG("LOGIC: call permissionRequest");
    requestActive = true;
    LOG_DEBUG("LOGIC: requestActive set to true");
    return;
  }
  // Active Transaction --> Stop timerServerTimeout, Set logic variables & switch State
  else if(transactionActive) {
    //_GUI Access Granted Status Label on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: transactionActive is true");
    timerServerTimeout.stop();
    LOG_DEBUG("TIMER: Server Timeout Timer stopped");
    requestActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    vendingState = 4; // Collect
    LOG_INFO("STATE: Switching to Collect State");
    return;
  }
}


void vendingCollect(){
  //if (timerPurchaseTimeout.read() > PurchaseTimeoutDELAY) { -- NOTE: Purchase Timeout handled through MoniToni --> timerPurchaseTimeout not necessary
  // __NUNU__ if (!permissionRequest()) {
  if (!permission) {
    //_GUI Denied Status Label on EndScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    CoreS3.delay(READINGDELAY); //_GUI READINGDELAY
    LOG_TRACE("LOGIC: permissionRequest returns false");
    //timerPurchaseTimeout.stop();  -- NOTE: see previous note
    transactionActive = false;
    LOG_DEBUG("LOGIC: transactionActive set to false");
    requestActive = false;
    LOG_DEBUG("LOGIC: requestActive set to false");
    timerSleep.stop();
    timerSleep.start();
    LOG_DEBUG("TIMER: Sleep Timer started");
    vendingState = 6; // Error
    LOG_INFO("STATE: Switching to Error State");
    return;
  }
  // _____________HIGHT PRIORITY TO DO_____________
  // -- NOTE: if statement to check permissionRequest might not be needed --> permissionRequest already triggered in previous state

  if (!itemUnlockedState && !doorOpenState) {
    //_GUI Door Open Status Label on ValidationScreen
    //_GUI show UserActionPanel on ValidationScreen
    //_GUI User Open Action Label on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: itemUnlockedState && doorOpenState are false");
    LOG_DEBUG("HARDWARE: Unlock Item: " + item);
    itemUnlock(item);
    timerServerTimeout.start(); //NU
    LOG_DEBUG("TIMER: Sleep Timer started");
    return;
  }

  if (doorOpenState && transactionActive) {
    //_GUI Door Open Status Label on ValidationScreen
    //_GUI User Close Action Label on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: doorOpenState && transactionActive are true");
    delay(openDoorDELAY);
    LOG_TRACE("HARDWARE: openDoorDELAY");
    LOG_DEBUG("HARDWARE: Lock Items");
    if (itemUnlockedState) {
      itemLock(item);
    }
    if( completeRequest() ) {
      LOG_TRACE("LOGIC: completeRequest returns true");
      //timerPurchaseTimeout.stop();   -- NOTE: see previous note
      timerServerTimeout.stop(); //NU
      timerServerTimeout.start(); //NU
      LOG_DEBUG("TIMER: Server Timeout Timer started");
      timerDoorOpen.start();
      LOG_DEBUG("TIMER: Door Open Timer started");
      transactionActive = false;
      LOG_DEBUG("LOGIC: transactionActive set to false");
      vendingState = 5; // Finished
      LOG_INFO("STATE: Switching to Finished State");
      return;
    }
    else {
      LOG_ERROR("LOGIC: completeRequest returns false");
      //_NUNU_ check logic flow -> if completeRequest returns false
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      //_GUI SERVER ERROR MESSAGE ON ENDSCREEN
      lv_task_handler(); //_GUI ui handler
      CoreS3.delay(READINGDELAY); //_GUI READINGDELAY
      LOG_TRACE("LOGIC: Server Timeout Timer greater than ServerTimeout");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      vendingState = 6; // Error
      LOG_INFO("STATE: Switching to Error State");
      return;
    }
  }
}


void vendingFinished() {
  if (timerDoorOpen.read() > DoorOpenSireneDELAY) {
    //_GUI Door Open Status Label on ValidationScreen
    //_GUI User Close Action Label on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: Door Open Timer greater than DoorOpenSireneDELAY");
    LOG_DEBUG("HARDWARE: Turn Sirene On");
    if (!sireneOnState) {
      sireneOn();
    }
    timerDoorOpen.stop();
    LOG_DEBUG("TIMER: Door Open Timer stopped");
  }

  if (!doorOpenState) {
    //_GUI Door Close Status Label on ValidationScreen
    //_GUI hide UserActionPanel on ValidationScreen
    ui_ticker();
    lv_task_handler(); //_GUI ui handler
    LOG_TRACE("LOGIC: doorOpenState is false");
    LOG_DEBUG("HARDWARE: Turn sirene off");
    if (sireneOnState) {
      sireneOff();
    }
    timerDoorOpen.stop();
    LOG_DEBUG("TIMER: Door Open Timer stopped");
    if ( closeRequest() ) {
      //_GUI CompleteTransactionLabel on EndingScreen
      //_GUI ThankYouLabel on EndingScreen
      ui_ticker();
      lv_task_handler(); //_GUI ui handler
      CoreS3.delay(READINGDELAY); //_GUI READINGDELAY
      LOG_TRACE("LOGIC: closeRequest returned true");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      timerSleep.stop();
      timerSleep.start();
      LOG_DEBUG("TIMER: Sleep Timer started");
      vendingState = 1; // Idle
      LOG_INFO("STATE: Switching to Idle State");
      return;
    }
    if (timerServerTimeout.read() > ServerTimeout) {
      //_GUI SERVER ERROR MESSAGE ON ENDSCREEN
      ui_ticker();
      lv_task_handler(); //_GUI ui handler
      CoreS3.delay(READINGDELAY); //_GUI READINGDELAY
      LOG_TRACE("LOGIC: Server Timeout Timer greater than ServerTimeout");
      timerServerTimeout.stop();
      LOG_DEBUG("TIMER: Server Timeout Timer stopped");
      vendingState = 6; // Error
      LOG_INFO("STATE: Switching to Error State");
      return;
    }
  }
}


void vendingError() {
  LOG_ERROR("ERROR: Error State entered");
  timerServerTimeout.stop();
  LOG_DEBUG("TIMER: Server Timeout Timer stopped");
  //timerPurchaseTimeout.stop();
  if (carrouselUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Carrousel");
    carrouselLock();
  }
  if (motorOnState) {
    LOG_TRACE("HARDWARE: Turn Off Motor");
    motorOff();
  }
  if (!lightOnState) {
    LOG_TRACE("HARDWARE: Turn On Light");
    lightOn();
  }
  if (sireneOnState) {
    LOG_TRACE("HARDWARE: Turn Off Sirene");
    sireneOff();
  }
  if (itemUnlockedState) {
    LOG_TRACE("HARDWARE: Lock Items");
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }
  

  transactionActive = false;
  LOG_DEBUG("LOGIC: requestActive set to false");
  requestActive = false;
  LOG_DEBUG("LOGIC: vendingActive set to false");
  vendingState = 1; // Idle
  LOG_INFO("STATE: Switching to Sleep State");
}






// _____________Finite State Machine - Possible States: 0 = Sleep / 1 = Idle / 2 = Turn / 3 = Validate / 4 = Collect / 5 = Finished / 6 = Error_____________

void vending(int state) {
  switch (state){
    // 0 = Sleep
    case 0:
      vendingSleep();
      break;
    // 1 = Idle
    case 1:
      vendingIdle();
      break;
    // 2 = Turn
    case 2:
      vendingTurn();
      break;
    // 3 = Validate
    case 3:
      vendingValidate();
      break;
    // 4 = Collect
    case 4:
      vendingCollect();
      break;
    // 5 = Finished
    case 5:
      vendingFinished();
      break;
    // 6 = Error
    case 6:
      vendingError();
      break;
  }
}


// _____________Main Loop_____________

void mainLoop() {
  // ui updates
  //lvgl ui ticker
  ui_ticker();
  //lvgl task handler
  lv_task_handler(); /* let the GUI do its work */


  //check wifi connection
  while (WiFi.status() != WL_CONNECTED)
  {
    // errorOn();
    delay(500);
    // errorOff();
    // Serial.print(".");
    //_GUI WIFI ERROR MESSAGE
  }

  // Update M5CoreS3 base functions
  CoreS3.update();

  //update hardware inputs
  updateInputs();


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

  //run state machine
  vending(vendingState);

  //restart esp daily
  if (globalHour == 1 && globalMinute == 45 && millis() >= 20000 ) {
    Serial.println("Daily Reset");
    ESP.restart();
  }
}


// _____________setup_____________

void setup()
{
  systemSetup();
}


// _____________loop_____________

void loop()
{
  mainLoop();
}
