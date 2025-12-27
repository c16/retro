// TFT_eSPI User Setup for ESP32-C3 with GC9D01 Round Display
// This file configures the TFT_eSPI library for your specific hardware

#define USER_SETUP_INFO "ESP32_C3_GC9D01"

// Driver selection
#define GC9A01_DRIVER      // GC9A01 driver is compatible with GC9D01

// Display resolution
#define TFT_WIDTH  160
#define TFT_HEIGHT 160

// ESP32-C3 GPIO pin configuration for SPI interface
#define TFT_MOSI 7   // SPI MOSI
#define TFT_SCLK 6   // SPI SCLK
#define TFT_CS   5   // Chip select
#define TFT_DC   4   // Data/Command
#define TFT_RST  8   // Reset

// Font loading
#define LOAD_GLCD   // Include Font 1 (original Adafruit font)
#define LOAD_FONT2  // Small 16 pixel font
#define LOAD_FONT4  // Medium 26 pixel font
#define LOAD_FONT6  // Large 48 pixel font
#define LOAD_FONT7  // 7 segment style font
#define LOAD_FONT8  // Large 75 pixel font
#define LOAD_GFXFF  // FreeFonts
#define SMOOTH_FONT

// SPI frequency (40MHz is maximum stable)
#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
