/*///////////////////////////////////////////////////////////////////////////
Aruduino Vending Machine 2020
For Arduino Mega 2560

Adafruit GFX Library: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf
///////////////////////////////////////////////////////////////////////////*/

//////////////////////////////////////
//  ライブラリ読み込み
//////////////////////////////////////

#include <stdint.h>
#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <FastLED.h>
#include <SD.h>
#include <Servo.h>

//////////////////////////////////////
//  マクロ定義
//////////////////////////////////////

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define YP A2
#define XM A3
#define YM 22
#define XP 23

#define TS_MINX 170
#define TS_MAXX 940
#define TS_MINY 170
#define TS_MAXY 910

#define BLACK   0x0000
#define GRAY    0x39E7
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define NUM_LEDS 85
#define LED_DATA_PIN 4

//コインセレクター
const byte coinPin = 2;

//硬貨判別関係
int coincheck = 0;
int coincheck_pass = 0;
int coinValue = 0;
int coin_sum = 0;
int coin = 0;

int button_num = 0;

int price = 0;

int random_num = 0;

int cancel_Sw = 0;

String goodsname_1;
String goodsname_2;
String goodsname_3;
String goodsname_4;

String goodsprice_1;
String goodsprice_2;
String goodsprice_3;
String goodsprice_4;

int testmode_sw = 0;
int rewind_sw = 0;

int i;

Servo refund_servo;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

CRGB leds[NUM_LEDS];

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//////////////////////////////////////
//  画面初期化
//////////////////////////////////////
void clear_LCD(){
    tft.fillScreen(BLACK);      
}

//////////////////////////////////////
//  最初画面に初期化
//////////////////////////////////////
void screen_init(){
  
  tft.setTextColor(GREEN);
  tft.setTextSize(2);

  if(testmode_sw == 0){
    random_num = random(4);
    if (random_num == 0){
      tft.setCursor(73,40);
      tft.print("HELLO :)");
    }else if (random_num == 1){
      tft.setCursor(60,40);
      tft.print("What's up?");
    }else if (random_num == 2){
      tft.setCursor(85,40);
      tft.print("Howdy!");
    }else if (random_num == 3){
      tft.setCursor(85,40);
      tft.print("Hi! :)");
    }
  }else{
    tft.setCursor(62,40);
    tft.print("TEST_MODE");
  }
  
  if(testmode_sw == 0){
    tft.setTextSize(1);
    tft.setCursor(63,285);
    tft.print("Please select number");
  }else{
    tft.setTextSize(2);
    tft.setCursor(85,286);
    tft.setTextColor(GRAY);
    tft.print("Rewind");
  }
  
  tft.fillRect(20,100,90,70,WHITE);
  tft.fillRect(130,100,90,70,WHITE);
  tft.fillRect(20,190,90,70,WHITE);
  tft.fillRect(130,190,90,70,WHITE);

  tft.drawRect(22,102,86,66,BLACK);
  tft.drawRect(132,102,86,66,BLACK);
  tft.drawRect(22,192,86,66,BLACK);
  tft.drawRect(132,192,86,66,BLACK);

  tft.setCursor(53,117);
  tft.setTextColor(BLACK);
  tft.setTextSize(5);
  tft.print("1");

  tft.setCursor(163,117);
  tft.print("2");

  tft.setCursor(53,207);
  tft.print("3");
  
  tft.setCursor(163,207);
  tft.print("4");
}

//////////////////////////////////////
//  商品選択のタッチ検出
//
//  "button_num" にタップされたボタン番号を記録する
//  テストモードが有効な場合、ボタン番号と同じサーボを回す
//////////////////////////////////////
void touch_detection(){
  
  if (button_num == 0){
    while(1){
      TSPoint p = ts.getPoint();
    if (p.z > ts.pressureThreshhold) {

     p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
     p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         
     if(p.x>119 && p.x<163 && p.y>148 && p.y<290)
     {
      Serial.print("push: 1\n");
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      
      if(testmode_sw == 1){
        if(rewind_sw == 0){
          pwm.writeMicroseconds(0, 3000);
          delay(1590);
          pwm.writeMicroseconds(0, 1500);
        }else{
          pwm.writeMicroseconds(0, 100);
          delay(1590);
          pwm.writeMicroseconds(0, 1500);
        }
      }else{
        clear_LCD();
      }

      button_num = 1;
      break;
   
     }
     
     if(p.x>119 && p.x<163 && p.y>0 && p.y<123)
     {
      Serial.print("push: 2\n");
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      //clear_LCD();

      if(testmode_sw == 1){
        if(rewind_sw == 0){
          pwm.writeMicroseconds(1, 3000);
          delay(1590);
          pwm.writeMicroseconds(1, 1500);
        }else{
          pwm.writeMicroseconds(1, 100);
          delay(1590);
          pwm.writeMicroseconds(1, 1500);
        }
      }else{
        clear_LCD();
      }

      button_num = 2;
      break;
  
     }

     if(p.x>43 && p.x<102 && p.y>148 && p.y<290)
     {
      Serial.print("push: 3\n");
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      //clear_LCD();

      if(testmode_sw == 1){
        if(rewind_sw == 0){
          pwm.writeMicroseconds(2, 3000);
          delay(1590);
          pwm.writeMicroseconds(2, 1500);
        }else{
          pwm.writeMicroseconds(2, 100);
          delay(1590);
          pwm.writeMicroseconds(2, 1500);
        }
      }else{
        clear_LCD();
      }

      button_num = 3;
      break;

     }
     
     if(p.x>43 && p.x<102 && p.y>0 && p.y<123)
     {
      Serial.print("push: 4\n");
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      //clear_LCD();

      if(testmode_sw == 1){
        if(rewind_sw == 0){
          pwm.writeMicroseconds(3, 3000);
          delay(1590);
          pwm.writeMicroseconds(3, 1500);
        }else{
          pwm.writeMicroseconds(3, 100);
          delay(1590);
          pwm.writeMicroseconds(3, 1500);
        }
      }else{
        clear_LCD();
      }

      button_num = 4;
      break;
   
     }
     if(p.x>12 && p.x<30 && p.y>97 && p.y<239 && testmode_sw == 1)
        {
          Serial.print("rewind\n");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);

          if(rewind_sw == 0){
            tft.setTextSize(2);
            tft.setCursor(85,286);
            tft.setTextColor(GREEN);
            tft.print("Rewind");
            rewind_sw = 1;
          }else{
            tft.setTextSize(2);
            tft.setCursor(85,286);
            tft.setTextColor(GRAY);
            tft.print("Rewind");
            rewind_sw = 0;
          }
          break;
   
        }
      }
    }
  }
}

//////////////////////////////////////
//  硬貨判別
//  
//  1 = 1円
//  2 = 20円
//  3 = 100円
//  4 = 500円
//
//  パルスが送られてきてから50ms待ち、送られてこなくなるまで
//  繰り返し処理をする
//////////////////////////////////////
void coincheck_loop(){
    while(1){
      
      TSPoint p = ts.getPoint();
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      
      if (p.z > ts.pressureThreshhold) {
        p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
        p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));

        if(p.x>12 && p.x<30 && p.y>97 && p.y<239)
        {
          Serial.print("cancel\n");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          
          cancel_Sw = 1;
          break;
   
        }
      }
        

    static unsigned long prevCoinMillis = 0;
    
    bool coinInserted = false;

    unsigned long currentMillis = millis();
    if((currentMillis - prevCoinMillis >= 50) && (digitalRead(coinPin) == LOW))
    {
        prevCoinMillis = currentMillis;
        
        coinInserted = true;
        ++coinValue;
        coincheck_pass=1;
        //Serial.println("1: "+String(coinValue));
    }

    if ((currentMillis - prevCoinMillis >= 50) && (digitalRead(coinPin) == HIGH) && (coincheck_pass==1)){
      ++coincheck;
      delay(10);
    }

    if ((coincheck > coinValue ) && (coincheck > 40)){
       if (coinValue == 4){
        coin = 500;
        coin_sum += coin;
       } else if (coinValue == 3){
        coin = 100;
        coin_sum += coin;
       } else if (coinValue == 2){
        coin = 10;
        coin_sum += coin;
       } else if (coinValue == 1){
        coin = 1;
        coin_sum += coin;
       }
       
       coinValue = 0;
       coincheck = 0;
       coincheck_pass = 0;

       tft.fillRect(25,235,100,40,BLACK);

       tft.setTextColor(WHITE);
       tft.setTextSize(3);
       tft.setCursor(25,235);
       tft.print(String(coin_sum));

       break;
    }
    }
}
//////////////////////////////////////
//  購入後の画面
//////////////////////////////////////
void thankyou(){
  
  clear_LCD();
  tft.setCursor(25,130);
  tft.setTextColor(GREEN);
  tft.setTextSize(3);
  tft.print("THANK YOU!!");

  tft.setCursor(43,165);
  tft.setTextSize(1);
  tft.print("Please pick up your items.");
  
}

//////////////////////////////////////
//  SDカード読み取り
//
//  SDカードから商品名、値段を読み出し変数に格納していく
//  商品名、値段は "," で区切る必要あり
//////////////////////////////////////
void SD_read(){
    
    int i;

    i = 0;
    
    if(SD.begin()){
    File file = SD.open("GOODS.TXT", FILE_READ);
  
    if (file) {
      
      while (file.available()) {
        
        int val =file.read();
        
        if (val == 44){
          i++;
        }else{
          if (i == 0){
            goodsname_1 += char(val);
          }else if (i == 1){
            goodsprice_1 += char(val);
          }else if (i == 2){
            goodsname_2 += char(val);
          }else if (i == 3){
            goodsprice_2 += char(val);
          }else if (i == 4){
            goodsname_3 += char(val);
          }else if (i == 5){
            goodsprice_3 += char(val);
          }else if (i == 6){
            goodsname_4 += char(val);
          }else if (i == 7){
            goodsprice_4 += char(val);
          }
        }
      }
      file.close();
    }else{
      //SDカードオープンエラー
      clear_LCD();
      tft.setCursor(0,0);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("File Open Error\nPlease Reset Arduino");
      while(1){
      }
    }
  }else{
    //SDカードファイルオープンエラー
    clear_LCD();
    tft.setCursor(0,0);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.print("SD Card Open Error\nPlease Reset Arduino");
    while(1){
    }
  }
}

//////////////////////////////////////
//  テストモード用タッチ検出
//
//  約5秒間待ちタップされた場合、テストモードを有効にする
//////////////////////////////////////
void testmode_tapcheck(){
  int tci;
  int cp=1;

  while(1){
    for(tci=3000;tci>=0;tci--){
      if(tci%1000==0){
        tft.setCursor(0,17*cp);
        tft.print(String(tci/1000));
        cp++;
      }
      delay(1);
      TSPoint p = ts.getPoint();
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      if (p.z > ts.pressureThreshhold) {
        testmode_sw = 1;
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        break;
      }
    }
    break;
  }
}

//////////////////////////////////////
//  セットアップ
//////////////////////////////////////
void setup() {

  pinMode(coinPin,INPUT_PULLUP);

  pwm.begin();

  Serial.begin(9600);

  pwm.setOscillatorFrequency(26600000);
  pwm.setPWMFreq(50);

  tft.reset();
  tft.begin(0x9341);

  randomSeed(analogRead(7));

  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);

  SD_read();

  for(i=0;i<85;i++){
    leds[i] = CRGB::White;
  }
  FastLED.show();

  refund_servo.attach(3);
  refund_servo.write(90);

  clear_LCD();

  tft.setCursor(0,0);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("If you want to use \"TEST_MODE\"\nTap on the display.");
  testmode_tapcheck();
}

//////////////////////////////////////
//  メインループ
//////////////////////////////////////
void loop(){
  
  clear_LCD();
  screen_init();

  if(testmode_sw == 0){
    touch_detection();
  }else{
    while(1){
      touch_detection();
      button_num = 0;
      delay(200);
    }
  }

  clear_LCD();

  tft.drawRect(87,20,30,40,WHITE);
  tft.drawRect(122,20,30,40,WHITE);
  tft.drawRect(87,63,30,40,WHITE);
  tft.drawRect(122,63,30,40,WHITE);

  tft.setCursor(25,130);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Please insert coin");
  
  tft.setCursor(25,215);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("Total Amount:");
  
  tft.setCursor(25,157);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);

  if (button_num == 1){
    tft.print(goodsname_1);
  }else if (button_num == 2){
    tft.print(goodsname_2);
  }else if (button_num == 3){
    tft.print(goodsname_3);
  }else if (button_num == 4){
    tft.print(goodsname_4);
  }
  
  tft.setCursor(25,180);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
   if (button_num == 1){
    tft.print(String(goodsprice_1)+" YEN");
    tft.fillRect(88,21,28,38,GREEN);
    price = goodsprice_1.toInt();
  }else if (button_num == 2){
    tft.print(String(goodsprice_2)+" YEN");
    tft.fillRect(123,21,28,38,GREEN);
    price = goodsprice_2.toInt();
  }else if (button_num == 3){
    tft.print(String(goodsprice_3)+" YEN");
    tft.fillRect(88,64,28,38,GREEN);
    price = goodsprice_3.toInt();
  }else if (button_num == 4){
    tft.print(String(goodsprice_4)+" YEN");
    tft.fillRect(123,64,28,38,GREEN);
    price = goodsprice_4.toInt();
  }

  tft.setCursor(130,235);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("YEN");
  tft.setCursor(25,235);
  tft.print("0");

  tft.fillRect(70,280,100,25,WHITE);
  tft.drawRect(72,282,96,21,BLACK);

  tft.setCursor(85,286);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Cancel");

  while(1){
    while(coin_sum < price){
      if (cancel_Sw == 1){
        break;
      }
      coincheck_loop();
    }
    if(coin_sum > price){
      refund_servo.write(60);
      tft.fillRect(25,235,160,40,BLACK);
      tft.setTextColor(RED);
      tft.setTextSize(2);
      tft.setCursor(25,240);
      tft.print("Too Much Pay!");
      for(i=0;i<85;i++){
        leds[i] = CRGB::Red;
      }
      FastLED.show();
      delay(2000);
      tft.fillRect(25,235,160,40,BLACK);
      tft.setCursor(130,235);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.print("YEN");
      tft.setCursor(25,235);
      tft.print("0");
      refund_servo.write(90);
      for(i=0;i<85;i++){
        leds[i] = CRGB::White;
      }
      FastLED.show();
      coin_sum = 0;
    }else{
      break;
    }
    
  }
  
  if (cancel_Sw == 0){
    
    clear_LCD();

    thankyou();

    pwm.writeMicroseconds(button_num-1, 3000);
    delay(1590);
    pwm.writeMicroseconds(button_num-1, 1500);

    delay(1000);
    
  }
  button_num = 0;
  coin_sum = 0;
  price = 0;

  cancel_Sw = 0;
}
