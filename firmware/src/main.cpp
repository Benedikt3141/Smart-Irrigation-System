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
//#include <lvgl.h>
#include <FastLED.h>
#include <Adafruit_NeoPixel.h>

#define WIRE Wire

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
    if (!pingI2C(ADDR_ADC1) &&
      !pingI2C(ADDR_ADC2) &&
      !pingI2C(ADDR_RTC)) {
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
    Serial.printf("Ping 0x48 vor ads1.begin(): %s\n", pingI2C(0x48) ? "OK" : "FAILED");
    if (!ads1.begin(ADDR_ADC1, &Wire)) {
      
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
    Serial.printf("Ping 0x49 vor ads1.begin(): %s\n", pingI2C(0x49) ? "OK" : "FAILED");
    if (!ads2.begin(ADDR_ADC2, &Wire)) {
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
    Serial.printf("Ping 0x68 vor ads1.begin(): %s\n", pingI2C(0x68) ? "OK" : "FAILED");
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

bool initI2C()
{
    if (!Wire.setPins(I2C_SDA, I2C_SCL)) {
        Serial.println("Wire.setPins() fehlgeschlagen");
        return false;
    }

    if (!Wire.begin()) {
        Serial.println("Wire.begin() fehlgeschlagen");
        return false;
    }

    Wire.setClock(100000);
    delay(50);
    return true;
}

void scanI2C() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0)
      {
      Serial.print("I2C device found at address 0x");
      if (address<16)
          Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
      }
      else if (error==4)
      {
      Serial.print("Unknown error at address 0x");
      if (address<16)
          Serial.print("0");
      Serial.println(address,HEX);
      }
  }
  if (nDevices == 0)
      Serial.println("No I2C devices found\n");
  else
      Serial.println("done\n");
}

bool pingI2C(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission(true) == 0;
}

bool testAddress(uint8_t address)
{
    uint32_t start = millis();

    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission(true);

    Serial.printf(
        "Adresse 0x%02X: Fehler=%u, Zeit=%lu ms, SDA=%d, SCL=%d\n",
        address,
        error,
        millis() - start,
        digitalRead(I2C_SDA),
        digitalRead(I2C_SCL)
    );

    return error == 0;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nStart Programm: 'PlantWatering BreadBoard_Code'\n");

  if (!initI2C()) {
    Serial.println("I2C konnte nicht initialisiert werden");
    while (true) {
        delay(1000);
    }
  }
  //scanI2C();

  // ---------------------- Initialization prozess ----------------------

  selfCheckRoutine check;
  //check.completeSelfCheck();
  //Serial.println(selfCheckRoutine.checkI2C());
  //Serial.println(selfCheckRoutine.checkADS1());
  //Serial.println(selfCheckRoutine.checkADS2());
  //Serial.println(selfCheckRoutine.checkRTC());

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

  //scanI2C();
}

// ***************** LEDs still don't work... *****************

void loop() {
  
}
