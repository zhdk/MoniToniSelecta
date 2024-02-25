#include <ShiftRegister74HC595_NonTemplate.h>

///////////////////////------ definitions ------///////////////////////

#define I2C_SDA 16
#define I2C_SCL 17
#define RTC_IRQ 15

// PINS OUT
#define DATA_PIN 7
#define CLOCK_PIN 5
#define LATCH_PIN 6



ShiftRegister74HC595_NonTemplate *control;


void setup() {
  Serial.begin(9600);
  Serial.println("Enter pin commands in the format: 'x on' or 'x off' where x is the pin number.");

  control = new ShiftRegister74HC595_NonTemplate(24, DATA_PIN, CLOCK_PIN, LATCH_PIN);
  assert(control);
  control->setAllLow();
  pinMode(8, OUTPUT);
  pinMode(47, OUTPUT);
  digitalWrite(47, LOW);
}

void loop() {
  
  char inputBuffer[20];
  int pinNumber = 0;
  String command;

  // Check if data is available to read from the serial port
  if (Serial.available()) {
    delay(10); // Small delay for the entire message to arrive
    int size = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);

    // Null terminate the string
    inputBuffer[size] = '\0';

    String inputString = String(inputBuffer);
    inputString.trim();  // Remove any leading/trailing whitespace

    // Split the input into pin number and command
    int spaceIndex = inputString.indexOf(' ');
    if(spaceIndex != -1) {
      pinNumber = inputString.substring(0, spaceIndex).toInt();
      command = inputString.substring(spaceIndex + 1);
      
      if (command == "on") {
        pinMode(pinNumber, OUTPUT);
        digitalWrite(pinNumber, HIGH);
        Serial.print("Pin ");
        Serial.print(pinNumber);
        Serial.println(" turned ON");
      } 
      else if (command == "off") {
        pinMode(pinNumber, OUTPUT);
        digitalWrite(pinNumber, LOW);
        Serial.print("Pin ");
        Serial.print(pinNumber);
        Serial.println(" turned OFF");
      } 
      else {
        Serial.println("Invalid command. Use 'on' or 'off'.");
      }
    } 
    else {
      Serial.println("Invalid format. Use 'x on' or 'x off'.");
    }
  }
  
}
