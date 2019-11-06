#include <Wire.h>
#include <rgb_lcd.h>             // 加载lcd 屏幕
#include <Ultrasonic.h>          // 加载超声波传感器库
#include <Adafruit_NeoPixel.h>   // 加载led库
#include <TimerDown.h>           // 加载异步TimerDown库
#include <TimerUp.h>             // 加载异步TimerUp库
// #include <SchedulerARMAVR.h>     // 加载异步SchedulerARMAVR库
#include <IOFilter.h>            // 加载IOFilterp库
#include <SCoop.h>               // 加载异步SCoop库
#ifdef __AVR__
  #include <avr/power.h>
#endif
            
#define BtnAction         2       // led socket kit         D2 
#define LED               4       // led socket kit         D4 
#define UltrasonicSENSOR  5       // 超声波传感器              D5
#define CollisionSENSOR   6       // 碰撞传感器               D6
#define PIXELPIN          8       // led RGB stick            D8 
    

rgb_lcd lcd;
Ultrasonic ultrasonic(UltrasonicSENSOR); 

//全彩LED的型号和参数
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, PIXELPIN, NEO_GRB + NEO_KHZ800);

// 初始化变量
const int checkDistance  = 20 ;
const int activeDistance = 8;
int TIME = 15 ; // 总时间
int limitTime = 15 ; // 倒计时开始时间
int Total = 0 ;
int Score = 0 ; 
long Distance = 300 ;
boolean Shake = false ;
boolean IsShoot = false ;
boolean IsScore = false ;



/*****************************************************/
/************** Methods VOID  ***************/
/*****************************************************/

/*初始化灯条*/

void initialLedStick(){
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
    strip.setBrightness(128);
    strip.begin(); 
    for(int i=0 ; i<limitTime ; i++){strip.setPixelColor(i,56, 192, 167);}
    strip.show(); // Initialize all strip to 'off'
}

/*灯条亮倒计时*/

void setLedStick(int sec , int R , int G , int B){  
 strip.setPixelColor(sec, strip.Color(R,G,B)); 
 strip.show(); 
 }


 /*倒计时函数*/

void getTime() {  
 if(TIME < limitTime){
    setLedStick(TIME,0,0,0);
    lcd.setCursor(9,1); lcd.print("TIME:");  lcd.print(TIME,DEC);
    if( TIME == 0 ){
      lcd.setRGB(255,0,0);
      for(int i=0 ; i<15 ; i++){strip.setPixelColor(i,255,0,0);}
      return ;
    } 
  } 
  TIME = TIME - 1;
}


/*超声波距离检测****/

void showDistance(){
    Distance = ultrasonic.MeasureInCentimeters();
  }

/*震动检测 ****/
boolean collisionTriggered(){
   if(!digitalRead(CollisionSENSOR))
    {
      if(!digitalRead(CollisionSENSOR))
        return true;//the collision sensor triggers
    }
    return false;
}

boolean checkScore(){

}

/*****************************************************/
/**************Mutiple Process Initial ***************/
/*****************************************************/

// 倒计时异步任务 

defineTask(TaskTimeTip);
 void TaskTimeTip::setup()
 {
  
 }
 void TaskTimeTip::loop()
 {
  if(TIME >= 0) getTime();
  sleep(1000);
 }

// 超声波测距异步任务 
defineTask(TaskDistance);
 void TaskDistance::setup()
 {
  
 }
 void TaskDistance::loop()
 {
    showDistance();
    if(!IsShoot && Distance < checkDistance ){
      Total = Total + 1;
      lcd.setCursor(0,0);  lcd.print("Total:"); lcd.print(Total,DEC);
    }
    if(Distance < activeDistance && !IsScore){
        Score = Score + 1;
        digitalWrite(LED, HIGH);
        IsScore = true ; 
        lcd.setCursor(0,1);  lcd.print("Score:"); lcd.print(Score,DEC);
        sleep(2000);
        IsScore = false ; 
    }else{
      digitalWrite(LED, LOW);
        IsScore = false ; 
    }
    sleep(200);
    
 }





// 碰撞传感器检测异步任务 

defineTask(TaskCollisionCheck);
void TaskCollisionCheck::setup(){
}
void TaskCollisionCheck::loop(){
  // Serial.println("checking....");
  if(collisionTriggered()){
    digitalWrite(LED, HIGH);           // 打开小灯泡
    Shake = true ; 
    Total = Total + 1 ;
    IsShoot = true ;
    lcd.setCursor(0,0);  lcd.print("Total:"); lcd.print(Total,DEC);
    sleep(2000);
  }else{
    digitalWrite(LED, LOW); 
    Shake = false ; 
    IsShoot = false ;
  } 
}

// 全局初始化

void setup()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(50 ,255,220);
    initialLedStick();
    pinMode(LED, OUTPUT);
    pinMode(CollisionSENSOR,INPUT);
    mySCoop.start();
    
    
}

void loop(){yield();}

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

//*************  近距离感应器检测异步任务 ******
/* defineTask(TaskMiniDistance);
void TaskMiniDistance::setup(){
  Serial.begin(115200);
  pinMode(InstanceSENSOR,INPUT);
}
void TaskMiniDistance::loop(){
    short val=0;
    val=digitalRead(InstanceSENSOR);
    Serial.print("val=");
    Serial.println((int)val);
    if(0==val)
    {
        Serial.println("Sensor is triggered!!");
        sleep(2000);
    }
    sleep(100);
} */

/* ********************压电振动检测*****/

/* void piezoVibra(){
   int sensorState = digitalRead(A0);
    Serial.println(sensorState);
    delay(500);
    if(sensorState == HIGH)
    {digitalWrite(LED,HIGH);}
    else
    { digitalWrite(LED,LOW);}
} */