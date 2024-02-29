// _____________MoniToni Vending Machine Integration_____________

// set log level NONE, ERROR, WARN, INFO, DEBUG, TRACE
#define DEBUGLOG_DEFAULT_LOG_TRACE
// set log level for file output NONE, ERROR, WARN, INFO, DEBUG, TRACE
#define DEBUGLOG_DEFAULT_FILE_LEVEL_TRACE
// define DEBUGLOG_ENABLE_FILE_LOGGER to enable file logger
#define DEBUGLOG_ENABLE_FILE_LOGGER
// define debug preamble
// default preamble with file & codeline information about debug source
#define LOG_PREAMBLE LOG_SHORT_FILENAME, LOG_MACRO_APPEND_STR(L.__LINE__), __func__, ":"
// no preamble
#define LOG_PREAMBLE ""
// Uncommenting DEBUGLOG_DISABLE_LOG disables ASSERT and all log (Release Mode)
// PRINT and PRINTLN are always valid even in Release Mode
// #define DEBUGLOG_DISABLE_LOG


// _____________Library Imports_____________

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

// DebugLog.h by hideakitai [0.8.3]
#include <DebugLog.h>
// SD.h by Arduino, SparkFun [1.2.4]
#include <SD.h>


// _____________definitions_____________
#define fs SD

// PINS IN
#define Door_PIN 45   //Green
#define ButtonTurn_PIN 48 //Violette
#define ButtonOpen_PIN 21 //Orange

// PINS OUT
#define Item_1_CH 14  //Relay 1
#define Item_2_CH 13  //Relay 2
#define Item_3_CH 12  //Relay 3
#define Item_4_CH 11  //Relay 4
#define Item_5_CH 10  //Relay 5
#define Item_6_CH 9  //Relay 6
#define Item_7_CH 46  //Relay 7
#define Item_8_CH 3  //Relay 8
#define Item_9_CH 8  //Relay 9
#define Item_10_CH 18  //Relay 10
#define Light_CH 17  //Relay 11
#define Lock_CH 16  //Relay 12
#define Sirene_CH 15  //Relay 13
#define Motor_CH 7  //Relay 14
#define Relay15_CH 6  //Relay 15
#define Relay16_CH 5  //Relay 16
#define Error_PIN 47 //Yellow


// Delays (in microseconds)
#define CarrouselDELAY 500
#define UserInputDELAY 5000
#define SleepDELAY 120000
#define debounceTurnButton 50
#define debounceOpenButton 1
#define debounceDoor 100
#define openDoorDELAY 500
#define PurchaseTimeoutDELAY 60000
#define DoorOpenSireneDELAY 10000
#define ServerTimeout 3000
#define MotorTurnDELAY 2500
#define BluetoothSetupDelay 15000

#define BLINKS 30 //Error LED Blinks


// _____________variables_____________

// Logic Variables
volatile int vendingState = 0; // 0 = Sleep, 1 = Idle, 2 = Turn, 3 = Validate, 4 = Collect, 5 = Finished, 6 = Error

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

// Network Variables
const char *SSID = "ZHdK-INVX2";        // Change this to your WiFi SSID
const char *PW = "WaLoMis03-Xd93+Agy5"; // Change this to your WiFi password


const String host = "monitoni.zhdk.ch";
const int port = 443;

String url_permission = "/api/vending/permission";
String url_complete = "/api/vending/complete";
String url_close = "/api/vending/close";
String monitoni_terminal = "Monitoni-Terminal: oFX3HzsH9GjR8lrGNWT6L3hNGqTrGtc1";

volatile bool permission = false;
volatile bool completed = false;
volatile bool closed = false;

volatile int globalHour;
volatile int globalMinute;
struct tm timeinfo;
time_t now;

// _____________instances_____________

WiFiClientSecure client;

Timer timerButtonTurnPress;
Timer timerButtonOpenPress;
Timer timerSleep;
Timer timerPurchaseTimeout;
Timer timerServerTimeout;
Timer timerDoorOpen;


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


