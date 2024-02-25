#include <ShiftRegister74HC595_NonTemplate.h>

///////////////////////------ definitions ------///////////////////////

#define I2C_SDA 16
#define I2C_SCL 17
#define RTC_IRQ 15

// PINS OUT
#define DATA_PIN 7
#define CLOCK_PIN 5
#define LATCH_PIN 6

#define ADDRESS_AMOUNT 21

String command = "GO";



ShiftRegister74HC595_NonTemplate *control;


void setup() {
  Serial.begin(9600);
  Serial.println("Type GO to cascade through the connected relays.");

  control = new ShiftRegister74HC595_NonTemplate(24, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  assert(control);
  control->setAllLow();
  delay(2000);
  pinMode(14, OUTPUT);
}

void loop() {
  char inputBuffer[21];
  int pinNumber = 0;
  //control->set(13, HIGH);


    
  delay(2000);
  digitalWrite(14, HIGH);
  //control->set(13,LOW);
  delay(5000);
  digitalWrite(14, LOW);
  

  /*
  // Check if data is available to read from the serial port
  if (Serial.available()) {
    delay(10); // Small delay for the entire message to arrive
    int size = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);

    // Null terminate the string
    inputBuffer[size] = '\0';

    String inputString = String(inputBuffer);
    
    if (inputString == command) {
      for (pinNumber = 0; pinNumber <= ADDRESS_AMOUNT; pinNumber++) {
        control->set(pinNumber, HIGH);
        delay(500);
      }
      delay(1000);
      for (pinNumber = 0; pinNumber <= ADDRESS_AMOUNT; pinNumber++) {
        control->set(pinNumber, LOW);
        delay(500);
      }
    }
    
    else {
      Serial.println("Invalid format. Use 'GO'.");
    }
  }
  */
}
