#include <Adafruit_NeoPixel.h>

#define PIXELPIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIXELPIN, NEO_GRB + NEO_KHZ800);

uint8_t brightness = 0; //LED的亮度

uint8_t fadeAmount = 1; //亮度变化增量

void setup() {

  strip.begin();

}

void loop() {

////  strip.setPixelColor(0, 0, brightness, 0);
//  strip.setPixelColor(1, 0, brightness, brightness);
//  strip.setPixelColor(2, brightness, brightness, 0);
//  strip.setPixelColor(3, brightness, brightness, 0);
  strip.setPixelColor(8, brightness, brightness, 0);
  strip.setPixelColor(9, brightness, brightness, 0);
  strip.setPixelColor(10, brightness, brightness, 0);
  strip.setPixelColor(14, brightness, brightness, 0);
  
  strip.show();

  brightness = brightness + fadeAmount;

  if (brightness <= 0 || brightness >= 64){
    fadeAmount = -fadeAmount ; //亮度翻转
    }

    

delay(20);

}
