#include <Arduino.h>
#include <Wire.h>
// Repository specific headers
#include "pindefinitions.h"
#include "functions.h"
// SD-Card
#include <SPI.h>
#include <SD.h>
#include "FS.h"
// Display
#include <TFT_eSPI.h>
#include <TouchScreen.h>
#include <JPEGDEC.h>
#include <lvgl.h>
//Sensors
#include <RTClib.h>
#include <Adafruit_BMP280.h>
#include "MQ2.h" // library: https://github.com/labay11/MQ-2-sensor-library <- Thank you so much!
#include <Adafruit_ADS1X15.h>
//LEDs
#include <Adafruit_NeoPixel.h>

extern Adafruit_BMP280 bmp; // use I2C interface
extern MQ2 mq2;
extern TFT_eSPI tft;
extern Adafruit_NeoPixel leds;
extern RTC_DS3231 rtc;
extern JPEGDEC jpeg;
extern Adafruit_ADS1115 adc1;
extern Adafruit_ADS1115 adc2;



void SelfCheckRoutine::completeSelfCheck() {
    Serial.println(checkDisplay());
    delay(100);
    Serial.println(checkI2C());
    //delay(100);
    //Serial.println(checkMQ2());
    delay(100);
    Serial.println(checkADC1());
    delay(100);
    Serial.println(checkADC2());
    delay(100);
    Serial.println(checkRTC());
    delay(100);
    Serial.println(checkSD());
    delay(100);
    Serial.println(checkLEDs());
    delay(100);
    Serial.println(checkBMP());
    delay(1000);
  }

void SelfCheckRoutine::selfCheck_wo_I2C() {
  Serial.println(checkDisplay());
  delay(100);
  Serial.println(checkSD());
  delay(100);
  Serial.println(checkLEDs());
  delay(1000);
}

void SelfCheckRoutine::selfCheckInfo(String info) {
    tft.setTextColor(BLUE, TFT_BLACK);
    tft.drawString(info , MARGIN_LEFT, linespace*row);
  }

void SelfCheckRoutine::selfCheckPositive() {
    tft.setTextColor(GREEN, TFT_BLACK);
    tft.drawString("[OK]", MARGIN_LEFT + info.length() *6.5, linespace*row);
  }

void SelfCheckRoutine::selfCheckNegative(int Errorcode) {
    tft.setTextColor(RED, TFT_BLACK);
    snprintf(Result, sizeof(Result), "[FAILED] %04d", Errorcode);
    tft.drawString(Result, info.length() * 6, row*linespace);
  }


int SelfCheckRoutine::checkDisplay() {
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

int SelfCheckRoutine::checkI2C() {
    info = "[INFO] Starting I2C communication... ";
    selfCheckInfo(info);

    bool started = Wire.begin();

    if(!started) {
      selfCheckNegative(101);
      row++;
      return 101;
    }

    Wire.setClock(100000);
    Wire.setTimeOut(20);

    bool adc1Found = pingI2C(ADDR_ADC1);
    bool adc2Found = pingI2C(ADDR_ADC2);
    bool rtcFound  = pingI2C(ADDR_RTC);

    Serial.printf(
        "ADC1 0x%02X: %s\n",
        ADDR_ADC1,
        adc1Found ? "OK" : "FAILED"
    );

    Serial.printf(
        "ADC2 0x%02X: %s\n",
        ADDR_ADC2,
        adc2Found ? "OK" : "FAILED"
    );

    Serial.printf(
        "RTC  0x%02X: %s\n",
        ADDR_RTC,
        rtcFound ? "OK" : "FAILED"
    );

    if (!adc1Found || !adc2Found || !rtcFound) {
        selfCheckNegative(101);
        row++;
        return 101;
    }
    

    Serial.printf("Wire.begin(): %s\n", started ? "OK" : "FAILED");
    Serial.printf("SDA=%d SCL=%d\n", SDA, SCL);
    Serial.printf("Clock=%lu\n", Wire.getClock());
    Serial.printf("Timeout=%u\n\n", Wire.getTimeOut());

    testAddress(0x48);
    testAddress(0x49);
    testAddress(0x50);
    testAddress(0x58);
    testAddress(0x68);
    
    selfCheckPositive();
    row++;
    return 0;
  }

int SelfCheckRoutine::checkMQ2() {
    info = "[INFO] Starting MQ2 communication... ";
    selfCheckInfo(info);
    mq2.begin(); // no if or error possible in this library...
    selfCheckPositive();
    row++;
    return 0;
  }

int SelfCheckRoutine::checkADC1() {
    info = "[INFO] Starting ADC1 communication... ";
    selfCheckInfo(info);
    Serial.printf("Ping 0x48 vor ads1.begin(): %s\n", pingI2C(0x48) ? "OK" : "FAILED");
    if (!adc1.begin(ADDR_ADC1, &Wire)) {
      
      selfCheckNegative(103);
      row++;
      return 103;
    }
    adc1.setGain(GAIN_ONE);
    selfCheckPositive();
    row++;
    return 0;
  }
  
int SelfCheckRoutine::checkADC2() {
    info = "[INFO] Statring ADC2 communication... ";
    selfCheckInfo(info);
    Serial.printf("Ping 0x49 vor ads2.begin(): %s\n", pingI2C(0x49) ? "OK" : "FAILED");
    if (!adc2.begin(ADDR_ADC2, &Wire)) {
      selfCheckNegative(104);
      row++;
      return 104;
    }
    adc2.setGain(GAIN_ONE);
    selfCheckPositive();
    row++;
    return 0;
  }

int SelfCheckRoutine::checkRTC() {
    info = "[INFO] Starting RTC communication... ";
    selfCheckInfo(info);
    Serial.printf("Ping 0x68 vor rtc.begin(): %s\n", pingI2C(0x68) ? "OK" : "FAILED");
    if (!rtc.begin(&Wire)) {
      selfCheckNegative(105);
      row++;
      return 105;
    } 
    setTime();
    selfCheckPositive();
    row++;
    return 0;
  }

int SelfCheckRoutine::checkSD() {
    info = "[INFO] Initializing SD card...";
    selfCheckInfo(info);

    const bool sdCardReady = initSDCard();
    
    if (!sdCardReady) {
      selfCheckNegative(106);
      row++;
      return 106;
    } 
    
    const bool communicationOK = testSDCardCommunication();

    if(!communicationOK) {
      selfCheckNegative(107);
      row++;
      return 107;
    }
    
    selfCheckPositive();
    row++;
    return 0;
  }

int SelfCheckRoutine::checkLEDs() {
    info = "[INFO] Initializing LEDs...";
    selfCheckInfo(info);
    leds.begin();
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

int SelfCheckRoutine::checkBMP() {
  info = "[INFO] Initializing BMP280...";
  selfCheckInfo(info);
  if (!bmp.begin(ADDR_BMP280)){
    selfCheckNegative(109);
    row++;
    return 109;
  }
  selfCheckPositive();
  row++;
  return 0;
}

