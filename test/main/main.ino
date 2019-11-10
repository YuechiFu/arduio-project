#include <Wire.h>
#include <rgb_lcd.h>             // 加载lcd 屏幕
#include <Ultrasonic.h>          // 加载超声波传感器库
#include <Adafruit_NeoPixel.h>   // 加载led库
#include <SCoop.h>               // 加载异步SCoop库
#include "KT403A_Player.h"
#ifdef __AVR__
  #include <avr/power.h>
  #include <SoftwareSerial.h>
  SoftwareSerial SSerial(2, 3); // RX, TX
  #define COMSerial SSerial
  #define ShowSerial Serial 
  KT403A<SoftwareSerial> Mp3Player;
#endif


#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define COMSerial Serial1
#define ShowSerial SerialUSB 

KT403A<Uart> Mp3Player;
#endif

#ifdef ARDUINO_ARCH_STM32F4
#define COMSerial Serial
#define ShowSerial SerialUSB 

KT403A<HardwareSerial> Mp3Player;
#endif

static uint8_t recv_cmd[8] = {};








#define RestartBTn        3       // led socket kit         D2 
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

/* 重启函数 */
void(* resetFunc) (void) = 0;

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
// MP3 Print
void printMenu()
{
  Serial.println("Grove - Serial MP3 Demo");
  Serial.println(
        "Input command:\r\n\r\n"
        "P[ ] play music by default index\r\n"
        "Pm[ ] play music in MP3 folder by index\r\n"
        "Pf[ ][ ] play music by specify folder and index\r\n"        
        "p Pause\r\n"
        "R Resume\r\n"
        "N Next\r\n"
        "L Previous\r\n"
        "l[] LoopFolder\r\n"
        "I Increase volume\r\n"
        "D Decrease volumern\r\n"
        "Other keys for print menu\r\n");
    delay(100);
}

// MP3 loop
void mp3Loop(){
    uint8_t len = 0;
    uint8_t i;

    if(Serial.available())
    {
        char chr = '\0';
        while(chr != '\n')  // Blockly read data from serial monitor
        {
            chr = Serial.read();
            // Serial.print(chr);
            recv_cmd[len++] = chr;        
        }
    }

    if(len > 0)
    {
        // Print reveiced data    
        // Serial.print("Received cmd: ");   
        // for(i = 0; i < len; i++) {
        //     Serial.print(recv_cmd[i]);
        //     Serial.print(" ");
        // }
        // Serial.println();
            
        switch (recv_cmd[0])
        {
            case 'P':
                if(recv_cmd[1] == 'm') 
                {
                    /** 
                      * Play music in "MP3" folder by index 
                      * example:
                      * "Pm1" -> ./MP3/0001.mp3
                    */
                    Mp3Player.playSongMP3(recv_cmd[2] - '0');
                    Serial.print("Play ");
                    Serial.write(recv_cmd[2]);
                    Serial.println(".mp3 in MP3 folder");
                } 
                else if(recv_cmd[1] == 'f')
                {
                    /** 
                      * Play specify folder and music
                      * example:
                      * "Pf11" -> ./01/001***.mp3
                    */
                    Mp3Player.playSongSpecify(recv_cmd[2] - '0', recv_cmd[3] - '0');
                    Serial.print("Play ");
                    Serial.write(recv_cmd[3]);
                    Serial.print("xxx.mp3");
                    Serial.print(" in folder ");
                    Serial.write(recv_cmd[2]);
                    Serial.println();
                    
                } 
                else
                {
                    /** 
                      * Play music by default index
                      * example:
                      * "P1" -> ./***.mp3
                    */                
                    Mp3Player.playSongIndex(recv_cmd[1] - '0');
                    Serial.print("Play xxx.MP3 by index ");
                    Serial.write(recv_cmd[1]);
                    Serial.println();
                }            
                // Serial.println("Specify the music index to play");
                break;
            case 'p':
                Mp3Player.pause();            
                Serial.println("Pause the MP3 player");
                break;
            case 'R':            
                Mp3Player.play();
                Serial.println("Resume the MP3 player");
                break;
            case 'N':            
                Mp3Player.next();
                Serial.println("Play the next song");
                break;
            case 'L':
                Mp3Player.previous();
                Serial.println("Play the previous song");
                break;
            case 'l':
                /** 
                      * Play music by default index
                      * example:
                      * "l1" -> l1/
                */                
                Mp3Player.loopFolder(recv_cmd[1]-'0');
                Serial.print("Play loop for all the songs in the floder");
                Serial.write(recv_cmd[1]);
                Serial.println();
                break;
            case 'I':
                Mp3Player.volumeUp();
                Serial.println("Increase volume");
                break;
            case 'D':
                Mp3Player.volumeDown();
                Serial.println("Decrease volume");
                break;
            default:
                printMenu();
                break;
        }

    }    
    delay(200);
}

// MP3 Setup 
void mp3Setup(){
  ShowSerial.begin(9600);
  COMSerial.begin(9600);
  while (!ShowSerial);
  while (!COMSerial);
  Mp3Player.init(COMSerial);
  Mp3Player.play();

  printMenu();
}


/*****************************************************/
/**************Mutiple Process Initial ***************/
/*****************************************************/


// MP3 异步任务



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
    pinMode(RestartBTn, INPUT);
    mp3Setup();
    mySCoop.start();
}

void loop(){
  yield();
  int buttonState = digitalRead(RestartBTn);
    if (buttonState == HIGH) {
        delay(500);
        resetFunc();
    }
    // mp3Loop();
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