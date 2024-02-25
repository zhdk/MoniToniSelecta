///////////////////////------ imports ------///////////////////////

#include <Timer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <debounce.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

///////////////////////------ definitions ------///////////////////////

/*
#define I2C_SDA 16
#define I2C_SCL 17
#define RTC_IRQ 15

// PINS OUT
#define DATA_PIN 7
#define CLOCK_PIN 5
#define LATCH_PIN 6
*/

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

///////////////////////------ instances ------///////////////////////

WiFiClientSecure client;

/*
BLEService bleService("19B10000-E8F2-537E-4F6C-D104768A1214");  // create service

// create characteristics and allow remote device to read and write
BLEBoolCharacteristic itemCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLECharacteristic serverTimerCharacteristic("19B10014-E8F2-537E-4F6C-D104768A1214", BLERead, sizeof(int32_t));
BLECharacteristic stateCharacteristic("19B10018-E8F2-537E-4F6C-D104768A1214", BLEWrite, sizeof(int8_t) * 32);  // this is the maxium length of 32 bytes

byte NoBLECharacteristics = 3;  // this needs to match your total number of Characteristics
// you need to add all your Characteristics to the following array:
BLECharacteristic characteristicList[] = { itemCharacteristic, serverTimerCharacteristic, stateCharacteristic };
*/

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


