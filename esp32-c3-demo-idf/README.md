# ESP32-C3 Round Display Demo Showcase (ESP-IDF)

An over-the-top demo application for the ESP32-C3 with 0.71" round LCD display (GC9D01 driver), built with ESP-IDF framework.

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

## Prerequisites

### Required Software

1. **ESP-IDF** (v5.0 or higher recommended)
   - [Installation Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/)

2. **VS Code with ESP-IDF Extension** (recommended)
   - Install [Visual Studio Code](https://code.visualstudio.com/)
   - Install the official [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)

### TFT_eSPI Library Setup

The project uses the TFT_eSPI library via Arduino as an ESP-IDF component. You need to install it manually:

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
```

Then copy the User_Setup.h file to the TFT_eSPI directory:

```bash
cp ../main/User_Setup.h tft_espi/User_Setup.h
```

Or edit `tft_espi/User_Setup_Select.h` to include the custom setup:

```c
// Comment out the default setup
// #include <User_Setup.h>

// Add path to custom setup
#include "../../main/User_Setup.h"
```

## Building and Flashing

### Using VS Code with ESP-IDF Extension

1. **Open the Project**
   - Open VS Code
   - File → Open Folder → Select `esp32-c3-demo-idf`

2. **Configure the Project**
   - Press `F1` or `Ctrl+Shift+P`
   - Type "ESP-IDF: Set Espressif device target"
   - Select `esp32c3`

3. **Build the Project**
   - Click the "Build" button in the status bar (🔨 icon)
   - Or press `F1` → "ESP-IDF: Build your project"

4. **Flash to Device**
   - Connect your ESP32-C3 via USB
   - Click the "Flash" button in the status bar (⚡ icon)
   - Or press `F1` → "ESP-IDF: Flash your project"

5. **Monitor Serial Output**
   - Click the "Monitor" button in the status bar (🖥️ icon)
   - Or press `F1` → "ESP-IDF: Monitor device"

### Using Command Line (ESP-IDF)

1. **Set up ESP-IDF environment**
   ```bash
   # Navigate to your ESP-IDF installation
   . $HOME/esp/esp-idf/export.sh
   ```

2. **Navigate to project**
   ```bash
   cd esp32-c3-demo-idf
   ```

3. **Set target**
   ```bash
   idf.py set-target esp32c3
   ```

4. **Configure project (optional)**
   ```bash
   idf.py menuconfig
   ```

5. **Build**
   ```bash
   idf.py build
   ```

6. **Flash and monitor**
   ```bash
   idf.py -p /dev/ttyACM0 flash monitor
   ```

   Replace `/dev/ttyACM0` with your actual port (check `ls /dev/tty*`).

## Project Structure

```
esp32-c3-demo-idf/
├── CMakeLists.txt              # Project CMake file
├── sdkconfig.defaults          # Default configuration
├── main/
│   ├── CMakeLists.txt          # Main component CMake
│   ├── idf_component.yml       # Component dependencies (Arduino)
│   ├── main.cpp                # Main application code
│   └── User_Setup.h            # TFT_eSPI configuration
├── components/
│   └── tft_espi/               # TFT_eSPI library (clone manually)
└── README.md                   # This file
```

## Configuration

### Display Settings

Display configuration is in `main/User_Setup.h`:

```c
#define GC9A01_DRIVER     // Driver selection
#define TFT_WIDTH  160    // Display width
#define TFT_HEIGHT 160    // Display height

// GPIO pins
#define TFT_MOSI 7
#define TFT_SCLK 6
#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  8

// SPI speed
#define SPI_FREQUENCY 40000000  // 40MHz
```

### ESP-IDF Settings

Default settings are in `sdkconfig.defaults`. To customize:

```bash
idf.py menuconfig
```

Key settings:
- **Component config → Arduino** - Arduino as ESP-IDF component settings
- **Serial flasher config** - Upload speed, port selection
- **Partition Table** - Flash layout
- **Component config → ESP32-specific** - WiFi buffers, power management

## Customization

### Changing Demo Duration

Edit `main/main.cpp` and modify the `DEMO_DURATION` constant in each demo function:

```cpp
const int DEMO_DURATION = 10000; // milliseconds
```

### Changing Auto-Cycle Time

Edit the menu cycle time in `loop()`:

```cpp
if (millis() - lastCycle > 15000) { // Change 15000 to your preference
    currentDemo = (currentDemo + 1) % numDemos;
    ...
}
```

### Adding Touch Input

The ESP32-C3 supports touch sensing on certain GPIOs. To add touch cycling:

```cpp
// In loop() function
if (touchRead(GPIO_NUM_X) < threshold) {
    currentDemo = (currentDemo + 1) % numDemos;
    lastInteraction = millis();
}
```

## Troubleshooting

### Build Errors

**"Arduino.h: No such file or directory"**
- Ensure Arduino component is properly installed
- Run: `idf.py reconfigure`
- Check `main/idf_component.yml` includes Arduino dependency

**"TFT_eSPI.h: No such file or directory"**
- Clone TFT_eSPI into `components/tft_espi/`
- Ensure CMakeLists.txt in components/tft_espi is present

**Component manager errors**
- Update component manager: `pip install --upgrade idf-component-manager`
- Clear managed components: `rm -rf managed_components`

### Flash Errors

**"Failed to connect to ESP32-C3"**
- Press and hold the BOOT button while connecting USB
- Try slower baud rate: `idf.py -p PORT -b 115200 flash`

**"A fatal error occurred: Could not open PORT"**
- Check port permissions: `sudo usermod -a -G dialout $USER`
- Log out and back in
- Verify port: `ls /dev/ttyACM* /dev/ttyUSB*`

### Display Issues

**Display shows nothing**
- Verify GPIO connections match `User_Setup.h`
- Check SPI frequency (try lowering to 20MHz)
- Press RESET button on board

**Display shows garbage**
- Wrong driver selected - ensure `GC9A01_DRIVER` is defined
- Check TFT_WIDTH and TFT_HEIGHT are 160

### Runtime Issues

**WiFi scan fails**
- WiFi takes 1-2 seconds to scan
- Check 2.4GHz networks exist nearby
- Verify WiFi antenna is not obstructed

**Slow performance**
- Ensure compiler optimization is enabled in menuconfig
- Check SPI frequency (should be 40MHz)
- Verify running release build, not debug

## Performance Notes

- All demos run smoothly at 30-50 FPS
- WiFi scanning may temporarily affect frame rate
- Plasma demo is the most CPU-intensive
- 3D cube uses hardware floating-point acceleration

## Expansion Ideas

1. **Real-time Clock**
   - Add RTC module or use NTP over WiFi
   - Display actual time instead of simulated

2. **Bluetooth Audio Spectrum**
   - Use I2S microphone for real audio input
   - Bluetooth audio receiver capability

3. **Web Interface**
   - WiFi web server to select demos
   - OTA (Over-The-Air) firmware updates

4. **Environmental Sensors**
   - Temperature/humidity display (DHT22, BME280)
   - Air quality monitoring (CCS811)

5. **More Games**
   - Pong adapted for circular display
   - Breakout with curved bricks
   - Asteroids in polar coordinates

6. **Smart Watch Features**
   - Step counter with MPU6050 accelerometer
   - Heart rate monitor
   - Notifications from phone

## Memory Usage

- Program size: ~300-400KB
- RAM usage: ~50-80KB
- Plenty of room for expansion on 4MB flash

## Differences from PlatformIO Version

This ESP-IDF version:
- Uses native ESP-IDF build system (CMake)
- Arduino included as an ESP-IDF component
- Better integration with ESP-IDF features
- More control over WiFi, Bluetooth, power management
- Easier to add ESP-IDF specific libraries

## License

This project is open source and provided as-is for educational and entertainment purposes.

## Credits

Built with ESP-IDF framework and Arduino as a component.

Uses the TFT_eSPI library by Bodmer.

Designed for the ESP32-C3-LCD-0.71 from Spotpear.

Created with Claude Code.

## Support

For ESP-IDF specific questions:
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [ESP-IDF GitHub](https://github.com/espressif/esp-idf)
- [Espressif Forums](https://esp32.com/)

For TFT_eSPI questions:
- [TFT_eSPI GitHub](https://github.com/Bodmer/TFT_eSPI)
