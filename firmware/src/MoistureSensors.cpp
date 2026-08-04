#include <Arduino.h>
#include "functions.h"
#include "pindefinitions.h"
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 adc1;
extern Adafruit_ADS1115 adc2;

int getMoistureData(int IndexSensor) {
    Serial.printf("Read Data from Sensor", IndexSensor, "\n");
    return 0;
}

