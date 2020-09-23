#include <ArduinoBLE.h>
#include <Adafruit_NeoPixel.h>


#define NUMPIXELS      64
#define NUMSTRINGS 5

struct ledString {
  unsigned int color = 0x00FFFFFF;
  uint32_t glowColor = 0;
  uint8_t sat  = 255;
  int val  = 255;
  uint16_t hue = 0;
  bool on = true;
  bool glow = false;
  bool dec = true;
  int pinNum; 
};

Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, 2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 3, NEO_GRB + NEO_KHZ800), 
  Adafruit_NeoPixel(NUMPIXELS, 5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, 9, NEO_GRB + NEO_KHZ800),};

ledString pixelGroup[5];

bool anyGlow;
Adafruit_NeoPixel dummy;
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
  anyGlow = false;
  pixelGroup[0].hue = 54650;
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

unsigned int step = 0x10101;// single step decrease/increase for glow THIS WILL NOT WORK! DUH

void loop() {  

  BLE.poll(10);  
  if(anyGlow) {
    int val = pixelGroup[0].val;
    bool direction = pixelGroup[0].dec;
   // pixels[0].fill(dummy.gamma32(dummy.ColorHSV(pixelGroup[0].hue, pixelGroup[0].sat, val)),0);
    //pixels[0].show();
    if(direction) {
      if(val >= 25) {
        pixelGroup[0].val = val - 1;
      }
      else {
        pixelGroup[0].dec = false;        
      }
    }
    else {
      if(val < 255) {
        pixelGroup[0].val = val + 1;
      }
      else {
        pixelGroup[0].dec = true;        
      }
    }  
    
  } 

}

void periphConnectedHandler(BLEDevice central) {  

  for(int j = 0; j < NUMSTRINGS; j++) {
    if(pixelGroup[j].on == true) {
      pixels[j].fill(pixelGroup[j].color,0);       
    }
  }
  
  for(int k = 0; k < NUMSTRINGS; k++) {
    pixels[k].show();    
  }
}

void noConnectionHandler(BLEDevice central) {    
    
}

void colorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {    
    
    byte charRead[4];
    characteristic.readValue(charRead, 4);

    byte red = charRead[0];
    byte green = charRead[1];
    byte blue = charRead[2];    
    byte group = charRead[3];
    Serial.println("Group: " + (String) group);
    

  if (((group & 0xF0) == 0xE0)) {  
    Serial.println("Glow signal received");
    if(pixelGroup[group & 0xF].glow == true) {
      Serial.println("Off");
      pixelGroup[group & 0xF].glow = false;
    }
    else {
      Serial.println("On");
      pixelGroup[group & 0xF].glow = true; // toggle glow      
    }
    Serial.println("Toggled glow: " + (String) pixelGroup[group & 0xF].glow);    
    anyGlow = false;
    for(int k = 0; k < NUMSTRINGS; k++) {
      Serial.println("glow value of " + (String) k + " group "+ (String) pixelGroup[k].glow);
      anyGlow = anyGlow | pixelGroup[k].glow;// global glow variable, prevent unnecessary cycles in main loop
    }   
    Serial.println("anyGlow: " + (String) anyGlow);
  }
  int groupN = group & 0xF;
  if ((group & 0xF0) == 0xF0) {   
    Serial.println("Off Signal");        
      if((group & 0xFF) == 0xFF) { // all toggle, not implemented in app yet...
            
        for(int k = 0; k < NUMSTRINGS; k++) {
          pixels[k].clear();
          pixels[k].show();            
        } 
   
      }    
      else { // group N toggle
        Serial.println("Group N signal");  
        
      
        if(pixelGroup[groupN].on == true) { //OFF
          pixels[groupN].clear();
          pixels[groupN].show();          
          pixelGroup[groupN].on = false;
        }      
        else {//ON            
          pixels[groupN].fill(pixelGroup[groupN].color,0);          
          pixels[groupN].show();
          pixelGroup[groupN].on = true;                
        }
      }
  }
  
  if((pixelGroup[groupN].glow == false) && (pixelGroup[groupN].on == true)) {
    Serial.println("Color change");
      pixelGroup[groupN].color = pixels[0].Color(charRead[0],charRead[1],charRead[2]);      
      pixelGroup[groupN].glowColor = pixelGroup[groupN].color;
      pixels[groupN].fill(pixelGroup[groupN].color,0);      
      pixels[groupN].show();    
  }

}
