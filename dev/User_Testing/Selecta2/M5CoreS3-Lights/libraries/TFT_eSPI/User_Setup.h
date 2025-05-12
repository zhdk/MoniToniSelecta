#define ILI9341_DRIVER

#define TFT_SDA_READ
#define M5STACK
#define TFT_INVERSION_ON

#define TFT_MISO -1
#define TFT_MOSI 37
#define TFT_SCLK 36
#define TFT_CS    3
#define TFT_DC   35
#define TFT_RST  -1
//#define TFT_BL   -1
#define TOUCH_CS -1  // (suppress warning)

#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  16000000

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
