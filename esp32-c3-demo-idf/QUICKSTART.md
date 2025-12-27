# Quick Start Guide - ESP-IDF Version

Get your ESP32-C3 round display running with VS Code and ESP-IDF extension!

## 1. Install Prerequisites

### Install VS Code and ESP-IDF Extension

1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Open VS Code
3. Go to Extensions (Ctrl+Shift+X)
4. Search for "ESP-IDF"
5. Install the official **ESP-IDF** extension by Espressif
6. Follow the extension's setup wizard to install ESP-IDF

The extension will automatically install:
- ESP-IDF framework
- Toolchain for ESP32-C3
- Python dependencies
- All required tools

## 2. Clone TFT_eSPI Library

Open a terminal in the project directory:

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
```

Then link the configuration file:

```bash
cp ../main/User_Setup.h tft_espi/User_Setup.h
```

## 3. Open Project in VS Code

1. File → Open Folder
2. Select `esp32-c3-demo-idf` folder
3. VS Code will detect it as an ESP-IDF project

## 4. Configure, Build, and Flash

### Set Target (First Time Only)

1. Press `F1` (or Ctrl+Shift+P)
2. Type: **ESP-IDF: Set Espressif device target**
3. Select: **esp32c3**

### Build

Click the **Build** button (🔨) in the bottom status bar

Or:
- Press `F1`
- Type: **ESP-IDF: Build your project**

### Connect Hardware

1. Connect your ESP32-C3 board via USB-C cable
2. The board should appear as a serial port

### Flash

Click the **Flash** button (⚡) in the bottom status bar

Or:
- Press `F1`
- Type: **ESP-IDF: Flash your project**

### Monitor

Click the **Monitor** button (🖥️) in the bottom status bar

Or:
- Press `F1`
- Type: **ESP-IDF: Monitor device**

## 5. Watch the Demos!

The display will show:
1. Boot animation with concentric circles
2. Circular demo selection menu
3. Automatic cycling through 9 different demos

Each demo runs for 10-15 seconds before transitioning to the next.

## Troubleshooting

### ESP-IDF Extension Not Found
Make sure you're using the official Espressif extension, not third-party ones.

### Build Fails - "Arduino.h not found"
The Arduino component should download automatically. If not:
- Check internet connection
- Run: ESP-IDF: Fullclean project
- Try building again

### Build Fails - "TFT_eSPI.h not found"
Make sure you cloned TFT_eSPI into `components/tft_espi/`

### Flash Fails - "Cannot Connect"
1. Press and hold **BOOT** button on board
2. Click **Flash** button
3. Release BOOT button when flashing starts

### Display is Blank
1. Press the **RESET** button on the board
2. Check USB cable supports data (not just charging)
3. Check GPIO connections

### Port Permission Denied (Linux)
```bash
sudo usermod -a -G dialout $USER
# Log out and log back in
```

## Using the Command Line Instead

If you prefer command line:

```bash
# Set up environment (do this in every new terminal)
. $HOME/esp/esp-idf/export.sh

# Navigate to project
cd esp32-c3-demo-idf

# Install TFT_eSPI
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
cd ..

# Set target
idf.py set-target esp32c3

# Build
idf.py build

# Flash and monitor (replace PORT with your serial port)
idf.py -p /dev/ttyACM0 flash monitor

# Exit monitor: Ctrl+]
```

## Next Steps

- Modify demo durations in `main/main.cpp`
- Add your own demos!
- Connect sensors or inputs
- Enable WiFi features
- Read the full [README.md](README.md)

## Estimated Time

From zero to running: **10-20 minutes** (including ESP-IDF installation)

If ESP-IDF already installed: **5 minutes**

---

**Happy Coding!** 🚀
