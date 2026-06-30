

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
#define ADDR_CLOCK1 0x57 
#endif

#ifndef ADDR_CLOCK2
#define ADDR_CLOCK2 0x68 
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
#ifndef MQ_SENSOR_PIN
#define MQ2_SENSOR_PIN 34 // Analog 9
#endif

#ifndef ANALOG10
#define ANALOG10 35 // Display Touch
#endif

#ifndef BUTTONS 
#define BUTTONS 36 // Analog 11
#endif

#ifndef ANALOG12
#define ANALOG12 39 // Display Touch
#endif


// ******** LCD Display Pins ******** //actually they're configured in .pio\libdeps\az-delivery-devkit-v4\TFT_eSPI\User_Setups. It's just for clarification :)
#ifndef LCD_Pins 

    // Control Pins
    #define TFT_CS   33
    #define TFT_DC   15   // LCD_RS
    #define TFT_RST  -1
    #define TFT_WR    4
    #define TFT_RD    2

    // 8-Bit-Databus
    #define TFT_D0   12
    #define TFT_D1   13
    #define TFT_D2   26
    #define TFT_D3   25
    #define TFT_D4   17
    #define TFT_D5   16
    #define TFT_D6   27
    #define TFT_D7   14

#endif


// ********** LED Strip ********
#ifndef LED_PIN
#define LED_PIN 32
#endif

#ifndef NUMBER_LEDS
#define NUMBER_LEDS 8
#endif

// ********** Screensaver Video ********
#ifndef SCREENSAVER_VIDEO_FILE_NAME
#define  SCREENSAVER_VIDEO_FILE_NAME "Lizard.mjpeg";
#endif
