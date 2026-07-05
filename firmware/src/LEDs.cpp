#include <Arduino.h>
#include "pindefinitions.h"
#include "functions.h"
#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel leds;

void testLEDs(void) {
    //TestLEDs
    leds.begin();
    leds.clear();
    while (true) {
        leds.clear();
        for(int i=0; i<NUMBER_LEDS; i++) {
            leds.setPixelColor(i, leds.Color(50,50,50));
            leds.show();
            delay(500);
        }
    }
}