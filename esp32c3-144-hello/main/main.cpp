/**
 * Spotpear ESP32-C3 1.44" LCD - Hello World & Button Test
 *
 * Hardware:
 * - ESP32-C3
 * - 128x128 ST7735 Square Display
 * - 4 Buttons (BOOT, Key1, Key2, Reset)
 */

#include <Arduino.h>
#include <TFT_eSPI.h>

// Button GPIO Pins
#define BTN_BOOT  9     // BOOT button
#define BTN_KEY1  8     // Key 1
#define BTN_KEY2  10    // Key 2
// Note: Reset button is hardware reset (EN pin), not software accessible

// LED (note: GPIO11 is connected to flash VCC, tricky to use)
#define LED_PIN   11

// Display instance
TFT_eSPI tft = TFT_eSPI(128, 128);

// Button states
bool bootPressed = false;
bool key1Pressed = false;
bool key2Pressed = false;

// Button press counters
int bootCount = 0;
int key1Count = 0;
int key2Count = 0;

// Colors
#define COLOR_BG       0x0000   // Black
#define COLOR_TITLE    0x07FF   // Cyan
#define COLOR_TEXT     0xFFFF   // White
#define COLOR_PRESSED  0xF800   // Red
#define COLOR_RELEASED 0x07E0   // Green
#define COLOR_BORDER   0xFFE0   // Yellow

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n╔════════════════════════════════════════════════════╗");
    Serial.println("║  Spotpear ESP32-C3 1.44\" LCD - Hello World       ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println();

    // Initialize buttons with internal pullups
    pinMode(BTN_BOOT, INPUT_PULLUP);
    pinMode(BTN_KEY1, INPUT_PULLUP);
    pinMode(BTN_KEY2, INPUT_PULLUP);

    Serial.println("Hardware Configuration:");
    Serial.println("  Display: ST7735 128x128");
    Serial.println("  GPIO3: SCLK");
    Serial.println("  GPIO4: MOSI");
    Serial.println("  GPIO0: DC");
    Serial.println("  GPIO2: CS");
    Serial.println("  GPIO5: RST");
    Serial.println();
    Serial.println("Buttons:");
    Serial.println("  GPIO9:  BOOT button");
    Serial.println("  GPIO8:  Key 1");
    Serial.println("  GPIO10: Key 2");
    Serial.println("  EN pin: Reset (hardware)");
    Serial.println();

    // Initialize display
    tft.init();
    tft.setRotation(2);  // 180 degree rotation
    tft.fillScreen(COLOR_BG);

    Serial.println("Display initialized!");
    Serial.println();

    // Draw initial screen
    drawScreen();

    Serial.println("Button test ready!");
    Serial.println("Press any button to test...");
    Serial.println();
}

void loop() {
    // Read button states (active LOW)
    bool bootNow = (digitalRead(BTN_BOOT) == LOW);
    bool key1Now = (digitalRead(BTN_KEY1) == LOW);
    bool key2Now = (digitalRead(BTN_KEY2) == LOW);

    // Detect button press events (edge detection)
    bool bootEvent = false;
    bool key1Event = false;
    bool key2Event = false;

    if (bootNow && !bootPressed) {
        bootPressed = true;
        bootCount++;
        bootEvent = true;
        Serial.printf("BOOT pressed! (count: %d)\n", bootCount);
    } else if (!bootNow && bootPressed) {
        bootPressed = false;
    }

    if (key1Now && !key1Pressed) {
        key1Pressed = true;
        key1Count++;
        key1Event = true;
        Serial.printf("Key 1 pressed! (count: %d)\n", key1Count);
    } else if (!key1Now && key1Pressed) {
        key1Pressed = false;
    }

    if (key2Now && !key2Pressed) {
        key2Pressed = true;
        key2Count++;
        key2Event = true;
        Serial.printf("Key 2 pressed! (count: %d)\n", key2Count);
    } else if (!key2Now && key2Pressed) {
        key2Pressed = false;
    }

    // Update display if any button changed
    if (bootEvent || key1Event || key2Event) {
        updateButtonDisplay();
    }

    delay(10);  // Small delay for debouncing
}

void drawScreen() {
    tft.fillScreen(COLOR_BG);

    // Draw border
    tft.drawRect(0, 0, 128, 128, COLOR_BORDER);
    tft.drawRect(1, 1, 126, 126, COLOR_BORDER);

    // Title
    tft.setTextColor(COLOR_TITLE, COLOR_BG);
    tft.setTextDatum(TC_DATUM);  // Top center
    tft.drawString("HELLO WORLD", 64, 10, 2);

    // Subtitle
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString("ESP32-C3", 64, 28, 2);

    // Draw button labels and states
    updateButtonDisplay();

    // Instructions at bottom
    tft.setTextDatum(BC_DATUM);  // Bottom center
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString("Press buttons", 64, 120, 1);
}

void updateButtonDisplay() {
    // Button positions
    int yStart = 50;
    int ySpacing = 20;

    // Clear button area
    tft.fillRect(5, yStart - 2, 118, 60, COLOR_BG);

    tft.setTextDatum(TL_DATUM);  // Top left

    // BOOT button
    drawButtonStatus("BOOT", bootPressed, bootCount, 10, yStart);

    // Key 1
    drawButtonStatus("KEY1", key1Pressed, key1Count, 10, yStart + ySpacing);

    // Key 2
    drawButtonStatus("KEY2", key2Pressed, key2Count, 10, yStart + ySpacing * 2);
}

void drawButtonStatus(const char* name, bool pressed, int count, int x, int y) {
    // Button name
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(name, x, y, 2);

    // Status indicator (circle)
    int circleX = x + 50;
    int circleY = y + 6;
    int circleRadius = 6;

    uint16_t statusColor = pressed ? COLOR_PRESSED : COLOR_RELEASED;
    tft.fillCircle(circleX, circleY, circleRadius, statusColor);
    tft.drawCircle(circleX, circleY, circleRadius, COLOR_TEXT);

    // Status text
    const char* statusText = pressed ? "ON " : "OFF";
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(statusText, circleX + 10, y, 2);

    // Count
    char countStr[16];
    sprintf(countStr, "x%d", count);
    tft.drawString(countStr, x + 95, y, 2);
}
