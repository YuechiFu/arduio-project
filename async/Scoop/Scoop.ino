#include "SCoop.h"
#include <Adafruit_NeoPixel.h>   // 加载led库
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIXELPIN 6
#define LED 2 

//全彩LED的型号和参数
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIXELPIN, NEO_GRB + NEO_KHZ800);


defineTask(Task1);

int index = 0;

void initialLedStick(){
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
    strip.setBrightness(128);
    strip.begin(); 
    strip.show(); // Initialize all strip to 'off'
}

 void Task1::setup()
 {
   pinMode(LED, OUTPUT);
 }
 void Task1::loop()
 {
   digitalWrite(LED, HIGH);
   sleep(500);
   digitalWrite(LED, LOW);
   sleep(500);
 }
//快速定义

 defineTaskLoop(Task2)
 {
  strip.clear();
  if(index > 14){
    index = 0 ;
  }
  strip.setPixelColor(index, strip.Color(index*18,index*8,154-index*14)); 
  strip.show(); 
  index++;
  delay(100);
 }
void setup() {
   mySCoop.start();
   pinMode(LED, OUTPUT);
   initialLedStick();
 }
void loop()
 {
   yield();
 }