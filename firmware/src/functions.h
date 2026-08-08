#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <JPEGDEC.h>
#include <Wire.h>

// SelfCheck
class SelfCheckRoutine {
    private:
    String info;
    char Result[16];
    int row = 0;

    public:
    void completeSelfCheck();
    void selfCheckInfo(String info);
    void selfCheckPositive();
    void selfCheckNegative(int Errorcode);
    int checkDisplay();
    int checkI2C();
    int checkMQ2();
    int checkADC1();
    int checkADC2();
    int checkRTC();
    int checkSD();
    int checkLEDs();
    int checkBMP();
};

// MoistureSensor
class MoistureSensor{
    private:
    public:
    int getSensorValue(int SensorID);
    void printSensorData(int SensorID);
    void printSensorData(void);
};

//CSV_logger
class CSV_Logger {
    private: 
    char lineBuffer[255];
    int offset = 0;
    
    public:
    void initSDCard(int CsPinInput);
    void sendBuffer(fs::FS &fs, const char * path);
    void addTimeStamp();
    void addMoistureData();
    void addWaterLevel();
    void addWateringStatus();
    void addTemperature();
    void addPreasure();
    void endLine();
    void clearBuffer();
    void appendSensorData();
};

// Display
void testDisplay(void);
void selfCheckInfo(String info, int row);
void selfCheckResult(String result, int row, int column, bool status);

// LEDs
void testLEDs(void);

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


//I2C
void testAddress(uint8_t address);
void scan_I2C_Addresses(void);
bool pingI2C(uint8_t address);

//Waterstation --> functions for later on
int getDistance(void);

#endif