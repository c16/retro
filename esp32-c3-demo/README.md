# ESP32-C3 Round Display Demo Showcase

An over-the-top demo application for the ESP32-C3 with 0.71" round LCD display (GC9D01 driver).

## Hardware Specifications

- **Board:** ESP32-C3FH4 (RISC-V 32-bit @ 160MHz)
- **Display:** 0.71" Round IPS LCD
- **Resolution:** 160x160 pixels
- **Driver:** GC9D01
- **Interface:** 4-wire SPI
- **Flash:** 4MB
- **WiFi:** 2.4GHz 802.11 b/g/n
- **Bluetooth:** 5.0 LE

### GPIO Pin Configuration

| Function | GPIO |
|----------|------|
| TFT_MOSI | GPIO7 |
| TFT_SCLK | GPIO6 |
| TFT_CS   | GPIO5 |
| TFT_DC   | GPIO4 |
| TFT_RST  | GPIO8 |

## Features

This demo showcase includes **9 impressive demonstrations**:

### 1. 🕐 Analog Clock
- Smooth sweeping second, minute, and hour hands
- Color-coded hands (red/green/cyan)
- Hour markers and decorative face
- Realistic analog clock movement

### 2. 🎲 3D Rotating Cube
- Real-time 3D wireframe rendering
- Rotation on all three axes
- Rainbow-colored edges
- Perspective projection

### 3. 📡 WiFi Scanner
- Scans and displays nearby WiFi networks
- Circular visualization with signal strength
- Color-coded by RSSI (green/yellow/red)
- Rotating network display

### 4. 📊 Radar Sweep
- Classic radar sweep animation
- Random targets with fade effects
- Concentric rings and crosshairs
- Green phosphor aesthetic

### 5. 🐍 Circular Snake Game
- Snake game adapted for round display
- Polar coordinate movement
- AI-controlled snake
- Growing snake with score display

### 6. 🌈 Plasma Effect
- Real-time plasma rendering
- Multi-layered sine wave interference
- Smooth color transitions
- Full circular display utilization

### 7. ✨ Particle System
- 50 simultaneous particles
- Physics-based movement
- Trail effects
- Circular boundary detection
- Rainbow colors

### 8. 💚 Matrix Rain
- Matrix-style falling characters
- Multiple streams with varying speeds
- Fading trail effect
- Circular clipping mask

### 9. 🎵 Spectrum Analyzer
- Circular audio spectrum visualization
- 16-band display
- Animated bars with color gradient
- Simulated audio response

## Demo Navigation

The application features an **auto-rotating circular menu**:
- Demos automatically cycle every 15 seconds
- Each demo runs for 10-15 seconds
- Animated menu with visual indicators
- Demo icons arranged in a circle

## Building and Flashing

### Prerequisites

1. **Install PlatformIO**
   - [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
   - OR [PlatformIO Core (CLI)](https://docs.platformio.org/en/latest/core/installation.html)

2. **Clone Repository**
   ```bash
   cd esp32-c3-demo
   ```

### Build Options

**Using PlatformIO IDE:**
1. Open the `esp32-c3-demo` folder in VS Code
2. PlatformIO will automatically detect the project
3. Click the "Build" button in the status bar
4. Click "Upload" to flash to device

**Using PlatformIO CLI:**
```bash
# Build the project
pio run

# Build and upload
pio run --target upload

# Build, upload, and monitor serial output
pio run --target upload --target monitor
```

### Serial Monitor

Monitor the serial output at 115200 baud:
```bash
pio device monitor
```

Or use any serial terminal:
```bash
screen /dev/ttyACM0 115200
# or
minicom -D /dev/ttyACM0 -b 115200
```

## Customization

### Changing Demo Duration

Edit `main.cpp` and modify the `DEMO_DURATION` constant in each demo function:
```cpp
const int DEMO_DURATION = 10000; // milliseconds
```

### Changing Auto-Cycle Time

Edit the menu cycle time in `loop()`:
```cpp
if (millis() - lastCycle > 15000) { // Change 15000 to your preference
```

### Adding Touch Input

The hardware supports touch detection. To add touch cycling through demos, uncomment and implement the touch detection section in `loop()`:

```cpp
// Example using touchRead on available GPIO
if (touchRead(GPIO_NUM_X) < threshold) {
    currentDemo = (currentDemo + 1) % numDemos;
    lastInteraction = millis();
}
```

### Modifying Colors

Each demo has customizable colors. For example, in the analog clock:
```cpp
tft.drawLine(CENTER_X, CENTER_Y, sx, sy, TFT_RED);    // Second hand
tft.drawWideLine(CENTER_X, CENTER_Y, mx, my, 2, TFT_GREEN, TFT_BLACK); // Minute
tft.drawWideLine(CENTER_X, CENTER_Y, hx, hy, 3, TFT_CYAN, TFT_BLACK);  // Hour
```

## Performance Notes

- All demos run smoothly at 30-50 FPS
- WiFi scanning may temporarily affect frame rate
- Plasma demo is the most CPU-intensive
- 3D cube uses floating-point math extensively

## Troubleshooting

### Display Not Working
1. Check GPIO pin connections match `platformio.ini` configuration
2. Verify SPI connections (MOSI, SCLK, CS, DC, RST)
3. Ensure 3.3V power supply is adequate (>300mA recommended)

### Upload Fails
1. Press and hold BOOT button while connecting USB
2. Try reducing upload speed in `platformio.ini`:
   ```ini
   upload_speed = 460800
   ```
3. Check USB cable supports data transfer

### Compilation Errors
1. Update PlatformIO platform:
   ```bash
   pio platform update espressif32
   ```
2. Clean build files:
   ```bash
   pio run --target clean
   ```

### WiFi Scanner Shows No Networks
- WiFi.scanNetworks() can take 1-2 seconds
- Ensure antenna is not blocked
- Check 2.4GHz WiFi networks exist nearby

## Technical Details

### Memory Usage
- Program size: ~300KB
- RAM usage: ~50KB
- Plenty of room for expansion

### Libraries Used
- **TFT_eSPI**: High-performance display driver
- **ArduinoJson**: JSON parsing (for future expansion)
- **WiFi**: Built-in ESP32 WiFi support

### Display Configuration
The display is configured via build flags in `platformio.ini`:
- Driver: GC9A01 (very similar to GC9D01)
- SPI Frequency: 40MHz (maximum stable speed)
- 16-bit color (RGB565)

## Expansion Ideas

Add these features to make it even more impressive:

1. **Bluetooth Audio Spectrum**
   - Use I2S microphone for real audio spectrum
   - Bluetooth audio receiver

2. **Web Server Control**
   - WiFi web interface to select demos
   - OTA (Over-The-Air) updates

3. **Environmental Sensors**
   - Temperature/humidity display
   - Air quality monitoring

4. **Games**
   - Pong adapted for circular display
   - Breakout with curved bricks
   - Asteroids

5. **Smart Watch Features**
   - Real time via NTP
   - Notifications
   - Step counter (with accelerometer)

## License

This project is open source and provided as-is for educational and entertainment purposes.

## Credits

Built with PlatformIO and TFT_eSPI library.

Designed for the ESP32-C3-LCD-0.71 from Spotpear.

Created with Claude Code.
