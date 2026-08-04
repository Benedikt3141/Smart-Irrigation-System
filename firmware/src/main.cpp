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
#include "RTClib.h"
#include <Adafruit_BMP280.h>
#include "MQ2.h" // library: https://github.com/labay11/MQ-2-sensor-library <- Thank you so much!
#include <Adafruit_ADS1X15.h>
//LEDs
#include <FastLED.h>
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


class selfCheckRoutine {
  String info;
  char Result[16];
  int row = 0;

  public:

  void completeSelfCheck() {
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
    delay(1000);
  }

  void selfCheckInfo(String info) {
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

  int checkMQ2() {
    info = "[INFO] Starting MQ2 communication... ";
    selfCheckInfo(info);
    mq2.begin(); // no if or error possible in this library...
    selfCheckPositive();
    row++;
    return 0;
  }

  int checkADC1() {
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
  
  int checkADC2() {
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

  int checkRTC() {
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

  int checkSD() {
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

  int checkLEDs() {
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
};

class MoistureSensor{
    private:

    public:
    int getSensorValue(int SensorID) {
        int value;
        switch(SensorID) {
            case 0:
                value = adc1.readADC_SingleEnded(0);
                return value;
            case 1:
                value = adc1.readADC_SingleEnded(1);
                return value;
            case 2:
                value = adc1.readADC_SingleEnded(2);
                return value;
            case 3:
                value = adc1.readADC_SingleEnded(3);
                return value;
            case 4:
                value = adc2.readADC_SingleEnded(0);
                return value;
            case 5:
                value = adc2.readADC_SingleEnded(1);
                return value;
            default:
                Serial.println("SensorID out of Range");
                return 120;
        }
    }

    void printSensorData(int SensorID) {
        Serial.printf("Sensor-%d Value: %4d \n", SensorID+1, getSensorValue(SensorID));
    }

    void printSensorData(void) {
        for (int i = 0; i<6; i++) {
            Serial.printf("Sensor-%d Value: %4d \n", i+1, getSensorValue(i));
            delay(100);
        }
    }

};

selfCheckRoutine check;
MoistureSensor sensors;
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
  sensors.printSensorData();
}