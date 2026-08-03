#define ESP32_WIFI_TOUCH

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "pindefinitions.h"
#include "functions.h"
#include <TFT_eSPI.h>
#include "RTClib.h"
#include <Wire.h>
#include <TouchScreen.h>
#include <Adafruit_BMP280.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "MQ2.h" // library: https://github.com/labay11/MQ-2-sensor-library <- Thank you so much!
#include "Buttons.h" //the button code that would mess up the main code
#include <JPEGDEC.h>
#include <Adafruit_ADS1X15.h>
#include <lvgl.h>
#include <FastLED.h>
#include <Adafruit_NeoPixel.h>



Adafruit_BMP280 bmp; // use I2C interface
MQ2 mq2(MQ2_SENSOR_PIN);
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
//CRGB leds[NUMBER_LEDS];
RTC_DS3231 rtc;
JPEGDEC jpeg;
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
int16_t adc1;

class selfCheckRoutine {
  String info;
  char Result[16];
  int row = 0;

  public:

  void completeSelfCheck(){
    Serial.println(checkDisplay());
    delay(100);
    Serial.println(checkI2C());
    delay(100);
    Serial.println(checkMQ2());
    delay(100);
    Serial.println(checkADS1());
    delay(100);
    Serial.println(checkADS2());
    delay(100);
    Serial.println(checkRTC());
    delay(100);
    Serial.println(checkSD());
    delay(100);
    Serial.println(checkLEDs());
    delay(1000);
  }

  void selfCheckInfo(String info){
    tft.setTextColor(BLUE, TFT_BLACK);
    tft.drawString(info , MARGIN_LEFT, linespace*row);
  }

  void selfCheckPositive() {
    tft.setTextColor(GREEN, TFT_BLACK);
    tft.drawString("[OK]", MARGIN_LEFT + info.length() *6.5, linespace*row);
  }

  void selfCheckNegative(int Errorcode) {
    tft.setTextColor(RED, TFT_BLACK);
    snprintf(Result, sizeof(Result), "[FAILED] %04d", Errorcode);
    tft.drawString(Result, info.length() * 6, row*linespace);
  }


  int checkDisplay() {
    info = "[INFO ] Initializing Display...";
    try{
      tft.begin(); // start Display
      tft.setRotation(1);
      tft.fillScreen(TFT_BLACK);
      tft.setSwapBytes(true);
      tft.setTextFont(2);
      tft.setTextSize(1);

      selfCheckInfo(info);
      selfCheckPositive();

      row++;
      return 0;
    }
    catch(int Errorcode) {
      //
      return Errorcode;
    }
  }

  int checkI2C() {
    info = "[INFO] Starting I2C communication... ";
    selfCheckInfo(info);
    if (!Wire.begin()){
      selfCheckNegative(101);
      row++;
      return 101;
    } 
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkMQ2() {
    info = "[INFO] Starting MQ2 communication... ";
    selfCheckInfo(info);
    mq2.begin(); // no if or error possible in this library...
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkADS1() {
    info = "[INFO] Starting ADC1 communication... ";
    selfCheckInfo(info);
    if (!ads1.begin(ADDR_ADC1)) {
      
      selfCheckNegative(103);
      row++;
      return 103;
    }
    ads1.setGain(GAIN_ONE);
    selfCheckPositive();
    row++;
    return 0;
  }
  
  int checkADS2() {
    info = "[INFO] Statring ADC2 communication... ";
    selfCheckInfo(info);
    if (!ads2.begin(ADDR_ADC2)) {
      selfCheckNegative(104);
      row++;
      return 104;
    }
    ads2.setGain(GAIN_ONE);
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkRTC() {
    info = "[INFO] Starting RTC communication... ";
    selfCheckInfo(info);
    if (!rtc.begin()) {
      selfCheckNegative(105);
      row++;
      return 105;
    } 
    setTime();
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkSD() {
    info = "[INFO] Initializing SD card...";
    selfCheckInfo(info);
    if (!SD.begin(CS_SD)) {
      selfCheckNegative(106);
      row++;
      return 106;
    } 
    const bool sdCardReady = initSDCard();
    if (sdCardReady) {
      testSDCardCommunication();
    } 
    else {
      selfCheckNegative(107);
      row++;
      return 107;
    }
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkLEDs() {
    info = "[INFO] Initializing LEDs...";
    selfCheckInfo(info);
    if (!leds.begin()){
      selfCheckNegative(107);
      row++;
      return 107;
    }
    leds.setPin(LED_PIN);
    leds.clear();
    leds.setPixelColor(1,200,200,200);
    leds.show();
    delay(1000);
    leds.clear();
    leds.show();

    selfCheckPositive();
    row++;
    return 0;
  }
};


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");

  // ---------------------- Initialization prozess ----------------------

  selfCheckRoutine selfCheckRoutine;
  selfCheckRoutine.completeSelfCheck();
  
  // -------------------------- GPIO initialization --------------------------
  pinMode(BUTTONS, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MQ2_SENSOR_PIN, INPUT);
  analogReadResolution(12);  

  jpeg.setPixelType(RGB565_BIG_ENDIAN);

  attachInterrupt( // Start Button Interrupt
        digitalPinToInterrupt(BUTTONS),
        onButtonChange,
        CHANGE
    );

  // clear screen
  //tft.fillScreen(TFT_BLACK);
}

// ***************** LEDs still don't work... *****************

void loop() {
  char data[64];
  int sen1 = ads1.readADC_SingleEnded(0);
  int sen2 = ads1.readADC_SingleEnded(1);
  int sen3 = ads1.readADC_SingleEnded(2);
  int sen4 = ads1.readADC_SingleEnded(3);
  int sen5 = ads2.readADC_SingleEnded(0);
  int sen6 = ads2.readADC_SingleEnded(1);
  int sen7 = ads2.readADC_SingleEnded(2);
  int sen8 = ads2.readADC_SingleEnded(3);

  snprintf(data, sizeof(data), "S1: %d", sen1);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString(data, MARGIN_LEFT, 10);
}
