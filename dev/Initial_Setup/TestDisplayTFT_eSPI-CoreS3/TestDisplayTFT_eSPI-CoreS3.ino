#include <M5CoreS3.h>

void stringmessage(const char *msg) {
  //display message
  Serial.println(msg);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  stringmessage("String test");
}

void loop() {
  // put your main code here, to run repeatedly:

}
