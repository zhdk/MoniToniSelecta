///////////////////////------ imports ------///////////////////////

#include <Timer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <ShiftRegister74HC595_NonTemplate.h>
#include <debounce.h>


///////////////////////------ definitions ------///////////////////////

#define I2C_SDA 16
#define I2C_SCL 17
#define RTC_IRQ 15

// PINS OUT
#define DATA_PIN 7
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define Error_PIN 8 // Error LED

// PINS IN
#define ButtonTurn_PIN 14 // PIN IO14
#define Door_PIN 12   // PIN IO12
#define ButtonOpen_PIN 11   // PIN IO11

// CHANNELS OUT
#define Item_1_CH 3
#define Item_2_CH 4
#define Item_3_CH 11
#define Item_4_CH 11
#define Item_5_CH 11
#define Item_6_CH 11
#define Item_7_CH 11
#define Item_8_CH 11
#define Item_9_CH 11
#define Item_10_CH 11
#define Light_CH 2
#define Sirene_CH 5
#define Motor_CH 1
#define Lock_CH 0

// Delays (in microseconds)
#define CarrouselDELAY 500
#define UserInputDELAY 5000
#define SleepDELAY 120000
#define debounceTurnButton 200
#define debounceOpenButton 100
#define debounceDoor 100
#define openDoorDELAY 100
#define PurchaseTimeoutDELAY 60000
#define DoorOpenSireneDELAY 10000
#define ServerTimeout 20000
#define MotorTurnDELAY 500


///////////////////////------ variables ------///////////////////////

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

const String host = "dev.monitoni.zhdk.ch";
const int port = 443;

String url_permission = "/api/vending/permission";
String url_complete = "/api/vending/complete";
String url_close = "/api/vending/close";
String monitoni_terminal = "Monitoni-Terminal: NfOr7GiqPnK9wd9taLBlsHJOIFdThNcE";

volatile bool permission = false;
volatile bool completed = false;
volatile bool closed = false;


///////////////////////------ instances ------///////////////////////

WiFiClientSecure client;

ShiftRegister74HC595_NonTemplate *control;

Timer timerButtonTurnPress;
Timer timerButtonOpenPress;
Timer timerSleep;
Timer timerPurchaseTimeout;
Timer timerServerTimeout;
Timer timerDoorOpen;


///////////////////////------ setup ------///////////////////////

void setup()
{
  systemSetup();
}


///////////////////////------ loop ------///////////////////////

void loop()
{
  mainLoop();
}


