#include <SoftwareSerial.h>

#define BAUD_RATE 115200

SoftwareSerial swSer(D2, D1, false, 256);

void setup() {
  Serial.begin(BAUD_RATE);
  swSer.begin(BAUD_RATE);
  while (!Serial);
}

void loop() {
  while (swSer.available() > 0) {
    Serial.write(swSer.read());
    yield();
  }
}
