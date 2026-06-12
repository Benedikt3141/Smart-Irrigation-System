#define USER_SETUP_ID 1001

// Display and Bus
#define ILI9341_DRIVER
#define TFT_PARALLEL_8_BIT

// Control Pins
#define TFT_CS   32
#define TFT_DC   15   // LCD_RS
#define TFT_WR   4
#define TFT_RST  21
#define TFT_RD   22

// 8-Bit-Databus
#define TFT_D0   16
#define TFT_D1   17
#define TFT_D2   25
#define TFT_D3   26
#define TFT_D4   27
#define TFT_D5   14
#define TFT_D6   12
#define TFT_D7   13

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT