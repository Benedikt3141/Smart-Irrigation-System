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
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define ESP32_WIFI_TOUCH

TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");

  Wire.begin(); //Start I2C

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
  }

  setTime();
  printTime();

  testDisplay();

  initSDCard();
}

void loop() {
  // put your main code here, to run repeatedly:
}
