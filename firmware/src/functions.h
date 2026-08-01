#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <JPEGDEC.h>



// MoistureSensor
int getMoistureData(int);

// Display
void testDisplay(void);
void selfCheckInfo(String info, int row);
void selfCheckResult(String result, int row, int column, bool status);

// LEDs
void testLEDs(void);

// I2C
void scan_I2C_Addresses(void);

// RTC
void setTime(void);
void printTime(void);

// SD Card
bool initSDCard(void);
bool testSDCardCommunication(void);
void listSDRootDirectory(void);
void handleSDCardSerialCommands(void);

// BMP280
void check_BMP_Sensor(void);

// Buttons
int readAverage(int, int);

// Screensaver
int JPEGDraw(JPEGDRAW *pDraw);
void Screensaver(void);

// Solar Panel
float getSolarVoltage(void);

#endif