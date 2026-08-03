#include <Arduino.h>
#include "functions.h"
#include <Wire.h>

void scan_I2C_Addresses(void) { //scan for I2C Addresses and print to Serial Monitor    
    
    int error, address, nDevices=0;

    Serial.println("Scanning...");

    for(address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
        Serial.print("I2C device found at address 0x");
        if (address<16)
            Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");

        nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknown error at address 0x");
            if (address<16)
                Serial.print("0");
            Serial.println(address,HEX);
        }
    }

    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    delay(5000);           // wait 5 seconds for next scan
}

void testAddress(uint8_t address)
{
    uint32_t start = millis();

    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission(true);

    Serial.printf(
        "0x%02X: error=%u, duration=%lu ms\n",
        address,
        error,
        millis() - start
    );
}

bool pingI2C(uint8_t address) {
    Wire.beginTransmission(address);
    return Wire.endTransmission(true) == 0;
}