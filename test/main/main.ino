#include <Wire.h>
#include <rgb_lcd.h>             // 加载lcd 屏幕
#include <Ultrasonic.h>          // 加载超声波传感器库
#include <Adafruit_NeoPixel.h>   // 加载led库
#ifdef __AVR__
  #include <avr/power.h>
#endif

Ultrasonic ultrasonic(7);
#define LED   2                       // 定义 led socket kit 接口 D2
#define PIXELPIN   6                 // 定义 led RGB stick  接口 D6  
#define InstanceSENSOR  4

rgb_lcd lcd;

//全彩LED的型号和参数
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIXELPIN, NEO_GRB + NEO_KHZ800);




const int activeDistance = 5;
int TIME = 15 ; // 总时间
int limitTime = 15 ; // 倒计时开始时间
int Total = 0 ;
int Score = 0 ; 




/* ********************初始化灯条*/

void initialLedStick(){
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
    strip.setBrightness(128);
    strip.begin(); 
    for(int i=0 ; i<limitTime ; i++){strip.setPixelColor(i,56, 192, 167);}
    strip.show(); // Initialize all strip to 'off'
}

/* ********************灯条亮倒计时*/

void setLedStick(int sec , int R , int G , int B){  
 strip.setPixelColor(sec, strip.Color(R,G,B)); 
 strip.show(); 
 }


 /* *******************倒计时函数*/

void getTime() {  
 if(TIME <= limitTime){
    setLedStick(TIME-1,0,0,0);
    if( TIME == 0 ){
      lcd.setRGB(255,0,0);
      for(int i=0 ; i<15 ; i++){strip.setPixelColor(i,255,0,0);}
      return ;
    } 
  } 
  TIME = TIME - 1;
}


/* ********************距离触发*/

void showDistance(){
    long RangeInCentimeters;
    RangeInCentimeters = ultrasonic.MeasureInCentimeters();
    if(TIME >= 0) getTime();
    if(RangeInCentimeters<20){
        Total = Total + 1;
        if(RangeInCentimeters < activeDistance){
          Score = Score + 1;
          digitalWrite(LED, HIGH);
        }else{digitalWrite(LED, LOW);}
    }else{
      digitalWrite(LED, LOW);
    }
    lcd.clear();
    lcd.setCursor(0,0);  lcd.print("Total:"); lcd.print(Total,DEC);
    lcd.setCursor(0,1);  lcd.print("Score:"); lcd.print(Score,DEC);
    lcd.setCursor(10,1); lcd.print("TIME:");  lcd.print(TIME,DEC);
    delay(1000);
  }


void setup()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(50 ,255,220);
    // pinMode(LED, OUTPUT);
    initialLedStick();
    
    Serial.begin(115200);
    pinMode(InstanceSENSOR,INPUT);

    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    
}

void loop()
{   
    showDistance();
  //  if(TIME >= 0) getTime();
//  short val=0;
//    val=digitalRead(InstanceSENSOR);
//    Serial.print("val=");
//    Serial.println((int)val);
//    if(0==val)
//    {
//        Serial.println("Sensor is triggered!!");
//    }
//    delay(50);

  int sensorState = digitalRead(A0);
    Serial.println(sensorState);
    delay(100);
    if(sensorState == HIGH)
    {
        digitalWrite(LED,HIGH);
    }
    else
    {
        digitalWrite(LED,LOW);
    }
        
}

/***
 **
 *

lcd.clear();                            // 清屏
lcd.setRGB();                           // 设置背景色
 
digitalWrite(LED, LOW);                 // 关闭小灯泡
digitalWrite(LED, HIGH);                // 打开小灯泡

strip.clear();                          // 重置 led stick
strip.setPixelColor(n,r,g,b)            // 设置 led stick 亮灯位置 以及 颜色
strip.setBrightness(64);                // 设置亮度
strip.begin();                          // This initializes the NeoPixel library.
strip.show();                           // 让led stick 根据载入配置 显示

ultrasonic.MeasureInCentimeters();      // 读取 超声波的测量距离 cm；

*/


/* #include "SCoop.h"
//完整定义
defineTask(TaskTest);
void TaskTest::setup()
{
  pinMode(13, OUTPUT);
}
void TaskTest::loop()
{
  digitalWrite(13, HIGH);
  sleep(1000);
  digitalWrite(13, LOW);
  sleep(1000);
}

//快速定义
defineTaskLoop(TaskTest1)
{
  digitalWrite(12, HIGH);
  sleep(100);
  digitalWrite(12, LOW);
  sleep(100);
}

void setup() {
  pinMode(12, OUTPUT);
  mySCoop.start();
}

void loop()
{
  yield();
}
 */