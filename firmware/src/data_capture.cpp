#include <Arduino.h>
#include "functions.h"
#include "pindefinitions.h"
#include <SD.h>
#include <FS.h>
#include <Adafruit_BMP280.h>


extern MoistureSensor sensors;
extern Adafruit_BMP280 bmp;
extern bool watering;

void CSV_Logger::initSDCard(int CsPinInput){
    // begin SD Card
    if(!SD.begin(CsPinInput)){ // Error handling
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD.cardType();

    // Card type
    if(cardType == CARD_NONE){ // Error handling
      Serial.println("No SD card attached");
      return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
      Serial.println("MMC");
    } else if(cardType == CARD_SD){
      Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }

    // Card Size
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void CSV_Logger::sendBuffer(fs::FS &fs, const char * path) {
    File data = fs.open(path, FILE_APPEND);
    if (!data) return;
    data.print(lineBuffer);
    data.close();
}

void CSV_Logger::addTimeStamp() {
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);
    offset += strftime(lineBuffer + offset, sizeof(lineBuffer)- offset, "%Y-%m-%d %H:%M:%S;", &timeinfo);
}

void CSV_Logger::addMoistureData() {
    int wetSensorData = 0; // Number of wet sensors

    for (int sensor = 0; sensor < 6; sensor++){
      int moistureData = sensors.getSensorValue(sensor);

      if (moistureData <= wateringValue) { // if any sensor is too dry
        watering = true;
      } else {
        wetSensorData++;
      }
      offset += snprintf(lineBuffer + offset, sizeof(lineBuffer) - offset, "%d;", moistureData);
    }

    if (wetSensorData == 6) {watering = false;} // deactivate watering if every sensor is wet
}


void CSV_Logger::addWaterLevel() {
    float distance = 0; // Waterlevel
    int counter = 0; // Counter for attempts to measure water depth

    do {
      distance = 0; //getDistance(); // as long as the value is not valid
      counter++;
    } while(distance == -1 && counter<10); // try 10 times

    offset += snprintf(lineBuffer + offset, sizeof(lineBuffer) - offset, "%.2f;", distance);

}


void CSV_Logger::addWateringStatus() {
    offset += snprintf(lineBuffer + offset, sizeof(lineBuffer) - offset, "%d;", watering);
}


void CSV_Logger::addTemperature() {
    float temperature = bmp.readTemperature();

    offset += snprintf(lineBuffer + offset, sizeof(lineBuffer) - offset, "%.2f;", temperature);
}


void CSV_Logger::addPreasure() {
    float preasure;
    preasure = bmp.readPressure();

    offset += snprintf(lineBuffer + offset, sizeof(lineBuffer) - offset, "%.2f;", preasure);
}


void CSV_Logger::endLine() {
    snprintf(lineBuffer + offset, sizeof(lineBuffer)- offset, "\n");
    offset = 0;
}


void CSV_Logger::clearBuffer() {
    memset(lineBuffer, 0, sizeof(lineBuffer));
    offset = 0;
}

void CSV_Logger::appendSensorData(void) {
    clearBuffer();
    addTimeStamp();
    addMoistureData();
    addWaterLevel();
    addWateringStatus();
    endLine();
    sendBuffer(SD, "/data.csv");
}