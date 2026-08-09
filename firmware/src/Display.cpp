#include <Arduino.h>
#include "pindefinitions.h"
#include "functions.h"
#include "lvgl.h"
#include <TFT_eSPI.h>
#include "Touch_PCB01.h"


extern Touch_PCB01 touch;
extern TFT_eSPI tft;

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


static int counter = 0;

void lvglDisplayFlush(
    lv_disp_drv_t* disp,
    const lv_area_t* area,
    lv_color_t* color_p
)
{
    const uint32_t width =
        area->x2 - area->x1 + 1;

    const uint32_t height =
        area->y2 - area->y1 + 1;

    tft.startWrite();

    tft.setAddrWindow(
        area->x1,
        area->y1,
        width,
        height
    );

    tft.pushColors(
        reinterpret_cast<uint16_t*>(&color_p->full),
        width * height,
        true
    );

    tft.endWrite();

    // LVGL mitteilen:
    // Übertragung abgeschlossen
    lv_disp_flush_ready(disp);
}

void lvglTouchRead(
    lv_indev_drv_t* indev,
    lv_indev_data_t* data
)
{
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


void buttonEvent(lv_event_t* event)
{
    lv_event_code_t code =
        lv_event_get_code(event);

    if (code == LV_EVENT_CLICKED)
    {
        counter++;

        lv_label_set_text_fmt(
            counterLabel,
            "Counter: %d",
            counter
        );

        Serial.printf(
            "Counter = %d\n",
            counter
        );
    }
}


void createGUI()
{
    // ------------------------------------------
    // Hintergrund
    // ------------------------------------------

    lv_obj_t* screen = lv_scr_act();

    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(0x202020),
        LV_PART_MAIN
    );


    // ------------------------------------------
    // Überschrift
    // ------------------------------------------

    lv_obj_t* title =
        lv_label_create(screen);

    lv_label_set_text(
        title,
        "Smart Irrigation"
    );

    lv_obj_align(
        title,
        LV_ALIGN_TOP_MID,
        0,
        25
    );


    // ------------------------------------------
    // Counter Label
    // ------------------------------------------

    counterLabel =
        lv_label_create(screen);

    lv_label_set_text(
        counterLabel,
        "Counter: 0"
    );

    lv_obj_align(
        counterLabel,
        LV_ALIGN_CENTER,
        0,
        -30
    );


    // ------------------------------------------
    // Button
    // ------------------------------------------

    lv_obj_t* button =
        lv_btn_create(screen);

    lv_obj_set_size(
        button,
        140,
        60
    );

    lv_obj_align(
        button,
        LV_ALIGN_CENTER,
        0,
        40
    );

    lv_obj_add_event_cb(
        button,
        buttonEvent,
        LV_EVENT_CLICKED,
        nullptr
    );


    // ------------------------------------------
    // Text innerhalb des Buttons
    // ------------------------------------------

    lv_obj_t* buttonLabel =
        lv_label_create(button);

    lv_label_set_text(
        buttonLabel,
        "+1"
    );

    lv_obj_center(buttonLabel);
}