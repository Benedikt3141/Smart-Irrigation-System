#pragma once

#include <Arduino.h>

class TFT_eSPI;

class Touch_PCB01 {
public:
    struct RawPoint {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        bool valid = false;
    };

    struct Point {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        bool touched = false;
    };

    struct Calibration {
        // raw -> native display coordinates (rotation 0)
        float xFromRawX = 1.0f;
        float xFromRawY = 0.0f;
        float xOffset   = 0.0f;
        float yFromRawX = 0.0f;
        float yFromRawY = 1.0f;
        float yOffset   = 0.0f;
        bool valid = false;
    };

    // PCB01 / common 2.4" UNO resistive TFT shield wiring:
    // XP = LCD_D6 = GPIO27
    // YM = LCD_D7 = GPIO14
    // YP drive = LCD_WR = GPIO4, analog sense = GPIO39 (ADC1)
    // XM drive = LCD_RS = GPIO15, analog sense = GPIO35 (ADC1)
    Touch_PCB01(
        uint8_t xp = 27,
        uint8_t ym = 14,
        uint8_t ypDrive = 4,
        uint8_t ypSense = 39,
        uint8_t xmDrive = 15,
        uint8_t xmSense = 35,
        uint8_t tftCs = 33,
        uint16_t nativeWidth = 240,
        uint16_t nativeHeight = 320
    );

    // Call after tft.begin(). Keep this rotation equal to TFT_eSPI's rotation.
    void begin(uint8_t rotation = 1);
    void setRotation(uint8_t rotation);
    uint8_t rotation() const { return _rotation; }

    uint16_t width() const;
    uint16_t height() const;

    // Pressure threshold for a valid touch. Typical starting point: 150..300.
    void setPressureThreshold(uint16_t threshold) { _pressureThreshold = threshold; }
    uint16_t pressureThreshold() const { return _pressureThreshold; }

    // Raw measurements, independent of calibration/rotation.
    RawPoint getRawPoint();
    bool isTouched();

    // Returns true only when a valid calibrated touch is present.
    bool getPoint(Point &point);
    Point getPoint();

    void setCalibration(const Calibration &calibration);
    const Calibration &calibration() const { return _calibration; }
    bool isCalibrated() const { return _calibration.valid; }
    void clearCalibration();

    // Interactive 5-point calibration. Calibration is stored in RAM.
    // Optionally call saveCalibration() afterwards.
    bool calibrate(TFT_eSPI &tft, Print &log = Serial);

#if defined(ESP32)
    // ESP32 NVS storage. Namespace must be <= 15 characters.
    bool saveCalibration(const char *nameSpace = "touchPCB01") const;
    bool loadCalibration(const char *nameSpace = "touchPCB01");
    bool eraseSavedCalibration(const char *nameSpace = "touchPCB01");
#endif

private:
    static constexpr uint8_t ADC_BITS = 12;
    static constexpr int ADC_MAX_VALUE = (1 << ADC_BITS) - 1;
    static constexpr uint8_t ADC_SAMPLES = 5; // odd number for median

    uint8_t _xp;
    uint8_t _ym;
    uint8_t _ypDrive;
    uint8_t _ypSense;
    uint8_t _xmDrive;
    uint8_t _xmSense;
    uint8_t _tftCs;

    uint16_t _nativeWidth;
    uint16_t _nativeHeight;
    uint8_t _rotation = 1;
    uint16_t _pressureThreshold = 180;

    Calibration _calibration;

    int _savedCsLevel = HIGH;

    void selectTouchBus();
    void restoreTftBus();

    int16_t readMedianAdc(uint8_t pin);
    int16_t readRawX();
    int16_t readRawY();
    int16_t readPressure();

    void nativeToScreen(float nativeX, float nativeY, float &screenX, float &screenY) const;
    void screenToNative(float screenX, float screenY, float &nativeX, float &nativeY) const;

    static bool solve3x3(float a[3][3], float b[3], float out[3]);
    bool fitAffine(const RawPoint raw[], const float nativeX[], const float nativeY[], size_t count);

    RawPoint waitForStablePress(Print &log);
    void waitForRelease();
    static void drawCrosshair(TFT_eSPI &tft, int16_t x, int16_t y, uint16_t color);
};
