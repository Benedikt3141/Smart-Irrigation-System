#ifndef BUTTONS_H
#define BUTTONS_H

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


volatile bool buttonInterrupt = false;


// Option declaration
enum Button {
    BUTTON_NONE,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_UNKNOWN
};


Button decodeButton(int value) {
    if (value > 3800) return BUTTON_NONE;

    if (value > 0 && value < 10) {
        return BUTTON_1;
    }

    if (value > 300 && value < 400) {
        return BUTTON_2;
    }

    if (value > 800 && value < 900) {
        return BUTTON_3;
    }

    if (value > 1100 && value < 1200) {
        return BUTTON_4;
    }

    return BUTTON_UNKNOWN;
}

void IRAM_ATTR onButtonChange() {
    buttonInterrupt = true;
}

void testButtons(void) {
    while (true) {
        Serial.println(analogRead(BUTTONS));
        delay(1000);
        if (buttonInterrupt) {
            buttonInterrupt = false;

            delay(25); // Entprellen

            int value = analogRead(BUTTONS);
            Button button = decodeButton(value);

            Serial.print("ADC: ");
            Serial.print(value);
            Serial.print(" -> ");

            switch (button) {
                case BUTTON_NONE:
                    Serial.println("kein Button");
                    break;
                case BUTTON_1:
                    Serial.println("Button 1");
                    break;
                case BUTTON_2:
                    Serial.println("Button 2");
                    break;
                case BUTTON_3:
                    Serial.println("Button 3");
                    break;
                case BUTTON_4:
                    Serial.println("Button 4");
                    break;
                default:
                    Serial.println("unbekannter Wert");
                    break;
            }
        }
    }
}

#endif
