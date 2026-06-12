

// ********** I2C Pins *********
#ifndef SCL
#define SCL 22
#endif

#ifndef SDA
#define SDA 21
#endif

#ifndef ADDR_ADC1
#define ADDR_ADC1 4B
#endif

#ifndef ADDR_ADC2
#define ADDR_ADC2 48
#endif

#ifndef ADDR_CLOCK1
#define ADDR_CLOCK1 0x57 // Address is not used
#endif

#ifndef ADDR_CLOCK2
#define ADDR_CLOCK2 0x68 // Address is not used
#endif

#ifndef ADDR_BMP280
#define ADDR_BMP280 0x76
#endif

#ifndef ADDR_BMP180
#define ADDR_BMP180 0x77
#endif


// ********* SPI Pins *********
#ifndef MOSI
#define MOSI 23
#endif

#ifndef MISO
#define MISO 19
#endif

#ifndef SCL_SPI
#define SCL_SPI 18
#endif

#ifndef CS_SD
#define CS_SD 5
#endif


// ********* Analog Signal Pins ********
#ifndef ANALOG9
#define ANALOG9 34 // Buttons
#endif

#ifndef ANALOG10
#define ANALOG10 35 // Display Touch
#endif

#ifndef ANALOG11
#define ANALOG11 36
#endif

#ifndef ANALOG12
#define ANALOG12 39 // Display Touch
#endif


// ******** LCD Display Pins ******** //actually they're configured in .pio\libdeps\az-delivery-devkit-v4\TFT_eSPI\User_Setups. It's just for clarification :)
#ifndef LCD_CS 
#define LCD_CS 32 
#endif

#ifndef LCD_WR
#define LCD_WR 4 // Analog Measurement on Pin 39
#endif

#ifndef LCD_RS
#define LCD_RS 15 // Analog Measurement on Pin 34
#endif




#ifndef LCD_D0
#define LCD_D0 16
#endif

#ifndef LCD_D1
#define LCD_D1 17
#endif

#ifndef LCD_D2
#define LCD_D2 25
#endif

#ifndef LCD_D3
#define LCD_D3 26
#endif

#ifndef LCD_D4
#define LCD_D4 27
#endif

#ifndef LCD_D5
#define LCD_D5 14
#endif

#ifndef LCD_D6
#define LCD_D6 12
#endif

#ifndef LCD_D7
#define LCD_D7 13
#endif


// ********** LED Strip ********
#ifndef LED_PIN
#define LED_PIN 33
#endif

#ifndef NUMBER_LEDS
#define NUMBER_LEDS 8
#endif

