# Spotpear ESP32-C3 1.44" LCD - Hello World & Button Test

**Simple demo to test the display and all 4 buttons on the Spotpear ESP32-C3 1.44" LCD board**

![Platform](https://img.shields.io/badge/platform-ESP32--C3-green) ![Display](https://img.shields.io/badge/display-128x128%20ST7735-blue) ![Buttons](https://img.shields.io/badge/buttons-4-orange)

---

## Hardware Overview

The **Spotpear ESP32-C3 1.44" LCD** is a compact development board featuring:
- **ESP32-C3** RISC-V microcontroller (160MHz, WiFi, BLE 5.0)
- **1.44" Square LCD** (128x128 pixels, ST7735 driver)
- **4 Buttons** (3 programmable + 1 hardware reset)
- **LiPo Charger** (PL4054 chip)
- **USB-C** connector
- **LED** on GPIO11
- **6-pin Expansion** connector

---

## Pin Assignments

### Display (ST7735 - SPI)

| Function | GPIO | Description |
|----------|------|-------------|
| SCLK | 3 | SPI clock |
| MOSI | 4 | SPI data out |
| DC | 0 | Data/command select |
| CS | 2 | Chip select |
| RST | 5 | Display reset |

**Display Specs:**
- Driver: ST7735 (GREENTAB3 variant)
- Resolution: 128 x 128 pixels
- Color: 16-bit RGB565
- Color Order: BGR (Blue-Green-Red)
- Inversion: Enabled

### Buttons

| Button | GPIO | Type | Description |
|--------|------|------|-------------|
| BOOT | 9 | Input (pullup) | Multi-purpose boot button |
| Key 1 | 8 | Input (pullup) | User button 1 |
| Key 2 | 10 | Input (pullup) | User button 2 |
| Reset | EN | Hardware | Chip reset (not software accessible) |

**Important:** All buttons pull GPIO to ground when pressed. Internal pullup resistors must be enabled.

### LED

| Component | GPIO | Note |
|-----------|------|------|
| LED | 11 | Connected to flash VCC - limited use during operation |

### Expansion Connector

6-pin connector with additional GPIOs:

| Pin | GPIO/Function |
|-----|---------------|
| 1 | GPIO1 |
| 2 | GPIO6 |
| 3 | GPIO7 |
| 4 | GPIO20 |
| 5 | GPIO21 |
| 6 | GND |

---

## What This Demo Does

This hello world application demonstrates:

1. **Display Initialization**
   - Configures ST7735 driver
   - Sets 180° rotation
   - Draws title and border

2. **Button Testing**
   - Monitors all 3 programmable buttons
   - Displays real-time button states
   - Shows press counters for each button
   - Visual indicators (red=pressed, green=released)

3. **Serial Output**
   - Prints hardware configuration
   - Logs button press events
   - Shows press counts

### Screen Layout

```
╔══════════════════════════╗
║ HELLO WORLD              ║ <- Title
║ ESP32-C3                 ║ <- Subtitle
║                          ║
║ BOOT  ●ON   x5           ║ <- Button states
║ KEY1  ●OFF  x12          ║    (circle = indicator)
║ KEY2  ●OFF  x3           ║    (number = press count)
║                          ║
║ Press buttons            ║ <- Instructions
╚══════════════════════════╝
```

- **Green circle** = Button released
- **Red circle** = Button pressed
- **Counter** = Number of times pressed

---

## Quick Start

### 1. Prerequisites

- **ESP-IDF v5.0+** with VS Code extension
- **TFT_eSPI library** (will be cloned)
- **Spotpear ESP32-C3 1.44" LCD board**
- **USB-C cable**

### 2. Clone TFT_eSPI Library

From the project root:

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../esp32c3-144-hello/main/User_Setup.h tft_espi/User_Setup.h
```

### 3. Build with VS Code

1. Open project folder in VS Code
2. Press F1 → "ESP-IDF: Set Espressif device target" → `esp32c3`
3. Click 🔨 **Build**
4. Click ⚡ **Flash**
5. Click 🖥️ **Monitor**

### 4. Build with Command Line

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Navigate to project
cd esp32c3-144-hello

# Configure for ESP32-C3
idf.py set-target esp32c3

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor
```

### 5. Test the Demo

1. **Power on** the board
2. **Watch the display** - should show "HELLO WORLD"
3. **Press BOOT button** (GPIO9) - see status change
4. **Press Key 1** (GPIO8) - see status change
5. **Press Key 2** (GPIO10) - see status change
6. **Check serial monitor** - see button events logged

---

## Serial Monitor Output

When running, you'll see:

```
╔════════════════════════════════════════════════════╗
║  Spotpear ESP32-C3 1.44" LCD - Hello World       ║
╚════════════════════════════════════════════════════╝

Hardware Configuration:
  Display: ST7735 128x128
  GPIO3: SCLK
  GPIO4: MOSI
  GPIO0: DC
  GPIO2: CS
  GPIO5: RST

Buttons:
  GPIO9:  BOOT button
  GPIO8:  Key 1
  GPIO10: Key 2
  EN pin: Reset (hardware)

Display initialized!

Button test ready!
Press any button to test...

BOOT pressed! (count: 1)
Key 1 pressed! (count: 1)
Key 2 pressed! (count: 1)
BOOT pressed! (count: 2)
...
```

---

## Understanding the Code

### Button Detection

The code uses **edge detection** to count button presses:

```cpp
// Active LOW with internal pullup
bool bootNow = (digitalRead(BTN_BOOT) == LOW);

// Detect press event (falling edge)
if (bootNow && !bootPressed) {
    bootPressed = true;
    bootCount++;
    Serial.printf("BOOT pressed! (count: %d)\n", bootCount);
}

// Detect release event (rising edge)
else if (!bootNow && bootPressed) {
    bootPressed = false;
}
```

### Display Updates

Only updates when button state changes:

```cpp
if (bootEvent || key1Event || key2Event) {
    updateButtonDisplay();
}
```

### Colors

Uses RGB565 color format:

```cpp
#define COLOR_BG       0x0000   // Black
#define COLOR_TITLE    0x07FF   // Cyan
#define COLOR_TEXT     0xFFFF   // White
#define COLOR_PRESSED  0xF800   // Red
#define COLOR_RELEASED 0x07E0   // Green
#define COLOR_BORDER   0xFFE0   // Yellow
```

---

## Troubleshooting

### Display Issues

**Black screen:**
- Check USB-C cable connection
- Press Reset button
- Verify TFT_eSPI User_Setup.h is copied correctly
- Check SPI wiring (should be built-in on this board)

**Wrong colors:**
- Verify `TFT_BGR` is defined in User_Setup.h
- Check `TFT_INVERSION_ON` is enabled

**Upside down:**
- Change `tft.setRotation(2)` to different value (0-3)

### Button Issues

**Buttons not responding:**
- Verify `INPUT_PULLUP` mode is set
- Check GPIO numbers are correct
- Press firmly - buttons might be small

**Buttons stuck "pressed":**
- Check for hardware issue
- Try rebooting the board

**Counts incrementing too fast:**
- This is button bounce - normal for mechanical buttons
- Can add software debouncing if needed

### Build Errors

**TFT_eSPI not found:**
```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../esp32c3-144-hello/main/User_Setup.h tft_espi/
```

**Wrong display driver:**
- Make sure User_Setup.h has `ST7735_DRIVER` defined
- NOT `GC9A01_DRIVER` (that's for round displays)

---

## Customization

### Change Colors

Edit color definitions in main.cpp:

```cpp
#define COLOR_BG       0x001F   // Blue background
#define COLOR_TITLE    0xF81F   // Magenta title
```

### Add More Features

The expansion connector provides additional GPIOs:

```cpp
#define SENSOR_PIN 1   // Use GPIO1 from expansion
pinMode(SENSOR_PIN, INPUT);
```

### Modify Display Rotation

Try different orientations:

```cpp
tft.setRotation(0);  // 0°
tft.setRotation(1);  // 90°
tft.setRotation(2);  // 180° (current)
tft.setRotation(3);  // 270°
```

---

## Hardware Limitations

**GPIO11 LED:**
- Connected to VCC for internal SPI flash
- Cannot be used freely during operation
- Disabled during deep sleep
- Use with caution or avoid

**Reset Button:**
- Hardwired to EN (enable) pin
- Causes full chip reset
- Not accessible from software
- Cannot be used for input

**Internal Pullups Required:**
- No external pull-up resistors on board
- Must enable `INPUT_PULLUP` mode
- Buttons are active LOW

---

## Next Steps

This is a foundation for more complex projects:

1. **Add WiFi** - ESP32-C3 has built-in WiFi
2. **Use BLE** - Bluetooth 5.0 Low Energy supported
3. **Graphics** - Draw shapes, images, animations
4. **Games** - Use buttons for gameplay
5. **IoT Dashboard** - Display sensor data
6. **Menu System** - Navigate with buttons

---

## References

- [Spotpear ESP32-C3 1.44 LCD Documentation](https://homeding.github.io/boards/esp32c3/spotpear-144lcd.htm)
- [Spotpear GitHub Repository](https://github.com/Spotpear/ESP32C3_1.44inch)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)

---

## License

This demo is provided as-is for educational purposes.

---

**Ready to build more with your ESP32-C3 board!** 🚀

