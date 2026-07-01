#include <Arduino.h>
#include "functions.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 ads1;

float getSolarVoltage(void) {
    unsigned int analogvalue = ads1.readADC_SingleEnded(3);

    // with GAIN = GAIN_ONE (up to 4.096V) and a 16 bit ADC resolution (values up to +-32767)
    float Voltage = (4.096 / 32767) * analogvalue;
    return Voltage;
}
