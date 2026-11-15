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

void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);

    // Standard TFT-Libraries erwarten RGB565 (2 Bytes pro Pixel)
    tft.pushColors((uint16_t*)px_map, w * h);
    tft.endWrite();

    // Informiert LVGL, dass das Rendering fertig ist
    lv_display_flush_ready(disp);
}

void my_touch_read(lv_indev_t * indev, lv_indev_data_t * data) {
    Touch_PCB01::Point p;

    if (touch.getPoint(p))
    {
        data->state = LV_INDEV_STATE_PR;

        data->point.x = p.x;
        data->point.y = p.y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void create_ui() {
    lv_obj_set_style_bg_color(lv_screen_active(), lv_palette_main(LV_PALETTE_BLUE_GREY), 0);

    // Button
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -20);

    // Textfeld für Klick-Zähler
    lv_obj_t * info_label = lv_label_create(lv_screen_active());
    lv_label_set_text(info_label, "Bereit für Touch...");
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 40);

    // Event Handler
    lv_obj_add_event_cb(btn, [](lv_event_t * e) {
        static uint32_t count = 0;
        count++;
        lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);
        lv_label_set_text_fmt(label, "Geklickt: %" PRIu32 " mal", count);
    }, LV_EVENT_CLICKED, info_label);

    // Button Label
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Drueck mich");
    lv_obj_center(btn_label);
}