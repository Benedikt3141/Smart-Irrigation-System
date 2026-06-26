#include "Arduino.h"
#include "functions.h"
#include "pindefinitions.h"

int readAverage(int GPIO_PIN, int Measurements) { //PinNumber as GPIO Pin

    //int values[10];
    int value;
    long sum = 0;


    for(int i = 0; i < Measurements; i++){
        value = analogRead(GPIO_PIN);
        sum += value;
        //values[i] = v;
        delay(20);
    }

    return sum / Measurements;
}



