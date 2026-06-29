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



Adafruit_BMP280 bmp; // use I2C interface
MQ2 mq2(MQ2_SENSOR_PIN);
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel leds(NUMBER_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
RTC_DS3231 rtc;

// ***************************************** Button Interrupt *****************************************
volatile bool buttonInterrupt = false;


// Option declaration
enum Button {
    BUTTON_NONE,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_UNKNOWN
};


Button decodeButton(int value) {
    if (value > 3800) return BUTTON_NONE;

    if (value > 0 && value < 10) {
        return BUTTON_1;
    }

    if (value > 300 && value < 400) {
        return BUTTON_2;
    }

    if (value > 800 && value < 900) {
        return BUTTON_3;
    }

    if (value > 1100 && value < 1200) {
        return BUTTON_4;
    }

    return BUTTON_UNKNOWN;
}

void IRAM_ATTR onButtonChange() {
    buttonInterrupt = true;
}


// ***************************************** Actual Code *****************************************

void setup() {
  Serial.begin(115200);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");


  // ******** GPIO initialization ********
  pinMode(BUTTONS, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MQ2_SENSOR_PIN, INPUT);
  analogReadResolution(12);


  Wire.begin(); //Start I2C
  mq2.begin(); //Start MQ2 Sensor

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
  }

  attachInterrupt(
        digitalPinToInterrupt(BUTTONS),
        onButtonChange,
        CHANGE
    );
}

void loop() {
    Serial.println(analogRead(BUTTONS));
    delay(1000);
    if (buttonInterrupt) {
        buttonInterrupt = false;

        delay(25); // Entprellen

        int value = analogRead(BUTTONS);
        Button button = decodeButton(value);

        Serial.print("ADC: ");
        Serial.print(value);
        Serial.print(" -> ");

        switch (button) {
            case BUTTON_NONE:
                Serial.println("kein Button");
                break;
            case BUTTON_1:
                Serial.println("Button 1");
                break;
            case BUTTON_2:
                Serial.println("Button 2");
                break;
            case BUTTON_3:
                Serial.println("Button 3");
                break;
            case BUTTON_4:
                Serial.println("Button 4");
                break;
            default:
                Serial.println("unbekannter Wert");
                break;
        }
    }
}
