 #include <ArduinoJson.h>
#include <SPI.h>
//#include <ESP8266WiFi.h>
//#include <WiFiNINA.h>
#include <WiFiClientSecure.h>
#include <ShiftRegister74HC595_NonTemplate.h>


#define I2C_SDA                     16
#define I2C_SCL                     17
#define RTC_IRQ                     15

#define DATA_PIN                    7
#define CLOCK_PIN                   5
#define LATCH_PIN                   6


//PINS OUT
#define Item_1_CH                   1   
//#define Item_2_PIN                   1       
#define Light_CH                        
#define Sirene_CH                   5    
#define Motor_CH                    0    
#define Lock_CH                     2    
//#define Error_PIN                    6    


//PIN IN
#define Button_PIN                   14    //PIN IO14
#define Door_PIN                     12    //PIN IO12

//TEMP
int requestDelay = 1;


const char* SSID     = "ZHdK-INVX2"; // Change this to your WiFi SSID
const char* PW = "WaLoMis03-Xd93+Agy5"; // Change this to your WiFi password

String host = "dev.monitoni.zhdk.ch";
int port = 443;

String url_permission = "/api/vending/permission";
String url_complete = "/api/vending/complete";
String url_close = "/api/vending/close";
String monitoni_terminal = "Monitoni-Terminal: NfOr7GiqPnK9wd9taLBlsHJOIFdThNcE";

bool permission = false;
bool completed = false;
bool closed = false;

int item;

volatile bool doorOpen = false;
volatile bool buttonPushed = false;
bool transactionActive = false;


WiFiClientSecure client;

ShiftRegister74HC595_NonTemplate     *control;


void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();

  // Make sure that the driver chip output is set to LOW when powering on
  control = new ShiftRegister74HC595_NonTemplate(24, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  assert(control);
  control->setAllLow();
  


  //Connect WIFI
  Serial.print("Connecting to ");
  Serial.println(SSID);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED) {
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
  while (now < 8 * 3600 * 2) {
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

  //client.setTimeout(10000);

  /*
  pinMode(Item_1_PIN, OUTPUT);
  pinMode(Item_2_PIN, OUTPUT);
  pinMode(Light_PIN, OUTPUT);
  pinMode(Sirene_PIN, OUTPUT);
  pinMode(Motor_PIN, OUTPUT);
  pinMode(Lock_PIN, OUTPUT);
  pinMode(Error_PIN, OUTPUT);
*/

  pinMode(Button_PIN, INPUT_PULLUP);
  pinMode(Door_PIN, INPUT_PULLUP);
  

  attachInterrupt(Button_PIN, ISR_Button, CHANGE);
  attachInterrupt(Door_PIN, ISR_Door, CHANGE);

  /*
  digitalWrite(Item_1_PIN, HIGH);
  digitalWrite(Item_2_PIN, HIGH);
  digitalWrite(Light_PIN, HIGH);
  digitalWrite(Sirene_PIN, HIGH);
  digitalWrite(Motor_PIN, HIGH);
  digitalWrite(Lock_PIN, HIGH);
  digitalWrite(Error_PIN, HIGH);
  */

/*
  Serial.println("Testing Error Led");
  for (int n = 0; n < 15; n++) {
    digitalWrite(Error_PIN, LOW);
    delay(200);
    digitalWrite(Error_PIN, HIGH);
    delay(200);
  }
*/
}



void loop() {
/*  
  //Button Pushed
  if (buttonPushed == true) {
    Serial.println("Button Pressed");
    //Lights on
    Serial.println("Lights on");
    digitalWrite(Light_PIN, LOW);
    delay(50);
    //Unlock Solenoids
    Serial.println("Unlock Carusell");
    digitalWrite(Lock_PIN, LOW);

    //Motor
    Serial.println("Turning Motor");
    while(buttonPushed == true){  
      digitalWrite(Motor_PIN, LOW);
    }
    Serial.println("Motor Off");
    digitalWrite(Motor_PIN, HIGH);
    Serial.println("Lock Carusell");
    digitalWrite(Lock_PIN, HIGH);
    delay(1000);
    return;
  }
*/

  //Check Monitoni
  if (transactionActive == false) {
    Serial.println("Check Permission");
    permissionRequest();
    delay(requestDelay);
  }
  
  control->set(Light_CH, HIGH);
  //digitalWrite(Light_PIN, LOW);

  //Transaction
  if (permission == true) {
    Serial.println("Transaction active");
    transactionActive = true;

    Serial.println("Lights On");
    control->set(Light_CH, LOW);
    //digitalWrite(Light_PIN, HIGH);

    //Transaction Item
    Serial.print("Item: ");
    Serial.println(item);
    
    switch (item) {
      case 1:
        Serial.println("Solenoid 1");
        control->set(Item_1_CH, HIGH);
        //digitalWrite(Item_1_PIN, LOW);
        break;
      case 2:
        Serial.println("Solenoid 2");
        //digitalWrite(Item_2_PIN, LOW);

        break;
      default:
        break;
    }

    //Sirene
    Serial.println("Sound Notification");
    Serial.println("Sirene On");
    control->set(Sirene_CH, HIGH);
    //digitalWrite(Sirene_PIN, LOW);
    delay(500);
    Serial.println("Sirene Off");
    control->set(Sirene_CH, LOW);
    //digitalWrite(Sirene_PIN, HIGH);

    //Wait till door open
    Serial.println("Please Open Door & Take Out Item");
    while(doorOpen == false) {
    }

    //Send Complete Request
    if (doorOpen == true) {
      delay(100);
      Serial.println("Sending Complete Request");
      completeRequest();
    }
  }

  //Transaction Active
  if (transactionActive == true) {
    //wait for door to close
    Serial.println("Please Close Door");
    while (doorOpen == true) {
    }

    //Door
    Serial.println("Closing All Item Doors");
    control->set(Item_1_CH, LOW);
    //digitalWrite(Item_1_PIN, HIGH);
    //digitalWrite(Item_2_PIN, HIGH);

    //Sirene
    Serial.println("Sound Notification");
    Serial.println("Sirene On");
    control->set(Sirene_CH, HIGH);
    //digitalWrite(Sirene_PIN, LOW);
    delay(500);
    Serial.println("Sirene Off");
    control->set(Sirene_CH, LOW);
    //digitalWrite(Sirene_PIN, HIGH);
    
    //Send Closed Request
    delay(100);
    if (doorOpen == false) {
      Serial.println("Send Closed Request");
      closeRequest();

      transactionActive = false;

      //Light
      Serial.println("Light Off");
      control->set(Light_CH, LOW);
      //digitalWrite(Light_PIN, HIGH);
    }
  }
}




void ISR_Button () {
  buttonPushed = !digitalRead(Button_PIN);

  //Button Pushed
  if (buttonPushed == true) {
    Serial.println("Button Pressed");
    //Lights on
    Serial.println("Lights on");
    control->set(Light_CH, HIGH);
    //digitalWrite(Light_PIN, LOW);
    //Unlock Solenoids
    Serial.println("Unlock Carusell");
    control->set(Lock_CH, HIGH);
    //digitalWrite(Lock_PIN, LOW);

    //Motor
    Serial.println("Turning Motor");
    control->set(Motor_CH, HIGH);
    //digitalWrite(Motor_PIN, LOW);
  }
  else {
    Serial.println("Motor Off");
    control->set(Motor_CH, LOW);
    //digitalWrite(Motor_PIN, HIGH);
    Serial.println("Lock Carusell");
    control->set(Lock_CH, LOW);
    //digitalWrite(Lock_PIN, HIGH);
  }
}

void ISR_Door () {
  doorOpen = !digitalRead(Door_PIN);
}




void updateInputs (){
  buttonPushed = !digitalRead(Button_PIN);
  doorOpen = !digitalRead(Door_PIN);
}




void permissionRequest (){
  //Serial.print("Connecting to ");
  //Serial.println(host);
  if (!client.connect("dev.monitoni.zhdk.ch", 443)) {
    Serial.println("Connection failed");
    return;
  }

  // Send HTTP request
  //Serial.print("Requesting URL: ");
  //Serial.println(url_permission);
  client.println(String("GET " + url_permission + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0) {
    Serial.println(String("Failed to send request"));
    client.stop();
    return;
  }
  else {
    //Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(String("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(String("Invalid response"));
    client.stop();
    return;
  }

  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(String("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return;
  }

  //Extract Values
  permission = doc["HasPermission"].as<bool>();
  if (permission == true) {
    Serial.println(F("Item:"));
    //Serial.println(doc["HasPermission"].as<bool>());
    Serial.println(doc["Item"].as<int>());
    item = doc["Item"].as<int>();
  }


  // Disconnect
  client.stop();
}


void completeRequest (){
  //Serial.print("Connecting to ");
  //Serial.println(host);
  if (!client.connect("dev.monitoni.zhdk.ch", 443)) {
    Serial.println("Connection failed");
    return;
  }


  // Send HTTP request
  //Serial.print("Requesting URL: ");
  //Serial.println(url_complete);
  client.println(String("GET " + url_complete + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0) {
    Serial.println(String("Failed to send request"));
    client.stop();
    return;
  }
  else {
    //Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    //Serial.print(String("Unexpected response: "));
    //Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(String("Invalid response"));
    client.stop();
    return;
  }

  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(String("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return;
  }

  while(client.available())
  {
    // read an incoming byte from the server and print them to serial monitor:
    char c = client.read();
    Serial.print(c);
  }

  // Extract values
  //Serial.println(F("Response:"));
  //Serial.println(doc.as<String>());

  // Disconnect
  client.stop();

  completed = doc.as<String>();
}


void closeRequest (){
  //Serial.print("Connecting to ");
  //Serial.println(host);
  if (!client.connect("dev.monitoni.zhdk.ch", 443)) {
    Serial.println("Connection failed");
    return;
  }


  // Send HTTP request
  //Serial.print("Requesting URL: ");
  //Serial.println(url_close);
  client.println(String("GET " + url_close + " HTTP/1.0"));
  client.println(String("Host: " + host));
  client.println(String(monitoni_terminal));
  client.println(String("Connection: close"));
  if (client.println() == 0) {
    Serial.println(String("Failed to send request"));
    client.stop();
    return;
  }
  else {
    //Serial.println("Request sent");
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    //Serial.print(String("Unexpected response: "));
    //Serial.println(status);
    client.stop();
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(String("Invalid response"));
    client.stop();
    return;
  }

  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(String("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return;
  }

  while(client.available())
  {
    // read an incoming byte from the server and print them to serial monitor:
    char c = client.read();
    Serial.print(c);
  }
/*  
  // Extract values
  Serial.println(F("Response:"));
  Serial.println(doc);
*/
  // Disconnect
  client.stop();

}
