#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 2 , NEO_GRB + NEO_KHZ800);
BLEService colorService("7f453f59-6477-40d8-8054-f5526953986a");

void setup() {
  pixels.begin();
  pixels.setBrightness(50);
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
  BLEIntCharacteristic colorSetInt("7f453f59-6477-40d8-8054-f5526953986a", BLERead | BLEWrite);
  colorService.addCharacteristic(colorSetInt);
  colorSetInt.setEventHandler(BLEWritten, colorCharacteristicWritten);
  BLE.addService(colorService);
  BLE.setAdvertisedService(colorService);
  
  
  BLE.advertise();
  
}

void loop() {
  // put your main code here, to run repeatedly\

  BLE.poll();

}

void periphConnectedHandler(BLEDevice central) {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level) 
  pixels.setPixelColor(0, pixels.Color(100,100,100));
  pixels.setPixelColor(1, pixels.Color(100,100,100));
  pixels.setPixelColor(2, pixels.Color(100,100,100));
  pixels.show(); 
}

void noConnectionHandler(BLEDevice central) {
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  pixels.clear();
  pixels.show();
}

void colorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {    
    
    byte charRead[4];
    characteristic.readValue(charRead, 4);
    Serial.println(charRead[0]);
    Serial.println(charRead[1]);
    Serial.println(charRead[2]);
    pixels.setPixelColor(charRead[3], pixels.Color(charRead[0],charRead[1],charRead[2]));
    //pixels.setPixelColor(1, pixels.Color(charRead[0],charRead[1],charRead[2]));
   // pixels.setPixelColor(2, pixels.Color(charRead[0],charRead[1],charRead[2]));
    pixels.show();
}
