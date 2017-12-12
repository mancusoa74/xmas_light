#include <Adafruit_NeoPixel.h>

// RemoteXY select connection mode and include library  
#define REMOTEXY_MODE__ESP8266WIFI_LIB
#include <ESP8266WiFi.h> 
#include <RemoteXY.h> 
#define REMOTEXY_WIFI_SSID "YOUR SSID" 
#define REMOTEXY_WIFI_PASSWORD "YOUR PASSWORD"
#define REMOTEXY_SERVER_PORT 6377 

// Led Strip config 
#define PIN D2
#define LEDS 50

int last_select = 0;

// RemoteXY configurate   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] = 
  { 255,1,0,0,0,66,0,6,4,1,
  3,10,7,16,9,80,2,129,0,15,
  4,36,6,11,88,109,97,115,32,76,
  105,103,104,116,115,0,129,0,24,46,
  22,6,9,67,108,105,99,107,32,116,
  111,0,129,0,18,56,41,6,9,67,
  104,97,110,103,101,32,69,102,102,101,
  99,116,0 }; 
   
// this structure defines all the variables of your control interface  
struct { 

    // input variable
  uint8_t select_1; // =0 if select position A, =1 if position B, =2 if position C, ... 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY; 
#pragma pack(pop) 


Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_RGB + NEO_KHZ800);

uint32_t random_color() {
  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);
  
  return strip.Color(r, g, b);
}

void setup() {
  Serial.begin(115200);
  Serial.println("starting Luci di Natale....");
  RemoteXY_Init ();  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void luci_off() {
  strip.clear();
  strip.show();
}

void RemoteXY_yield() {
  for(int c = 0; c < 11; c++) {
    RemoteXY_Handler (); 
    if (last_select != RemoteXY.select_1)
      return;
  }
}

void supercar(int wait) {
  uint16_t i, j;

  strip.clear();
  strip.show();
  
  for(i=0; i<strip.numPixels()-2; i++) {
    if (i > 0)
      strip.setPixelColor(i-1, strip.Color(0, 0, 0));
      
    strip.setPixelColor(i, strip.Color(255, 0, 0));
    strip.setPixelColor(i+1, strip.Color(255, 0, 0));
    strip.setPixelColor(i+2, strip.Color(255, 0, 0));
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  
  strip.clear();
  strip.show();
  for (j = strip.numPixels(); j > 2; j--){
    if (j < strip.numPixels())
      strip.setPixelColor(j+1, strip.Color(0, 0, 0));
   if (j >= 2) {
     strip.setPixelColor(j, strip.Color(255, 0, 0));
     strip.setPixelColor(j-1, strip.Color(255, 0, 0));
     strip.setPixelColor(j-2, strip.Color(255, 0, 0));
   }
   strip.show();
   delay(wait);
   RemoteXY_yield();
  }  
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  strip.clear();
  strip.show();
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  strip.clear();
  strip.show();
}


//fill the strip one dot by one, and then switch it off one by one
void onexone(uint8_t wait) {
  strip.clear();
  strip.show();
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(random(0,255)));
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  strip.clear();
  strip.show();
}

//light on and off as a curtain
void curtain(uint8_t wait) {
  strip.clear();
  strip.show();
  for(uint16_t i=0; i<strip.numPixels()/2; i++) {
    strip.setPixelColor(i, Wheel(random(0,255)));
    strip.setPixelColor(strip.numPixels() - i, Wheel(random(0,255)));
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  
  for(uint16_t i=0; i<strip.numPixels()/2; i++) {
    strip.setPixelColor(i, 0);
    strip.setPixelColor(strip.numPixels() - i, 0);
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  strip.clear();
  strip.show();
}


void rainbow(uint8_t wait) {
  uint16_t i, j;
  strip.clear();
  strip.show();
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
  strip.clear();
  strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    RemoteXY_yield();
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  strip.clear();
  strip.show();
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      RemoteXY_yield();
     
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
  strip.clear();
  strip.show();
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      RemoteXY_yield();

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void loop() {
    for(int i = 0; i< 50; i++)
      RemoteXY_Handler (); 
  
      if (RemoteXY.connect_flag)
        last_select = RemoteXY.select_1;
    
      switch(last_select) {        
        case 0:
          luci_off();
          break;
        case 1:
          supercar(20);
          break;
        case 2:
          onexone(20);
          break;
        case 3:
          curtain(20);
          break;
        case 4:
          colorWipe(random_color(),20); // Blue
          break;
        case 5:
          theaterChase(strip.Color(127, 127, 127), 20); // White
          break;
        case 6:
          theaterChase(strip.Color(127, 0, 0), 20); // Red
          break;
        case 7:
          rainbow(3);
          break;
        case 8:
          rainbowCycle(3);
          break;
        case 9:
          theaterChaseRainbow(3);
          break;       
    } 
}
