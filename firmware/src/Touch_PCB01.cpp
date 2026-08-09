#include "Touch_PCB01.h"

#include <TFT_eSPI.h>
#include <math.h>

#if defined(ESP32)
#include <Preferences.h>
#endif

Touch_PCB01::Touch_PCB01(
    uint8_t xp,
    uint8_t ym,
    uint8_t ypDrive,
    uint8_t ypSense,
    uint8_t xmDrive,
    uint8_t xmSense,
    uint8_t tftCs,
    uint16_t nativeWidth,
    uint16_t nativeHeight)
    : _xp(xp),
      _ym(ym),
      _ypDrive(ypDrive),
      _ypSense(ypSense),
      _xmDrive(xmDrive),
      _xmSense(xmSense),
      _tftCs(tftCs),
      _nativeWidth(nativeWidth),
      _nativeHeight(nativeHeight) {}

void Touch_PCB01::begin(uint8_t rotation) {
    _rotation = rotation & 0x03;

    // The project already uses 12-bit ADC resolution. Set it explicitly so
    // touch readings always use 0..4095.
    analogReadResolution(ADC_BITS);

    pinMode(_ypSense, INPUT); // GPIO39, input-only on classic ESP32
    pinMode(_xmSense, INPUT); // GPIO35, input-only on classic ESP32

    // TFT_eSPI has already initialized these pins. Ensure a safe idle state.
    pinMode(_tftCs, OUTPUT);
    pinMode(_ypDrive, OUTPUT); // TFT_WR
    digitalWrite(_ypDrive, HIGH);
    pinMode(_xmDrive, OUTPUT); // TFT_DC / LCD_RS
    pinMode(_xp, OUTPUT);      // TFT_D6
    pinMode(_ym, OUTPUT);      // TFT_D7
}

void Touch_PCB01::setRotation(uint8_t rotation) {
    _rotation = rotation & 0x03;
}

uint16_t Touch_PCB01::width() const {
    return (_rotation & 1) ? _nativeHeight : _nativeWidth;
}

uint16_t Touch_PCB01::height() const {
    return (_rotation & 1) ? _nativeWidth : _nativeHeight;
}

void Touch_PCB01::selectTouchBus() {
    // Save the TFT's CS state, then deselect it before changing shared pins.
    _savedCsLevel = digitalRead(_tftCs);
    pinMode(_tftCs, OUTPUT);
    digitalWrite(_tftCs, HIGH);

    // WR must never pulse low while the TFT is selected.
    pinMode(_ypDrive, OUTPUT);
    digitalWrite(_ypDrive, HIGH);
}

void Touch_PCB01::restoreTftBus() {
    // Keep TFT deselected while restoring all shared bus pins.
    digitalWrite(_tftCs, HIGH);

    // WR first: HIGH is the safe idle level.
    pinMode(_ypDrive, OUTPUT);
    digitalWrite(_ypDrive, HIGH);

    // Restore TFT_eSPI's shared DC and data pins to outputs.
    pinMode(_xmDrive, OUTPUT);
    pinMode(_xp, OUTPUT);
    pinMode(_ym, OUTPUT);

    // Their actual data levels do not matter while WR stays HIGH.
    digitalWrite(_xmDrive, LOW);
    digitalWrite(_xp, LOW);
    digitalWrite(_ym, LOW);

    digitalWrite(_tftCs, _savedCsLevel);
}

int16_t Touch_PCB01::readMedianAdc(uint8_t pin) {
    int values[ADC_SAMPLES];

    for (uint8_t i = 0; i < ADC_SAMPLES; ++i) {
        values[i] = analogRead(pin);
        delayMicroseconds(12);
    }

    for (uint8_t i = 1; i < ADC_SAMPLES; ++i) {
        const int key = values[i];
        int8_t j = static_cast<int8_t>(i) - 1;
        while (j >= 0 && values[j] > key) {
            values[j + 1] = values[j];
            --j;
        }
        values[j + 1] = key;
    }

    return static_cast<int16_t>(values[ADC_SAMPLES / 2]);
}

int16_t Touch_PCB01::readRawX() {
    // Adafruit 4-wire principle:
    // X+ = HIGH, X- = LOW, measure Y+.
    // On PCB01, Y+ is driven by GPIO4 but sensed via ADC1 GPIO39.
    pinMode(_ypDrive, INPUT); // Y+ high impedance
    pinMode(_ym, INPUT);      // Y- high impedance

    pinMode(_xp, OUTPUT);
    digitalWrite(_xp, HIGH);

    pinMode(_xmDrive, OUTPUT);
    digitalWrite(_xmDrive, LOW);

    delayMicroseconds(25);
    const int value = readMedianAdc(_ypSense);
    return static_cast<int16_t>(ADC_MAX_VALUE - value);
}

int16_t Touch_PCB01::readRawY() {
    // Y+ = HIGH, Y- = LOW, measure X-.
    // On PCB01, X- is driven by GPIO15 but sensed via ADC1 GPIO35.
    pinMode(_xp, INPUT);      // X+ high impedance
    pinMode(_xmDrive, INPUT); // X- high impedance

    pinMode(_ypDrive, OUTPUT);
    digitalWrite(_ypDrive, HIGH);

    pinMode(_ym, OUTPUT);
    digitalWrite(_ym, LOW);

    delayMicroseconds(25);
    const int value = readMedianAdc(_xmSense);
    return static_cast<int16_t>(ADC_MAX_VALUE - value);
}

int16_t Touch_PCB01::readPressure() {
    // Adafruit-style pressure measurement:
    // X+ = LOW, Y- = HIGH, X- and Y+ high impedance.
    pinMode(_xp, OUTPUT);
    digitalWrite(_xp, LOW);

    pinMode(_ym, OUTPUT);
    digitalWrite(_ym, HIGH);

    pinMode(_xmDrive, INPUT);
    pinMode(_ypDrive, INPUT);

    delayMicroseconds(25);

    const int z1 = readMedianAdc(_xmSense);
    const int z2 = readMedianAdc(_ypSense);

    int32_t pressure = ADC_MAX_VALUE - (z2 - z1);
    if (pressure < 0) pressure = 0;
    if (pressure > ADC_MAX_VALUE) pressure = ADC_MAX_VALUE;

    return static_cast<int16_t>(pressure);
}

Touch_PCB01::RawPoint Touch_PCB01::getRawPoint() {
    RawPoint p;

    selectTouchBus();

    p.x = readRawX();
    p.y = readRawY();
    p.z = readPressure();
    p.valid = p.z >= static_cast<int16_t>(_pressureThreshold);

    restoreTftBus();
    return p;
}

bool Touch_PCB01::isTouched() {
    selectTouchBus();
    const int16_t pressure = readPressure();
    restoreTftBus();
    return pressure >= static_cast<int16_t>(_pressureThreshold);
}

bool Touch_PCB01::getPoint(Point &point) {
    point = Point{};

    const RawPoint raw = getRawPoint();
    point.z = raw.z;

    if (!raw.valid || !_calibration.valid) {
        return false;
    }

    const float nativeX =
        _calibration.xFromRawX * raw.x +
        _calibration.xFromRawY * raw.y +
        _calibration.xOffset;

    const float nativeY =
        _calibration.yFromRawX * raw.x +
        _calibration.yFromRawY * raw.y +
        _calibration.yOffset;

    float screenX = 0.0f;
    float screenY = 0.0f;
    nativeToScreen(nativeX, nativeY, screenX, screenY);

    screenX = constrain(screenX, 0.0f, static_cast<float>(width() - 1));
    screenY = constrain(screenY, 0.0f, static_cast<float>(height() - 1));

    point.x = static_cast<int16_t>(lroundf(screenX));
    point.y = static_cast<int16_t>(lroundf(screenY));
    point.touched = true;
    return true;
}

Touch_PCB01::Point Touch_PCB01::getPoint() {
    Point p;
    getPoint(p);
    return p;
}

void Touch_PCB01::setCalibration(const Calibration &calibration) {
    _calibration = calibration;
}

void Touch_PCB01::clearCalibration() {
    _calibration = Calibration{};
}

void Touch_PCB01::nativeToScreen(float x, float y, float &sx, float &sy) const {
    switch (_rotation & 0x03) {
        case 0:
            sx = x;
            sy = y;
            break;
        case 1:
            sx = static_cast<float>(_nativeHeight - 1) - y;
            sy = x;
            break;
        case 2:
            sx = static_cast<float>(_nativeWidth - 1) - x;
            sy = static_cast<float>(_nativeHeight - 1) - y;
            break;
        case 3:
            sx = y;
            sy = static_cast<float>(_nativeWidth - 1) - x;
            break;
    }
}

void Touch_PCB01::screenToNative(float sx, float sy, float &x, float &y) const {
    switch (_rotation & 0x03) {
        case 0:
            x = sx;
            y = sy;
            break;
        case 1:
            x = sy;
            y = static_cast<float>(_nativeHeight - 1) - sx;
            break;
        case 2:
            x = static_cast<float>(_nativeWidth - 1) - sx;
            y = static_cast<float>(_nativeHeight - 1) - sy;
            break;
        case 3:
            x = static_cast<float>(_nativeWidth - 1) - sy;
            y = sx;
            break;
    }
}

bool Touch_PCB01::solve3x3(float a[3][3], float b[3], float out[3]) {
    // Gaussian elimination with partial pivoting.
    for (uint8_t col = 0; col < 3; ++col) {
        uint8_t pivot = col;
        float maxAbs = fabsf(a[col][col]);

        for (uint8_t row = col + 1; row < 3; ++row) {
            const float candidate = fabsf(a[row][col]);
            if (candidate > maxAbs) {
                maxAbs = candidate;
                pivot = row;
            }
        }

        if (maxAbs < 1e-8f) return false;

        if (pivot != col) {
            for (uint8_t k = 0; k < 3; ++k) {
                const float tmp = a[col][k];
                a[col][k] = a[pivot][k];
                a[pivot][k] = tmp;
            }
            const float tmpB = b[col];
            b[col] = b[pivot];
            b[pivot] = tmpB;
        }

        const float divisor = a[col][col];
        for (uint8_t k = col; k < 3; ++k) a[col][k] /= divisor;
        b[col] /= divisor;

        for (uint8_t row = 0; row < 3; ++row) {
            if (row == col) continue;
            const float factor = a[row][col];
            for (uint8_t k = col; k < 3; ++k) {
                a[row][k] -= factor * a[col][k];
            }
            b[row] -= factor * b[col];
        }
    }

    out[0] = b[0];
    out[1] = b[1];
    out[2] = b[2];
    return true;
}

bool Touch_PCB01::fitAffine(
    const RawPoint raw[],
    const float nativeX[],
    const float nativeY[],
    size_t count) {

    if (count < 3) return false;

    // Least-squares solution of:
    // target = a*rawX + b*rawY + c
    float normal[3][3] = {};
    float bx[3] = {};
    float by[3] = {};

    for (size_t i = 0; i < count; ++i) {
        const float v[3] = {
            static_cast<float>(raw[i].x),
            static_cast<float>(raw[i].y),
            1.0f
        };

        for (uint8_t r = 0; r < 3; ++r) {
            for (uint8_t c = 0; c < 3; ++c) {
                normal[r][c] += v[r] * v[c];
            }
            bx[r] += v[r] * nativeX[i];
            by[r] += v[r] * nativeY[i];
        }
    }

    float axMatrix[3][3];
    float ayMatrix[3][3];
    memcpy(axMatrix, normal, sizeof(normal));
    memcpy(ayMatrix, normal, sizeof(normal));

    float xCoeff[3];
    float yCoeff[3];

    if (!solve3x3(axMatrix, bx, xCoeff)) return false;
    if (!solve3x3(ayMatrix, by, yCoeff)) return false;

    _calibration.xFromRawX = xCoeff[0];
    _calibration.xFromRawY = xCoeff[1];
    _calibration.xOffset   = xCoeff[2];
    _calibration.yFromRawX = yCoeff[0];
    _calibration.yFromRawY = yCoeff[1];
    _calibration.yOffset   = yCoeff[2];
    _calibration.valid = true;

    return true;
}

Touch_PCB01::RawPoint Touch_PCB01::waitForStablePress(Print &log) {
    RawPoint first;

    while (true) {
        first = getRawPoint();
        if (first.valid) break;
        delay(10);
    }

    delay(35); // finger/stylus settles

    constexpr uint8_t COLLECT = 9;
    int32_t sumX = 0;
    int32_t sumY = 0;
    int32_t sumZ = 0;
    uint8_t validCount = 0;

    for (uint8_t i = 0; i < COLLECT; ++i) {
        const RawPoint p = getRawPoint();
        if (p.valid) {
            sumX += p.x;
            sumY += p.y;
            sumZ += p.z;
            ++validCount;
        }
        delay(8);
    }

    RawPoint result = first;
    if (validCount > 0) {
        result.x = static_cast<int16_t>(sumX / validCount);
        result.y = static_cast<int16_t>(sumY / validCount);
        result.z = static_cast<int16_t>(sumZ / validCount);
        result.valid = true;
    }

    log.print("raw x=");
    log.print(result.x);
    log.print(" y=");
    log.print(result.y);
    log.print(" z=");
    log.println(result.z);

    return result;
}

void Touch_PCB01::waitForRelease() {
    uint32_t releasedSince = 0;

    while (true) {
        if (!isTouched()) {
            if (releasedSince == 0) releasedSince = millis();
            if (millis() - releasedSince >= 80) return;
        } else {
            releasedSince = 0;
        }
        delay(10);
    }
}

void Touch_PCB01::drawCrosshair(TFT_eSPI &tft, int16_t x, int16_t y, uint16_t color) {
    constexpr int16_t R = 10;
    tft.drawCircle(x, y, R, color);
    tft.drawLine(x - R - 4, y, x + R + 4, y, color);
    tft.drawLine(x, y - R - 4, x, y + R + 4, color);
}

bool Touch_PCB01::calibrate(TFT_eSPI &tft, Print &log) {
    constexpr size_t POINT_COUNT = 5;
    constexpr int16_t MARGIN = 24;

    tft.setRotation(_rotation);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Touch calibration", width() / 2, 8, 2);
    tft.drawString("Touch each crosshair", width() / 2, 28, 2);

    const int16_t targetX[POINT_COUNT] = {
        MARGIN,
        static_cast<int16_t>(width() - 1 - MARGIN),
        static_cast<int16_t>(width() - 1 - MARGIN),
        MARGIN,
        static_cast<int16_t>((width() - 1) / 2)
    };

    const int16_t targetY[POINT_COUNT] = {
        MARGIN + 24,
        MARGIN + 24,
        static_cast<int16_t>(height() - 1 - MARGIN),
        static_cast<int16_t>(height() - 1 - MARGIN),
        static_cast<int16_t>((height() - 1) / 2)
    };

    RawPoint raw[POINT_COUNT];
    float nativeX[POINT_COUNT];
    float nativeY[POINT_COUNT];

    log.println("\nPCB01 touch calibration");
    log.println("Touch the 5 crosshairs.");

    for (size_t i = 0; i < POINT_COUNT; ++i) {
        drawCrosshair(tft, targetX[i], targetY[i], TFT_YELLOW);

        raw[i] = waitForStablePress(log);
        screenToNative(
            static_cast<float>(targetX[i]),
            static_cast<float>(targetY[i]),
            nativeX[i],
            nativeY[i]
        );

        drawCrosshair(tft, targetX[i], targetY[i], TFT_GREEN);
        waitForRelease();
        delay(80);
        drawCrosshair(tft, targetX[i], targetY[i], TFT_BLACK);
    }

    if (!fitAffine(raw, nativeX, nativeY, POINT_COUNT)) {
        clearCalibration();
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawCentreString("Calibration failed", width() / 2, height() / 2 - 10, 2);
        log.println("Calibration failed: affine matrix is singular.");
        return false;
    }

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString("Calibration OK", width() / 2, height() / 2 - 10, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Touch to test", width() / 2, height() / 2 + 14, 2);

    log.println("Calibration coefficients:");
    log.print("x = ");
    log.print(_calibration.xFromRawX, 8);
    log.print(" * rx + ");
    log.print(_calibration.xFromRawY, 8);
    log.print(" * ry + ");
    log.println(_calibration.xOffset, 4);
    log.print("y = ");
    log.print(_calibration.yFromRawX, 8);
    log.print(" * rx + ");
    log.print(_calibration.yFromRawY, 8);
    log.print(" * ry + ");
    log.println(_calibration.yOffset, 4);

    delay(500);
    return true;
}

#if defined(ESP32)
bool Touch_PCB01::saveCalibration(const char *nameSpace) const {
    if (!_calibration.valid) return false;

    Preferences prefs;
    if (!prefs.begin(nameSpace, false)) return false;

    bool ok = true;
    ok &= prefs.putBool("valid", true) == 1;
    ok &= prefs.putFloat("xrx", _calibration.xFromRawX) == sizeof(float);
    ok &= prefs.putFloat("xry", _calibration.xFromRawY) == sizeof(float);
    ok &= prefs.putFloat("xo",  _calibration.xOffset) == sizeof(float);
    ok &= prefs.putFloat("yrx", _calibration.yFromRawX) == sizeof(float);
    ok &= prefs.putFloat("yry", _calibration.yFromRawY) == sizeof(float);
    ok &= prefs.putFloat("yo",  _calibration.yOffset) == sizeof(float);

    prefs.end();
    return ok;
}

bool Touch_PCB01::loadCalibration(const char *nameSpace) {
    Preferences prefs;
    if (!prefs.begin(nameSpace, true)) return false;

    if (!prefs.getBool("valid", false)) {
        prefs.end();
        return false;
    }

    Calibration c;
    c.xFromRawX = prefs.getFloat("xrx", NAN);
    c.xFromRawY = prefs.getFloat("xry", NAN);
    c.xOffset   = prefs.getFloat("xo",  NAN);
    c.yFromRawX = prefs.getFloat("yrx", NAN);
    c.yFromRawY = prefs.getFloat("yry", NAN);
    c.yOffset   = prefs.getFloat("yo",  NAN);
    prefs.end();

    const bool finite =
        isfinite(c.xFromRawX) && isfinite(c.xFromRawY) && isfinite(c.xOffset) &&
        isfinite(c.yFromRawX) && isfinite(c.yFromRawY) && isfinite(c.yOffset);

    if (!finite) return false;

    c.valid = true;
    _calibration = c;
    return true;
}

bool Touch_PCB01::eraseSavedCalibration(const char *nameSpace) {
    Preferences prefs;
    if (!prefs.begin(nameSpace, false)) return false;
    const bool ok = prefs.clear();
    prefs.end();
    clearCalibration();
    return ok;
}
#endif
