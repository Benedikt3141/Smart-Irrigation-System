#define ESP32_WIFI_TOUCH

// General
#include <Arduino.h>
#include <Wire.h>
// Repository specific headers
#include "pindefinitions.h"
#include "functions.h"
#include "Buttons.h" //the button code that would mess up the main code
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


Adafruit_BMP280 bmp; // use I2C interface
MQ2 mq2(MQ2_SENSOR_PIN);
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
//CRGB leds[NUMBER_LEDS];
RTC_DS3231 rtc;
JPEGDEC jpeg;
Adafruit_ADS1115 adc1;
Adafruit_ADS1115 adc2;
MoistureSensor sensors;
SelfCheckRoutine check;

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("Start Programm: 'PlantWatering BreadBoard_Code'");
    Serial.println();

    // GPIO
    pinMode(BUTTONS, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(MQ2_SENSOR_PIN, INPUT);

    pinMode(CS_SD, OUTPUT);
    digitalWrite(CS_SD, HIGH);

    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    
    check.completeSelfCheck();

    
    
    analogReadResolution(12);
    
    jpeg.setPixelType(RGB565_BIG_ENDIAN);
}


void loop() {
  
}