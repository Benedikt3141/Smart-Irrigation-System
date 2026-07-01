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



void setup() {
  Serial.begin(115200);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");


  // ******** GPIO initialization ********
  pinMode(BUTTONS, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MQ2_SENSOR_PIN, INPUT);
  analogReadResolution(12);

  tft.begin(); // begin Display
  #ifdef USE_DMA
    tft.initDMA();
  #endif

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  SPI.begin(); // begin SD Card

  if (!SD.begin(CS_SD)) {
      Serial.println("SD card mount failed!");
      return;
  }

  Serial.println("SD card mounted successfully.");



  Wire.begin(); // Start I2C
  mq2.begin(); // Start MQ2 Sensor

  ads1.begin(ADDR_ADC1); // start analog digital I2C extender
  ads2.begin(ADDR_ADC2);
  ads1.setGain(GAIN_ONE);
  ads2.setGain(GAIN_ONE);


  jpeg.setPixelType(RGB565_BIG_ENDIAN);


  if (!rtc.begin()) { // Start Clock
    Serial.println("Couldn't find RTC!");
  }

  attachInterrupt( // Start Button Interrupt
        digitalPinToInterrupt(BUTTONS),
        onButtonChange,
        CHANGE
    );
}

void loop() {
    Serial.printf("SolarValue: %.2f V\n", getSolarVoltage());
    delay(1000);
}
