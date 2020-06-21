#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS      64

Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 3, NEO_GRB + NEO_KHZ800), 
  Adafruit_NeoPixel(NUMPIXELS, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 9, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 10, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 11, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 12, NEO_GRB + NEO_KHZ800),};
  
BLEService colorService("7f453f59-6477-40d8-8054-f5526953986a");

void setup() {
  
  for (int i = 0; i < 8; i++) {
    pixels[i].begin(); 
    pixels[i].setBrightness(50);
    pixels[i].clear();
    pixels[i].show(); 
  }
  
  if (!BLE.begin()) {
    Serial.println("Failed to start BLE module");
    while (1);
  }
    
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
  for (int i = 0; i < NUMPIXELS ; i++) {    
    pixels[0].setPixelColor(i, pixels[0].Color(100,100,100));
    pixels[1].setPixelColor(i, pixels[1].Color(100,100,100));
    pixels[2].setPixelColor(i, pixels[2].Color(100,100,100));
    pixels[3].setPixelColor(i, pixels[3].Color(100,100,100));
    pixels[4].setPixelColor(i, pixels[4].Color(100,100,100));    
  }
    pixels[0].show();
    pixels[1].show();
    pixels[2].show();
    pixels[3].show();
    pixels[4].show();
}

void noConnectionHandler(BLEDevice central) {  
  for (int i = 0; i < 8; i++) {    
    pixels[i].clear();    
  } 
    pixels[0].show();
    pixels[1].show();
    pixels[2].show();
    pixels[3].show();
    pixels[4].show();
}

void colorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {    
    
    byte charRead[4];
    characteristic.readValue(charRead, 4);
    /*Serial.println(charRead[0]);
    Serial.println(charRead[1]);
    Serial.println(charRead[2]);*/
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels[charRead[3]].setPixelColor(i, pixels[charRead[3]].Color(charRead[0],charRead[1],charRead[2]));
    }      
    //pixels.setPixelColor(1, pixels.Color(charRead[0],charRead[1],charRead[2]));
   // pixels.setPixelColor(2, pixels.Color(charRead[0],charRead[1],charRead[2]));
    pixels[charRead[3]].show();
}
