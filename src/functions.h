#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <JPEGDEC.h>


// MoistureSensor
int getMoistureData(int);

// Display
void testDisplay(void);

// LEDs
void testLEDs(void);

// I2C
void scan_I2C_Addresses(void);

// RTC
void setTime(void);
void printTime(void);

// SD Card
void initSDCard(void);

// BMP280
void check_BMP_Sensor(void);

// Buttons
int readAverage(int, int);

// Screensaver
int JPEGDraw(JPEGDRAW *pDraw);
void Screensaver(void);

#endif