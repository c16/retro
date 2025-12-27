# ESP32-C3 Round Display Demo - Project Overview

Two versions of an over-the-top demo application for your ESP32-C3 with 0.71" GC9D01 round display.

## 📦 Two Project Versions

### 1. **PlatformIO Version** (`esp32-c3-demo/`)

Best for: Quick setup, Arduino-focused development

**Pros:**
- Fastest to get started
- Simple `platformio.ini` configuration
- Automatic library management
- Works in VS Code with PlatformIO extension
- Great for Arduino developers

**Setup:**
```bash
cd esp32-c3-demo
pio run --target upload
```

**Documentation:**
- `esp32-c3-demo/README.md` - Full guide
- `esp32-c3-demo/QUICKSTART.md` - 5-minute setup

### 2. **ESP-IDF Version** (`esp32-c3-demo-idf/`)

Best for: Official ESP toolchain, native ESP-IDF features

**Pros:**
- Official Espressif framework
- Better ESP32 hardware integration
- Official VS Code extension
- More control over WiFi, Bluetooth, power
- Industry standard for ESP32 development

**Setup:**
```bash
cd esp32-c3-demo-idf
# Clone TFT_eSPI library
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
cd ..
# Build with VS Code ESP-IDF extension or:
idf.py set-target esp32c3
idf.py build flash monitor
```

**Documentation:**
- `esp32-c3-demo-idf/README.md` - Full ESP-IDF guide
- `esp32-c3-demo-idf/QUICKSTART.md` - VS Code setup
- `esp32-c3-demo-idf/SETUP_INSTRUCTIONS.md` - Step-by-step

## 🎮 Included Demos (Both Versions)

1. **🕐 Analog Clock** - Smooth sweeping hands
2. **🎲 3D Cube** - Real-time rotating wireframe
3. **📡 WiFi Scanner** - Circular network visualization
4. **📊 Radar Sweep** - Classic radar animation
5. **🐍 Snake Game** - Circular snake with AI
6. **🌈 Plasma** - Real-time plasma effects
7. **✨ Particles** - 50 particle physics system
8. **💚 Matrix Rain** - Falling characters
9. **🎵 Spectrum** - Audio spectrum analyzer

## 🎯 Which Version Should You Use?

### Choose **PlatformIO** if:
- ✅ You want the fastest setup
- ✅ You're familiar with Arduino
- ✅ You prefer simple configuration
- ✅ You want automatic dependency management

### Choose **ESP-IDF** if:
- ✅ You're using the official ESP extension in VS Code *(you mentioned this!)*
- ✅ You want native ESP32 features
- ✅ You need professional/production development
- ✅ You want better hardware control

## 🚀 Quick Start (ESP-IDF Version)

Since you mentioned using the official ESP extension:

1. **Open `esp32-c3-demo-idf/` in VS Code**

2. **Install TFT_eSPI library:**
   ```bash
   cd esp32-c3-demo-idf/components
   git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
   cp ../main/User_Setup.h tft_espi/User_Setup.h
   ```

3. **Set target:**
   - Press F1 → "ESP-IDF: Set Espressif device target" → esp32c3

4. **Build:**
   - Click the 🔨 Build button in status bar

5. **Flash:**
   - Click the ⚡ Flash button in status bar

6. **Done!** The demos will start cycling automatically.

## 📋 Hardware Specs

- **Board:** ESP32-C3FH4 (RISC-V 160MHz)
- **Display:** 0.71" Round IPS LCD, 160x160
- **Driver:** GC9D01
- **Interface:** SPI (40MHz)

**GPIO Pins:**
| Pin | GPIO |
|-----|------|
| MOSI | 7 |
| SCLK | 6 |
| CS | 5 |
| DC | 4 |
| RST | 8 |

## 🔧 Customization

Both versions use identical code! Customize in `main.cpp`:

**Change demo duration:**
```cpp
const int DEMO_DURATION = 10000; // milliseconds
```

**Change auto-cycle time:**
```cpp
if (millis() - lastCycle > 15000) { // 15 seconds
```

**Add your own demo:**
1. Create a new function: `void demoMyDemo() { ... }`
2. Add to the demos array
3. Rebuild and flash

## 📚 Documentation Summary

### PlatformIO Version
| File | Purpose |
|------|---------|
| `README.md` | Complete guide, troubleshooting |
| `QUICKSTART.md` | 5-minute quick start |
| `platformio.ini` | Build configuration |

### ESP-IDF Version
| File | Purpose |
|------|---------|
| `README.md` | Complete ESP-IDF guide |
| `QUICKSTART.md` | Fast VS Code setup |
| `SETUP_INSTRUCTIONS.md` | Detailed step-by-step |
| `CMakeLists.txt` | Build configuration |
| `sdkconfig.defaults` | ESP32 settings |

## 🐛 Common Issues

### Display shows nothing
- Press RESET button
- Check USB cable supports data
- Verify GPIO connections

### Build fails
- PlatformIO: `pio run --target clean`
- ESP-IDF: `idf.py fullclean`

### Can't flash
- Hold BOOT button while connecting
- Check serial port permissions

### WiFi scanner empty
- Takes 1-2 seconds to scan
- Needs 2.4GHz networks nearby

## 💡 What's Next?

Ideas for expansion:
- Add real-time clock (NTP over WiFi)
- Bluetooth audio spectrum
- Temperature/humidity sensors
- Touch input for menu control
- Web interface for demo selection
- Accelerometer for games

## 📖 Full Documentation

For complete details:
- **PlatformIO:** See `esp32-c3-demo/README.md`
- **ESP-IDF:** See `esp32-c3-demo-idf/README.md`

---

**Created with Claude Code**

Both versions pushed to branch: `claude/esp32-c3-demo-app-Qk3WM`
