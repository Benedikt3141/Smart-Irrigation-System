#include <Arduino.h>
#include "functions.h"
#include "pindefinitions.h"
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 adc1;
extern Adafruit_ADS1115 adc2;

int MoistureSensor::getSensorValue(int SensorID) {
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

void MoistureSensor:: printSensorData(int SensorID) {
        Serial.printf("Sensor-%d Value: %4d \n", SensorID+1, getSensorValue(SensorID));
    }

void MoistureSensor:: printSensorData(void) {
        for (int i = 0; i<6; i++) {
            Serial.printf("Sensor-%d Value: %4d \n", i+1, getSensorValue(i));
            delay(100);
        }
    }

