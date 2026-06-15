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

#define ESP32_WIFI_TOUCH


Adafruit_BMP280 bmp; // use I2C interface
MQ2 mq2(ANALOG9);
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");

  Wire.begin(); //Start I2C
  mq2.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
  }

  setTime();
  printTime();
  testDisplay();
  initSDCard();
  scan_I2C_Addresses();
  mq2.read(true);
  check_BMP_Sensor();
}

void loop() {
  // put your main code here, to run repeatedly:
}
