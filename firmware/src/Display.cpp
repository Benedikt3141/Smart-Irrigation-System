#include <Arduino.h>
#include "pindefinitions.h"
#include "functions.h"
#include "lvgl.h"
#include <TFT_eSPI.h>
#include "Touch_PCB01.h"


extern Touch_PCB01 touch;
extern TFT_eSPI tft;

// LVGL calls this function to print log information
void print_error(lv_log_level_t level, const char *buf) {
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}

void testDisplay(void) {
    // code
    tft.init();
    tft.setRotation(1);  // Querformat: 320 x 240 Pixel
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    tft.setCursor(15, 15);
    tft.println("ESP32 + ILI9341");

    tft.fillRect(20, 60, 100, 50, TFT_RED);
    tft.fillCircle(190, 90, 30, TFT_GREEN);
    tft.drawLine(20, 150, 290, 150, TFT_YELLOW);

    tft.setCursor(20, 180);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.println("Display funktioniert!");
    return;
}


