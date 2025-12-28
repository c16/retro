// TFT_eSPI User Setup for Spotpear ESP32-C3 1.44" LCD
// 128x128 ST7735 Square Display

#define USER_SETUP_INFO "ESP32_C3_ST7735_144"

// Driver selection
#define ST7735_DRIVER      // ST7735 128x128 display
#define ST7735_GREENTAB3   // Specific variant

// Display resolution
#define TFT_WIDTH  128
#define TFT_HEIGHT 128

// Color order
#define TFT_BGR  // Blue-Green-Red order

// Display inversion
#define TFT_INVERSION_ON

// ESP32-C3 GPIO pin configuration for SPI interface
#define TFT_MOSI 4   // SPI MOSI
#define TFT_SCLK 3   // SPI SCLK
#define TFT_CS   2   // Chip select
#define TFT_DC   0   // Data/Command
#define TFT_RST  5   // Reset

// Font loading
#define LOAD_GLCD   // Include Font 1 (original Adafruit font)
#define LOAD_FONT2  // Small 16 pixel font
#define LOAD_FONT4  // Medium 26 pixel font
#define LOAD_FONT6  // Large 48 pixel font
#define LOAD_FONT7  // 7 segment style font
#define LOAD_FONT8  // Large 75 pixel font
#define LOAD_GFXFF  // FreeFonts
#define SMOOTH_FONT

// SPI frequency (40MHz is maximum)
#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// SPIFFS for smooth fonts
#define SMOOTH_FONT
