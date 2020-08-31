#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS      64
#define NUMSTRINGS 5

struct ledString {
  unsigned long color = 0x00FFFFFF;
  bool on = true;
  int pinNum; 
};

Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 3, NEO_GRB + NEO_KHZ800), 
  Adafruit_NeoPixel(NUMPIXELS, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 9, NEO_GRB + NEO_KHZ800),};

ledString pixelGroup[5];


  
BLEService colorService("7f453f59-6477-40d8-8054-f5526953986a");

void setup() {
  
  for (int i = 0; i < NUMSTRINGS; i++) {
    
    pixels[i].begin(); 
    pixels[i].setBrightness(100);
    pixels[i].clear();
    pixels[i].show();    
    pixelGroup[i].pinNum = pixels[i].getPin();
     
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

  BLE.poll();

}

void periphConnectedHandler(BLEDevice central) {  
  for (int i = 0; i < NUMPIXELS ; i++) {
    for(int j = 0; j < NUMSTRINGS; j++) {
      pixels[j].setPixelColor(i, pixelGroup[j].color);       
    }
  }
  for(int k = 0; k < NUMSTRINGS; k++) {
    pixels[k].show();    
  }
}

void noConnectionHandler(BLEDevice central) {    
    pixels[0].show();
    pixels[1].show();
    pixels[2].show();
    pixels[3].show();
    pixels[4].show();
}

void colorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {    
    
    byte charRead[4];
    characteristic.readValue(charRead, 4);
    Serial.println(charRead[3]);
   if ((charRead[3] & 0xFF) > 0xF) {
            
        if((charRead[3] & 0xFF) == 0xFF) { // all off
          
          for(int k = 0; k < NUMSTRINGS; k++) {
            pixels[k].clear();
            pixels[k].show();
            pixelGroup[k].color = 0;
          } 
        }        
        else { // group N off          
          pixels[charRead[3] & 0xF].clear();
          pixels[charRead[3] & 0xF].show();
          pixelGroup[charRead[3] & 0xF].color = 0;
        }
    }
    else {
    
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels[charRead[3]].setPixelColor(i, pixels[charRead[3]].Color(charRead[0],charRead[1],charRead[2]));
      }   
       
      pixelGroup[charRead[3]].color = pixels[0].Color(charRead[0],charRead[1],charRead[2]);
      pixels[charRead[3]].show();
    }
}
