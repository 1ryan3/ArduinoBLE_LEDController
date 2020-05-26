#include <ArduinoBLE.h>


void setup() {
  if (!BLE.begin()) {
    Serial.println("Failed to start BLE module");
    while (1);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  delay(5000);
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  BLE.setEventHandler(BLEConnected, periphConnectedHandler);
  BLE.setEventHandler(BLEDisconnected, noConnectionHandler);
  BLE.setLocalName("LED");
  BLE.setDeviceName("LED");
  BLE.advertise();
  
}

void loop() {
  // put your main code here, to run repeatedly\

  BLE.poll();

}

void periphConnectedHandler(BLEDevice central) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level) 
}

void noConnectionHandler(BLEDevice central) {
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}
