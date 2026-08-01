#define ESP32_WIFI_TOUCH

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "pindefinitions.h"
#include "functions.h"
#include <TFT_eSPI.h>
#include <Adafruit_NeoPixel.h>
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



Adafruit_BMP280 bmp; // use I2C interface
MQ2 mq2(MQ2_SENSOR_PIN);
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
RTC_DS3231 rtc;
JPEGDEC jpeg;
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
int16_t adc1;

class selfCheckRoutine {
  String info;
  char Result[16];
  int row;

  public:

  void completeSelfCheck(){
    //
  }

  void selfCheckInfo(String info){
    tft.setTextColor(BLUE, TFT_BLACK);
    tft.drawString(info , MARGIN_LEFT, linespace*row);
  }

  void selfCheckPositive() {
    tft.setTextColor(GREEN, TFT_BLACK);
    tft.drawString("[OK]", MARGIN_LEFT + info.length(), linespace*row);
  }

  void selfCheckNegative(int Errorcode) {
    tft.setTextColor(RED, TFT_BLACK);
    snprintf(Result, sizeof(Result), "[FAILED] %06d", Errorcode);
    tft.drawString(Result, info.length() * 6, row);
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
    try {
      Wire.begin();
      selfCheckPositive();
      row++;
      return 0;
    } 
    catch(int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }
  }

  int checkMQ2() {
    info = "[INFO] Starting MQ2 communication... ";
    selfCheckInfo(info);
    try {
      mq2.begin();
      selfCheckPositive();
      row++;
      return 0;
    } catch (int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }
  }

  int checkADS() {
    info = "[INFO] Starting ADC1 communication... ";
    selfCheckInfo(info);
    try {
      ads1.begin(ADDR_ADC1);
      ads1.setGain(GAIN_ONE);
      selfCheckPositive();
      row++;
      return 0;
    } catch (int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }

    info = "[INFO] Statring ADC2 communication... ";
    selfCheckInfo(info);
    try {
      ads2.begin(ADDR_ADC2);
      ads2.setGain(GAIN_ONE);
      row++;
      return 0;
    } catch (int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }
  }

  int checkRTC() {
    info = "[INFO] Starting RTC communication... ";
    selfCheckInfo(info);
    try {
      rtc.begin();
      setTime();
      selfCheckPositive();
      row++;
      return 0;
    } catch (int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }
  }

  int checkSD() {
    info = "[Info] Initializing SD card...";
    try {
      SD.begin(CS_SD);
      selfCheckPositive();
      row++;
      return 0;
    } catch (int Errorcode) {
      selfCheckNegative(Errorcode);
      row++;
      return Errorcode;
    }

  }
};

void completeSelfCheck() {

  // SD-Card

  const bool sdCardReady = initSDCard();

  if (sdCardReady) {
    testSDCardCommunication();
  } else {
    Serial.println();
    Serial.println("System runs without SD Card");
    Serial.println("Restart testing by pressing T");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");

  // ---------------------- Initialization prozess ----------------------

  
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

void loop() {
  /*
  tft.setTextColor(BLUE, TFT_BLACK);
  char data[32];
  snprintf(data, sizeof(data), "MQ2: %04d", readAverage(MQ2_SENSOR_PIN, 10));
  tft.drawString(data , MARGIN_LEFT, 10);

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "BUTTON: %04d", readAverage(BUTTONS, 10));
  String button = buffer;

  if (readAverage(BUTTONS, 10) == 0) {
    tft.fillScreen(TFT_BLACK);
    Screensaver();
    tft.fillScreen(TFT_BLACK);
  }

  tft.drawString(button, MARGIN_LEFT, 30);
  delay(500);
  */
}
