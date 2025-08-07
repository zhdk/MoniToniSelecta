// _____________MoniToni Vending Machine Integration_____________
// Hardware Iteration: M5CoreS3 Microcontroller & Waveshare 32CH Modbus RTU Relay Board


// ============================================================================
// MONITONI VENDING MACHINE CONTROLLER
// Hardware: M5CoreS3 + Waveshare 32CH Modbus RTU Relay Board
// ============================================================================

// _____________DEBUG CONFIGURATION_____________
#define DEBUGLOG_ENABLE_FILE_LOGGER
#define DEBUGLOG_DEFAULT_LOG_LEVEL_INFO
#define DEBUGLOG_DEFAULT_FILE_LEVEL_INFO
#define LOG_PREAMBLE ""
// #define DEBUGLOG_DISABLE_LOG  // Uncomment for release mode

// _____________SYSTEM CONFIGURATION_____________
#define WIFI_RESTART_COUNTER 120  // Restart WiFi after 120 seconds if no connection is established

// _____________HARDWARE PIN DEFINITIONS_____________
// SD Card SPI Pins
#define SD_SPI_SCK_PIN  36
#define SD_SPI_MISO_PIN 35
#define SD_SPI_MOSI_PIN 37
#define SD_SPI_CS_PIN   4

// Serial Communication Pins (M5CoreS3)
#define RX_PIN_SERIAL2 18
#define TX_PIN_SERIAL2 17

// Input Pins
#define Door_PIN 8   // GPIO 8 - M5CoreS3

// _____________RELAY CHANNEL ASSIGNMENTS_____________
// Item Dispensing Relays (1-10)
#define Item_1_CH 1
#define Item_2_CH 2
#define Item_3_CH 3
#define Item_4_CH 4
#define Item_5_CH 5
#define Item_6_CH 6
#define Item_7_CH 7
#define Item_8_CH 8
#define Item_9_CH 9
#define Item_10_CH 10

// System Control Relays
#define Lock_CH 12       // Carousel lock
#define Sirene_CH 13     // Siren (unused - using internal speaker)
#define Motor_CH 14      // Carousel motor

// Item Indicator LED Relays (Green lights)
#define Item_1_Light_Green 32  // Relay 17
#define Item_2_Light_Green 31  // Relay 18
#define Item_3_Light_Green 30  // Relay 19
#define Item_4_Light_Green 29  // Relay 20
#define Item_5_Light_Green 28  // Relay 21
#define Item_6_Light_Green 27  // Relay 22
#define Item_7_Light_Green 26  // Relay 23
#define Item_8_Light_Green 25  // Relay 24
#define Item_9_Light_Green 24  // Relay 25
#define Item_10_Light_Green 23 // Relay 26

// Status Light Relays
#define Red_Light_CH 17    // 
#define White_Light_CH 18  // 
#define Blue_Light_CH 19  // 


// _____________TIMING CONSTANTS (milliseconds)_____________
#define CarrouselDELAY 0
#define UserInputDELAY 5000
#define SleepDELAY 120000
#define debounceTurnButton 1
#define debounceOpenButton 1
#define debounceDoor 10
#define openDoorDELAY 500
#define PurchaseTimeoutDELAY 60000
#define DoorOpenSireneDELAY 10000
#define ServerTimeout 3000
#define MotorTurnDELAY 2400
#define BluetoothSetupDelay 15000
#define ErrorDELAY 3000
#define READINGDELAY 8000

// _____________SENSOR CONFIGURATION_____________
#define ProximityThreshold 2

// _____________AUDIO CONFIGURATION_____________
#define SpeakerVolume 20        // [0-255]
#define SpeakerFrequency 600    // Hz

// _____________DISPLAY CONFIGURATION_____________
#define TFT_BRIGHTNESS 75        // Display brightness [0-100]
#define TFT_HOR_RES   240        // Screen width
#define TFT_VER_RES   320        // Screen height

// LVGL buffer size (1/8 screen for better performance)
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 8 * (LV_COLOR_DEPTH / 8))


// _____________LIBRARY INCLUDES_____________
// Core Hardware Libraries
#include <M5CoreS3.h>              // M5CoreS3 by Tinyu-Zhao,M5Stack [1.0.0]
#include <SPI.h>                   // SPI by Arduino [2.1.1]

// Timing and Utilities
#include <Timer.h>                 // Timer by Stefan Staub [1.2.1]
#include <debounce.h>              // debounce by Aaron Kimball [0.2.0]

// Network and JSON
//#include <WiFi.h>
#include <WiFiClientSecure.h>      // WiFiClientSecure by Espressif [2.0.11]
#include <ArduinoJson.h>           // ArduinoJson by Benoit Blanchon [7.0.3]

// Storage and Logging
#include <SD.h>                    // SD by Arduino, SparkFun [1.2.4]
#include <DebugLog.h>              // DebugLog by hideakitai [0.8.3]

// Display and UI
#include <lvgl.h>                  // LVGL UI library [CUSTOM CONFIG]
#include <TFT_eSPI.h>              // TFT display library [CUSTOM CONFIG]
#include <ui.h>                    // Custom UI Files

// Communication Protocols
#include "modbus_crc.h"            // Modbus checksum calculation

// Configuration
#include "credentials.h"           // WiFi and API credentials


// _____________GLOBAL VARIABLES_____________

// ===== STATE MACHINE VARIABLES =====
volatile int vendingState = 1;     // 0=Sleep, 1=Idle, 2=Turn, 3=Validate, 4=Collect, 5=Finished, 6=Error
volatile int activeScreen = 0;     // 0=StartUp, 1=Main, 2=Validation, 3=End, 4=Sleep, 5=Error
volatile int item;                 // Currently selected item number

// ===== HARDWARE STATE FLAGS =====
volatile bool doorOpenState = false;
volatile bool buttonTurnPushedState = false;
volatile bool buttonOpenPushedState = false;
volatile bool carrouselUnlockedState = false;
volatile bool motorOnState = false;
volatile bool itemUnlockedState = false;
volatile bool sireneOnState = false;
volatile bool lightOnState = false;
volatile bool errorOnState = false;
volatile bool proximityTriggered = false;
volatile bool displayOn = true;

// ===== TRANSACTION STATE FLAGS =====
volatile bool transactionActive = false;
volatile bool requestActive = false;
volatile bool permission = false;
volatile bool completed = false;
volatile bool closed = false;

// ===== NETWORK CONFIGURATION =====
const char *SSID = WIFI_SSID;
const char *PW = WIFI_PASS;
const String host = "monitoni.zhdk.ch";
const int port = 443;
const String url_permission = "/api/vending/permission";
const String url_complete = "/api/vending/complete";
const String url_close = "/api/vending/close";
const String monitoni_terminal = TOKEN;
volatile bool wifiError = false;

// ===== TIME TRACKING =====
volatile int globalHour;
volatile int globalMinute;
struct tm timeinfo;
time_t now;

// ===== DISPLAY AND UI =====
void *draw_buf_1;                  // Draw buffer for graphics
unsigned long lastTickMillis = 0;  // Tick variable for LVGL
lv_display_t * disp;               // Display instance

// ===== SENSORS =====
uint16_t read_ps_value;            // Proximity sensor value

// ===== HARDWARE INSTANCES =====
WiFiClientSecure client;
Ltr5xx_Init_Basic_Para device_init_base_para = LTR5XX_BASE_PARA_CONFIG_DEFAULT;

// ===== TIMER INSTANCES =====
Timer timerButtonTurnPress;
Timer timerButtonOpenPress;
Timer timerSleep;
Timer timerPurchaseTimeout;
Timer timerServerTimeout;
Timer timerDoorOpen;


// _____________BUTTON HANDLERS_____________

static void doorSwitchHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_OPEN) {
    LOG_TRACE("LOGIC: Door opened");
    doorOpenState = true;
    timerDoorOpen.stop();
    return;
  }
  
  // btnState == BTN_PRESSED (door closed)
  LOG_TRACE("LOGIC: Door closed");
  doorOpenState = false;
  timerDoorOpen.stop();
  timerDoorOpen.start();
  LOG_TRACE("TIMER: Door open timer started");
}

static Button doorSwitch(1, doorSwitchHandler);


// _____________MODBUS COMMUNICATION_____________

void sendModbusCommand(int relayNumber, bool state) {
  unsigned char relayNumberAddress = relayNumber - 1;
  unsigned char cmdModbus[8] = {0x01, 0x05, 0, 0, 0, 0, 0, 0};
  unsigned int checksumModbus;
  
  cmdModbus[2] = 0;
  cmdModbus[3] = relayNumberAddress;
  cmdModbus[4] = state ? 0xFF : 0x00;  // 0xFF = ON, 0x00 = OFF
  cmdModbus[5] = 0;
  
  checksumModbus = ModbusCRC((unsigned char *)cmdModbus, 6);
  cmdModbus[6] = checksumModbus & 0xFF;
  cmdModbus[7] = checksumModbus >> 8;
  
  for (int i = 0; i < 8; i++) {
    Serial2.write(cmdModbus[i]);
  }
  delay(100);
}

void sendModbusOpen(int relayNumber) {
  sendModbusCommand(relayNumber, true);
}

void sendModbusClose(int relayNumber) {
  sendModbusCommand(relayNumber, false);
}


// _____________HARDWARE CONTROL FUNCTIONS_____________

// ===== CAROUSEL CONTROL =====
void carrouselLock() {
  sendModbusClose(Lock_CH);
  carrouselUnlockedState = false;
  LOG_TRACE("HARDWARE: Carousel locked");
}

void carrouselUnlock() {
  sendModbusOpen(Lock_CH);
  delay(CarrouselDELAY);
  carrouselUnlockedState = true;
  LOG_TRACE("HARDWARE: Carousel unlocked");
}

// ===== MOTOR CONTROL =====
void motorOn() {
  sendModbusOpen(Motor_CH);
  motorOnState = true;
  LOG_TRACE("HARDWARE: Motor on");
}

void motorOff() {
  sendModbusClose(Motor_CH);
  motorOnState = false;
  LOG_TRACE("HARDWARE: Motor off");
}

// ===== LIGHTING CONTROL =====
void lightOn() {
  sendModbusOpen(White_Light_CH);
  lightOnState = true;
  LOG_TRACE("HARDWARE: Main light on");
}

void lightOff() {
  sendModbusClose(White_Light_CH);
  lightOnState = false;
  LOG_TRACE("HARDWARE: Main light off");
}

void lightOnError() {
  sendModbusOpen(Red_Light_CH);
  errorOnState = true;
  LOG_TRACE("HARDWARE: Error light on");
}

void lightOffError() {
  sendModbusClose(Red_Light_CH);
  errorOnState = false;
  LOG_TRACE("HARDWARE: Error light off");
}

// ===== ITEM LIGHTING CONTROL =====
void lightItemControl(int itemNum, bool state) {
  // Array mapping item numbers to relay channels for cleaner code
  const int itemLightChannels[] = {0, // index 0 unused (items start at 1)
    Item_1_Light_Green, Item_2_Light_Green, Item_3_Light_Green, Item_4_Light_Green, Item_5_Light_Green,
    Item_6_Light_Green, Item_7_Light_Green, Item_8_Light_Green, Item_9_Light_Green, Item_10_Light_Green
  };
  
  if (itemNum < 1 || itemNum > 10) {
    LOG_ERROR("ERROR: Invalid item number for light control: ", itemNum);
    return;
  }
  
  if (state) {
    sendModbusOpen(itemLightChannels[itemNum]);
    LOG_TRACE("HARDWARE: Item ", itemNum, " light green on");
  } else {
    sendModbusClose(itemLightChannels[itemNum]);
    LOG_TRACE("HARDWARE: Item ", itemNum, " light green off");
  }
}

// ===== AUDIO CONTROL =====
void sireneOn() {
  CoreS3.Speaker.tone(SpeakerFrequency);
  sireneOnState = true;
  LOG_TRACE("HARDWARE: Siren on");
}

void sireneOff() {
  CoreS3.Speaker.stop();
  sireneOnState = false;
  LOG_TRACE("HARDWARE: Siren off");
}

// ===== ITEM DISPENSING CONTROL =====
void itemUnlock(int itemNum) {
  if (itemNum < 1 || itemNum > 10) {
    LOG_ERROR("ERROR: Invalid item number for unlock: ", itemNum);
    return;
  }
  sendModbusOpen(itemNum);
  itemUnlockedState = true;
  LOG_TRACE("HARDWARE: Item ", itemNum, " unlocked");
}

void itemLock(int itemNum) {
  if (itemNum < 1 || itemNum > 10) {
    LOG_ERROR("ERROR: Invalid item number for lock: ", itemNum);
    return;
  }
  sendModbusClose(itemNum);
  itemUnlockedState = false;
  LOG_TRACE("HARDWARE: Item ", itemNum, " locked");
}

// ===== SENSOR INPUT UPDATES =====
void updateSwitchInputs() {
  // Only update door switch during collect and finished states
  if (vendingState == 4 || vendingState == 5) {
    int buttonSignal = digitalRead(Door_PIN);
    doorSwitch.update(buttonSignal);
  }
}

void updateProximity() {
  read_ps_value = CoreS3.Ltr553.getPsValue();
  proximityTriggered = (read_ps_value > ProximityThreshold);
}

// _____________UI HELPER FUNCTIONS_____________

void showServerErrorUI() {
  if (activeScreen != 3) {
    lv_screen_load(ui_EndScreen);
    activeScreen = 3;
  }
  lv_obj_add_flag(ui_DeniedTransactionLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(ui_ErrorTransactionLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_CompleteTransactionLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_DeniedLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_remove_flag(ui_ErrorLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_ThankYouLabel, LV_OBJ_FLAG_HIDDEN);
  ui_ticker();
  lv_task_handler();
  CoreS3.delay(READINGDELAY);
}

// _____________HTTP REQUEST FUNCTIONS_____________

bool permissionRequest() {
  // Connect to server
  if (!client.connect("monitoni.zhdk.ch", 443)) {
    LOG_ERROR("ERROR: Connection failed - permission request");
    showServerErrorUI();
    return false;
  }

  // Send HTTP request
  client.println(String("GET " + url_permission + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  
  if (client.println() == 0) {
    LOG_ERROR("ERROR: Failed to send permission request");
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  
  if (strcmp(status + 9, "200 OK") != 0) {
    LOG_ERROR("ERROR: Unexpected response status: ", status);
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    LOG_ERROR("ERROR: Invalid response format");
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Parse JSON response
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, client);
  
  if (error) {
    LOG_ERROR("ERROR: JSON deserialization failed: ", error.f_str());
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Process response
  permission = doc["HasPermission"].as<bool>();
  client.stop();

  if (permission) {
    transactionActive = true;
    timerServerTimeout.stop();
    item = doc["Item"].as<int>();
    LOG_DEBUG("SUCCESS: Permission granted for item ", item);
    return true;
  } else {
    transactionActive = false;
    LOG_DEBUG("INFO: Permission denied");
    return false;
  }
}

bool completeRequest() {
  transactionActive = false;
  permission = false;

  // Connect to server
  if (!client.connect("monitoni.zhdk.ch", 443)) {
    LOG_ERROR("ERROR: Connection failed - complete request");
    showServerErrorUI();
    return false;
  }

  // Send HTTP request
  client.println(String("GET " + url_complete + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  
  if (client.println() == 0) {
    LOG_ERROR("ERROR: Failed to send complete request");
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));

  if (strcmp(status, "HTTP/1.1 201 Created") != 0) {
    LOG_ERROR("ERROR: Unexpected response status: ", status);
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    LOG_ERROR("ERROR: Invalid response format for complete request");
    client.stop();
    showServerErrorUI();
    return false;
  }

  client.stop();
  LOG_TRACE("SUCCESS: Complete request successful");
  return true;
}



bool closeRequest() {
  transactionActive = false;
  permission = false;

  // Connect to server
  if (!client.connect("monitoni.zhdk.ch", 443)) {
    LOG_ERROR("ERROR: Connection failed - close request");
    showServerErrorUI();
    return false;
  }

  // Send HTTP request
  client.println(String("GET " + url_close + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  
  if (client.println() == 0) {
    LOG_ERROR("ERROR: Failed to send close request");
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  
  if (strcmp(status, "HTTP/1.1 201 Created") != 0) {
    LOG_ERROR("ERROR: Unexpected response status: ", status);
    client.stop();
    showServerErrorUI();
    return false;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    LOG_ERROR("ERROR: Invalid response format for close request");
    client.stop();
    showServerErrorUI();
    return false;
  }

  client.stop();
  LOG_TRACE("SUCCESS: Close request successful");
  return true;
}


// _____________UI SETUP AND MANAGEMENT_____________

void ui_setup() {
  // Allocate graphics buffer
  draw_buf_1 = heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  // Initialize LVGL
  lv_init();
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf_1, DRAW_BUF_SIZE);
  lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

  // Setup touch input
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // Initialize custom UI
  ui_init();

  // Setup button callbacks
  lv_obj_add_event_cb(ui_ButtonSpin, onSpinButtonPressed, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(ui_ButtonSpin, onSpinButtonReleased, LV_EVENT_RELEASED, NULL);
  lv_obj_add_event_cb(ui_ButtonOpen, onOpenButtonPressed, LV_EVENT_PRESSED, NULL);
  lv_obj_add_event_cb(ui_ButtonOpen, onOpenButtonReleased, LV_EVENT_RELEASED, NULL);

  // Set startup screen
  lv_screen_load(ui_StartUpScreen);
  lv_obj_add_state(ui_StartUpPanel, LV_STATE_CHECKED);
  lv_obj_remove_flag(ui_SetupLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ui_StartUpErrorLabel, LV_OBJ_FLAG_HIDDEN);

  ui_ticker();
  lv_task_handler();
}

// Touch input callback
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  auto t = CoreS3.Touch.getDetail();
  if (t.state != 0) {
    data->point.x = TFT_HOR_RES - t.y;
    data->point.y = t.x;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// Display control functions
void turnOffDisplay() {
  if (activeScreen != 4) {
    CoreS3.Display.setBrightness(0);
    lv_screen_load(ui_MainScreen);
    lv_obj_add_flag(ui_ButtonOpen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ButtonSpin, LV_OBJ_FLAG_HIDDEN);
    ui_ticker();
    lv_task_handler();
    activeScreen = 4;
  }
  displayOn = false;
}

void turnOnDisplay() {
  if (activeScreen != 1) {
    CoreS3.Display.setBrightness(TFT_BRIGHTNESS);
    lv_screen_load(ui_MainScreen);
    lv_obj_remove_flag(ui_ButtonOpen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui_ButtonSpin, LV_OBJ_FLAG_HIDDEN);
    ui_ticker();
    lv_task_handler();
    activeScreen = 1;
  }
  displayOn = true;
}

// LVGL tick interface
void ui_ticker() {
  unsigned int tickPeriod = millis() - lastTickMillis;
  lv_tick_inc(tickPeriod);
  lastTickMillis = millis();
}

// _____________UI EVENT CALLBACKS_____________

static void onSpinButtonPressed(lv_event_t *event) {
  buttonTurnPushedState = true;
}

static void onSpinButtonReleased(lv_event_t *event) {
  buttonTurnPushedState = false;
}

static void onOpenButtonPressed(lv_event_t *event) {
  // Button press handled on release for better UX
}

static void onOpenButtonReleased(lv_event_t *event) {
  if (vendingState == 1) {
    buttonOpenPushedState = true;
  } else {
    buttonOpenPushedState = false;
  }
}



// _____________UI VALIDATION SCREEN HELPERS_____________

void setValidationScreenState(bool validating, bool granted, bool denied, bool doorOpen, bool showUserAction, bool closeAction) {
  // Show/hide panels
  lv_obj_remove_flag(ui_ValidationStatusPanel, LV_OBJ_FLAG_HIDDEN);
  if (showUserAction) {
    lv_obj_remove_flag(ui_UserActionPanel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_UserActionPanel, LV_OBJ_FLAG_HIDDEN);
  }

  // Set panel state
  if (granted) {
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_DEFAULT);
    lv_obj_add_state(ui_ValidationStatusPanel, LV_STATE_CHECKED);
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_DISABLED);
  } else if (denied) {
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_DEFAULT);
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_CHECKED);
    lv_obj_add_state(ui_ValidationStatusPanel, LV_STATE_DISABLED);
  } else {
    lv_obj_add_state(ui_ValidationStatusPanel, LV_STATE_DEFAULT);
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_CHECKED);
    lv_obj_remove_state(ui_ValidationStatusPanel, LV_STATE_DISABLED);
  }

  // Set status labels
  if (validating) {
    lv_obj_remove_flag(ui_ValidationStatusLabel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_ValidationStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }
  
  if (granted) {
    lv_obj_remove_flag(ui_AccessGrantedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_AccessGrantedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }
  
  if (denied) {
    lv_obj_remove_flag(ui_AccessDeniedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_AccessDeniedStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }

  // Set door status
  if (doorOpen) {
    lv_obj_add_flag(ui_DoorClosedStatusLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui_DoorOpenStatusLabel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_remove_flag(ui_DoorClosedStatusLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_DoorOpenStatusLabel, LV_OBJ_FLAG_HIDDEN);
  }

  // Set user action labels
  if (closeAction) {
    lv_obj_remove_flag(ui_UserCloseActionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_UserOpenActionLabel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(ui_UserCloseActionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui_UserOpenActionLabel, LV_OBJ_FLAG_HIDDEN);
  }

  ui_ticker();
  lv_task_handler();
}



// _____________STATE MACHINE FUNCTIONS_____________

void vendingSleep() {
  permission = false;
  
  // Stop timers and turn off display
  if (timerSleep.state() == RUNNING) {
    timerSleep.stop();
    LOG_TRACE("TIMER: Sleep timer stopped");
  }
  
  if (displayOn) {
    turnOffDisplay();
    LOG_TRACE("HARDWARE: Display turned off");
  }
  
  // Ensure all hardware is in safe state
  if (carrouselUnlockedState) carrouselLock();
  if (motorOnState) motorOff();
  if (lightOnState) lightOff();
  if (sireneOnState) sireneOff();
  if (itemUnlockedState) {
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }
  
  // Check proximity sensor for wake-up
  updateProximity();
  if (proximityTriggered) {
    timerSleep.stop();
    timerSleep.start();
    vendingState = 1;  // Wake up to Idle
    lv_screen_load(ui_MainScreen);
    proximityTriggered = false;
    LOG_DEBUG("STATE: Proximity triggered - switching to Idle");
  }
}

void vendingIdle() {
  permission = false;
  
  // Handle door alarm
  if (doorOpenState) {
    sireneOn();
    LOG_TRACE("LOGIC: Door open alarm triggered");
    return;
  }
  
  // Turn on display if needed
  if (!displayOn) {
    turnOnDisplay();
    LOG_TRACE("HARDWARE: Display turned on");
  }
  
  // Check sleep timeout
  if (timerSleep.read() > SleepDELAY) {
    timerSleep.stop();
    vendingState = 0;  // Go to sleep
    LOG_DEBUG("STATE: Sleep timeout - switching to Sleep");
    return;
  }
  
  // Ensure proper hardware state for idle
  if (carrouselUnlockedState) carrouselLock();
  if (motorOnState) motorOff();
  if (!lightOnState) lightOn();
  if (sireneOnState) sireneOff();
  if (itemUnlockedState) {
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }
  
  // Handle user input
  if (buttonTurnPushedState) {
    vendingState = 2;  // Turn
    LOG_DEBUG("STATE: Turn button pressed - switching to Turn");
    return;
  }
  
  if (buttonOpenPushedState) {
    vendingState = 3;  // Validate
    timerServerTimeout.stop();
    timerServerTimeout.start();
    buttonOpenPushedState = false;
    if (activeScreen != 2) {
      lv_screen_load(ui_ValidationScreen);
      activeScreen = 2;
    }
    LOG_DEBUG("STATE: Open button pressed - switching to Validate");
    return;
  }
}


void vendingTurn() {
  // Unlock carousel if needed
  if (!carrouselUnlockedState) {
    carrouselUnlock();
    ui_ticker();
    lv_task_handler();
    LOG_DEBUG("HARDWARE: Carousel unlocked for turning");
    return;
  }

  if (buttonTurnPushedState) {
    // Start motor if not already running
    if (!motorOnState) {
      motorOn();
      lv_obj_set_state(ui_ButtonSpin, LV_STATE_PRESSED, true);
      lv_obj_set_state(ui_ButtonOpen, LV_STATE_DISABLED, true);
      ui_ticker();
      lv_task_handler();
      LOG_DEBUG("HARDWARE: Motor started");
    }
    return;
  } else {
    // Button released - stop turning
    if (carrouselUnlockedState) carrouselLock();
    
    delay(MotorTurnDELAY);
    LOG_TRACE("HARDWARE: Motor turn delay completed");
    
    if (motorOnState) motorOff();
    
    // Reset UI state
    lv_obj_set_state(ui_ButtonSpin, LV_STATE_PRESSED, false);
    lv_obj_set_state(ui_ButtonOpen, LV_STATE_DISABLED, false);
    ui_ticker();
    lv_task_handler();
    
    // Return to idle
    timerSleep.stop();
    timerSleep.start();
    vendingState = 1;
    lv_screen_load(ui_MainScreen);
    LOG_DEBUG("STATE: Turn completed - returning to Idle");
  }
}


void vendingValidate() {
  // Check for server timeout
  if (timerServerTimeout.read() > ServerTimeout) {
    if (activeScreen != 3) {
      lv_screen_load(ui_EndScreen);
      activeScreen = 3;
    }
    lv_obj_remove_flag(ui_DeniedTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ErrorTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_CompleteTransactionLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(ui_DeniedLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ErrorLabel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_ThankYouLabel, LV_OBJ_FLAG_HIDDEN);
    ui_ticker();
    lv_task_handler();
    lightOnError();  // Use our function instead of ledSetRed()
    CoreS3.delay(READINGDELAY);
    
    timerServerTimeout.stop();
    transactionActive = false;
    requestActive = false;
    vendingState = 6;  // Error
    LOG_INFO("STATE: Server timeout - switching to Error");
    return;
  }

  if (!transactionActive) {
    // Show validation in progress
    setValidationScreenState(true, false, false, false, false, false);
    
    LOG_DEBUG("NETWORK: Starting permission request");
    permissionRequest();
    requestActive = true;
    return;
  } else {
    // Permission granted - proceed to collect
    setValidationScreenState(false, true, false, false, true, false);
    
    timerServerTimeout.stop();
    requestActive = false;
    vendingState = 4;  // Collect
    LOG_DEBUG("STATE: Permission granted - switching to Collect");
  }
}


void vendingCollect() {
  updateSwitchInputs();

  // Check if permission is still valid
  if (!permission) {
    ui_ticker();
    lv_task_handler();
    CoreS3.delay(READINGDELAY);
    
    transactionActive = false;
    requestActive = false;
    timerSleep.stop();
    timerSleep.start();
    vendingState = 6;  // Error
    LOG_INFO("STATE: Permission lost - switching to Error");
    return;
  }

  if (!doorOpenState) {
    // Door is closed - unlock item and show instruction
    if (!itemUnlockedState) {
      itemUnlock(item);
      lightItemControl(item, true);
      setValidationScreenState(false, true, false, false, true, false);
      LOG_DEBUG("HARDWARE: Item ", item, " unlocked and lit");
      return;
    }
    // Item already unlocked, just wait
    return;
  }

  // Door is open and transaction is active
  if (doorOpenState && transactionActive) {
    setValidationScreenState(false, true, false, true, true, true);
    
    delay(openDoorDELAY);
    
    // Lock item and turn off light
    if (itemUnlockedState) {
      itemLock(item);
    }
    lightItemControl(item, false);
    
    timerServerTimeout.start();
    
    if (completeRequest()) {
      timerServerTimeout.stop();
      timerServerTimeout.start();
      timerDoorOpen.start();
      transactionActive = false;
      vendingState = 5;  // Finished
      LOG_DEBUG("STATE: Transaction completed - switching to Finished");
      return;
    }
    
    // Check for server timeout during complete request
    if (timerServerTimeout.read() > ServerTimeout) {
      timerServerTimeout.stop();
      showServerErrorUI();
      vendingState = 6;  // Error
      LOG_INFO("STATE: Server timeout during complete - switching to Error");
      return;
    }
  }
}


void vendingFinished() {
  updateSwitchInputs();

  // Check if door has been open too long
  if (timerDoorOpen.read() > DoorOpenSireneDELAY) {
    setValidationScreenState(false, true, false, true, true, true);
    
    if (!sireneOnState) {
      sireneOn();
      LOG_DEBUG("HARDWARE: Door open alarm activated");
    }
    timerDoorOpen.stop();
    return;
  }
  
  if (!doorOpenState) {
    // Door closed - complete the transaction
    setValidationScreenState(false, true, false, true, true, true);
    
    if (sireneOnState) {
      sireneOff();
      LOG_DEBUG("HARDWARE: Door alarm deactivated");
    }
    timerDoorOpen.stop();
    lightOn();
    
    if (closeRequest()) {
      // Show success screen
      if (activeScreen != 3) {
        lv_screen_load(ui_EndScreen);
        activeScreen = 3;
      }
      lv_obj_add_flag(ui_DeniedTransactionLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_ErrorTransactionLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(ui_CompleteTransactionLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_DeniedLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_ErrorLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_remove_flag(ui_ThankYouLabel, LV_OBJ_FLAG_HIDDEN);
      ui_ticker();
      lv_task_handler();
      CoreS3.delay(READINGDELAY);
      
      timerServerTimeout.stop();
      timerSleep.stop();
      timerSleep.start();
      vendingState = 1;  // Return to idle
      lv_screen_load(ui_MainScreen);
      lightOn();
      LOG_DEBUG("STATE: Transaction complete - returning to Idle");
      return;
    }
    
    // Check for server timeout during close request
    if (timerServerTimeout.read() > ServerTimeout) {
      timerServerTimeout.stop();
      showServerErrorUI();
      vendingState = 6;  // Error
      LOG_INFO("STATE: Server timeout during close - switching to Error");
      return;
    }
  }
}


void vendingError() {
  LOG_ERROR("ERROR: Error state entered");
  permission = false;
  
  // Stop all timers
  timerServerTimeout.stop();
  
  // Ensure all hardware is in safe state
  if (carrouselUnlockedState) carrouselLock();
  if (motorOnState) motorOff();
  if (sireneOnState) sireneOff();
  if (itemUnlockedState) {
    for (int i = 1; i <= 10; i++) {
      itemLock(i);
    }
  }
  
  // Flash error light
  lightOnError();
  delay(ErrorDELAY);
  lightOffError();
  lightOn();

  // Reset state variables
  transactionActive = false;
  requestActive = false;
  permission = false;
  
  // Return to idle state
  vendingState = 1;
  lv_screen_load(ui_MainScreen);
  LOG_DEBUG("STATE: Error handled - returning to Idle");
}

// _____________FINITE STATE MACHINE_____________
// States: 0=Sleep, 1=Idle, 2=Turn, 3=Validate, 4=Collect, 5=Finished, 6=Error

void vending(int state) {
  switch (state) {
    case 0: vendingSleep(); break;
    case 1: vendingIdle(); break;
    case 2: vendingTurn(); break;
    case 3: vendingValidate(); break;
    case 4: vendingCollect(); break;
    case 5: vendingFinished(); break;
    case 6: vendingError(); break;
    default:
      LOG_ERROR("ERROR: Invalid vending state: ", state);
      vendingState = 6;  // Force error state
      break;
  }
}


// _____________SYSTEM INITIALIZATION_____________

void systemSetup() {
  // Initialize core hardware
  CoreS3.begin();
  CoreS3.Display.setBrightness(TFT_BRIGHTNESS);
  
  // Wait for serial connection (development aid)
  delay(2000);
  Serial.begin(115200);
  Serial.println();

  // Initialize UI and display
  ui_setup();

  // Initialize SD card for logging
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  if (!SD.begin(SD_SPI_CS_PIN, SPI, 40000000)) {
    Serial.println("SD Card initialization failed");
  }

  // Initialize Modbus communication
  Serial2.begin(9600, SERIAL_8N1, RX_PIN_SERIAL2, TX_PIN_SERIAL2);
  
  // Connect to WiFi
  setupWiFiConnection();
  
  // Setup time synchronization
  setupTimeSync();
  
  // Setup logging
  setupLogging();
  
  // Setup sensors and audio
  setupSensorsAndAudio();
  
  // Initialize hardware to safe state
  initializeHardwareState();
  
  // Setup input debouncing
  doorSwitch.setPushDebounceInterval(debounceDoor);

  LOG_INFO("SYSTEM: Setup completed successfully");
  
  // Play startup sound sequence
  playStartupSequence();
  
  // Load main screen
  lv_screen_load(ui_MainScreen);
}

void setupWiFiConnection() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lv_label_set_text(ui_SetupLabel, "Connecting to WiFi...");
    ui_ticker();
    lv_task_handler();
  }
  
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupTimeSync() {
  // Set time via NTP for SSL certificate validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  Serial.print("Waiting for NTP time sync: ");
  now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  gmtime_r(&now, &timeinfo);
  globalHour = timeinfo.tm_hour;
  globalMinute = timeinfo.tm_min;
  
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
  
  // Configure secure client
  client.setInsecure();
  client.setTimeout(20000);
}

void setupLogging() {
  // Create log filename based on current time
  String filename = "/" + String(timeinfo.tm_yday) + "_" + 
                   String(timeinfo.tm_hour) + "_" + 
                   String(timeinfo.tm_min) + ".txt";
  
  LOG_ATTACH_FS_AUTO(SD, filename, FILE_WRITE);
  LOG_INFO("LOGGING: Debug log initialized");
}

void setupSensorsAndAudio() {
  // Setup proximity sensor
  device_init_base_para.ps_led_pulse_freq = LTR5XX_LED_PULSE_FREQ_40KHZ;
  device_init_base_para.ps_measurement_rate = LTR5XX_PS_MEASUREMENT_RATE_50MS;
  CoreS3.Ltr553.setPsMode(LTR5XX_PS_ACTIVE_MODE);
  LOG_TRACE("SETUP: Proximity sensor configured");

  // Setup speaker
  auto spk_cfg = CoreS3.Speaker.config();
  if (spk_cfg.use_dac || spk_cfg.buzzer) {
    spk_cfg.sample_rate = 192000;  // High quality audio
  }
  CoreS3.Speaker.config(spk_cfg);
  CoreS3.Speaker.begin();
  
  if (!CoreS3.Speaker.isEnabled()) {
    LOG_TRACE("WARNING: Speaker not found");
  }
  CoreS3.Speaker.setVolume(SpeakerVolume);
  CoreS3.Speaker.setAllChannelVolume(255);
  LOG_TRACE("SETUP: Audio system configured");
}

void initializeHardwareState() {
  // Turn off unused relays
  //sendModbusClose(Relay15_CH);
  //sendModbusClose(Relay16_CH);
  LOG_TRACE("SETUP: Unused relays disabled");
}

void playStartupSequence() {
  // Play startup sound sequence
  const int tones[] = {2000, 1000, 2000, 1000};
  const int durations[] = {200, 200, 200, 200};
  
  for (int i = 0; i < 4; i++) {
    CoreS3.Speaker.tone(tones[i], durations[i]);
    CoreS3.delay(durations[i]);
  }
  
  // Wait for audio to complete
  while (CoreS3.Speaker.isPlaying()) {
    CoreS3.delay(1);
  }
  CoreS3.Speaker.stop();
}



// _____________MAIN LOOP_____________

void mainLoop() {
  // Update UI
  ui_ticker();
  lv_task_handler();

  // Check WiFi connection with recovery
  checkWiFiConnection();
  
  // Update M5CoreS3 base functions
  CoreS3.update();

  // Update time tracking
  updateTimeTracking();

  // Run state machine
  vending(vendingState);

  // Daily reset at 1:45 AM (after 50 seconds uptime to avoid boot loops)
  if (globalHour == 1 && globalMinute == 45 && millis() >= 50000) {
    LOG_INFO("SYSTEM: Performing daily reset");
    ESP.restart();
  }
}

void checkWiFiConnection() {
  static int wifiCounter = 0;
  
  if (WiFi.status() != WL_CONNECTED) {
    if (!wifiError) {
      LOG_ERROR("ERROR: WiFi connection lost at ", globalHour, ":", globalMinute);
      lightOnError();
      
      // Show WiFi error on startup screen
      if (activeScreen != 0) {
        lv_screen_load(ui_StartUpScreen);
        activeScreen = 0;
      }
      lv_obj_remove_flag(ui_WIFILabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_SetupLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ui_StartUpErrorLabel, LV_OBJ_FLAG_HIDDEN);
      ui_ticker();
      lv_task_handler();
      
      wifiError = true;
    }
    
    wifiCounter++;
    if (wifiCounter > WIFI_RESTART_COUNTER) {
      LOG_ERROR("ERROR: WiFi connection failed - restarting system");
      ESP.restart();
    }
    
    delay(1000);  // Wait before next check
    return;
  }

  // WiFi connection is good - check if we were previously in error state
  if (wifiError) {
    LOG_INFO("NETWORK: WiFi connection re-established at ", globalHour, ":", globalMinute);
    
    if (activeScreen != 1) {
      lv_screen_load(ui_MainScreen);
      activeScreen = 1;
    }
    ui_ticker();
    lv_task_handler();
    
    wifiError = false;
    wifiCounter = 0;  // Reset counter on successful reconnection
  }
}

void updateTimeTracking() {
  now = time(nullptr);
  gmtime_r(&now, &timeinfo);
  globalHour = timeinfo.tm_hour;
  globalMinute = timeinfo.tm_min;
}

// _____________ARDUINO MAIN FUNCTIONS_____________

void setup() {
  systemSetup();
}

void loop() {
  mainLoop();
}
