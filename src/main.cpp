#include <Arduino.h>
#include <ArduinoBLE.h>

void setup() {
  Serial.begin(115200);

  if(!BLE.begin()) {
    Serial.println("Bluetooth Failed");
    while(1);
  }

  Serial.println("Bluetooth Started: Waiting for connection...");
}

void loop() {
  BLE.poll();
}

