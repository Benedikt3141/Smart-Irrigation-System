#define USER_SETUP_ID 1001

// Display and Bus
#define ILI9341_DRIVER
#define TFT_PARALLEL_8_BIT

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

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT