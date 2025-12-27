# Setup Instructions for ESP-IDF Version

## Step-by-Step Setup

### 1. Install ESP-IDF Extension in VS Code

1. Open Visual Studio Code
2. Click on Extensions icon (or press `Ctrl+Shift+X`)
3. Search for "**ESP-IDF**"
4. Install the official extension by **Espressif Systems**
5. After installation, the extension will guide you through:
   - Installing ESP-IDF framework
   - Installing ESP32 toolchains
   - Installing Python packages

### 2. Install TFT_eSPI Library

The TFT_eSPI library needs to be manually cloned into the components directory.

**Open a terminal in the project root** and run:

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cd ..
```

**Copy the configuration file:**

```bash
cp main/User_Setup.h components/tft_espi/User_Setup.h
```

This configures TFT_eSPI for the GC9D01 display with the correct GPIO pins.

### 3. Open Project in VS Code

1. **File** → **Open Folder**
2. Navigate to and select the `esp32-c3-demo-idf` folder
3. Click **Select Folder**

VS Code should detect this as an ESP-IDF project.

### 4. Set ESP32-C3 as Target

1. Press `F1` (or `Ctrl+Shift+P`)
2. Type: `ESP-IDF: Set Espressif device target`
3. Select: `esp32c3`
4. Wait for configuration to complete

### 5. Build the Project

**Option A: Using UI**
- Click the **Build** button (🔨 hammer icon) in the bottom status bar

**Option B: Using Command Palette**
1. Press `F1`
2. Type: `ESP-IDF: Build your project`
3. Press Enter

The first build will take several minutes as it downloads dependencies and builds the Arduino component.

### 6. Connect Your ESP32-C3

1. Connect the ESP32-C3 board to your computer via USB-C cable
2. The board should appear as a serial port:
   - Linux: `/dev/ttyACM0` or `/dev/ttyUSB0`
   - Windows: `COM3`, `COM4`, etc.
   - macOS: `/dev/cu.usbserial-*`

### 7. Flash the Firmware

**Option A: Using UI**
- Click the **Flash** button (⚡ lightning icon) in the bottom status bar

**Option B: Using Command Palette**
1. Press `F1`
2. Type: `ESP-IDF: Flash your project`
3. Select your serial port if prompted

**If flashing fails:**
- Press and hold the **BOOT** button on the board
- Click Flash again
- Release BOOT when "Connecting..." appears

### 8. Monitor Serial Output

**Option A: Using UI**
- Click the **Monitor** button (🖥️ monitor icon) in the bottom status bar

**Option B: Using Command Palette**
1. Press `F1`
2. Type: `ESP-IDF: Monitor device`

You should see:
```
ESP32-C3 Round Display Demo Starting...
Total demos: 9
Starting demo: Analog Clock
...
```

To exit monitor: Press `Ctrl+]`

## Project Structure

After setup, your project should look like this:

```
esp32-c3-demo-idf/
├── CMakeLists.txt                 # Project build configuration
├── sdkconfig.defaults             # Default ESP-IDF settings
├── README.md                      # Full documentation
├── QUICKSTART.md                  # Quick reference guide
├── SETUP_INSTRUCTIONS.md          # This file
│
├── main/
│   ├── CMakeLists.txt             # Main component build config
│   ├── idf_component.yml          # Arduino dependency
│   ├── main.cpp                   # Your application code
│   └── User_Setup.h               # TFT_eSPI configuration
│
└── components/
    └── tft_espi/                  # TFT_eSPI library (you clone this)
        ├── TFT_eSPI.h
        ├── TFT_eSPI.cpp
        ├── User_Setup.h           # Copied from main/
        └── ...
```

## Verifying Your Setup

### Check ESP-IDF Installation

Open a terminal and run:
```bash
# This should print ESP-IDF version
idf.py --version
```

Should show something like: `ESP-IDF v5.1.2` or higher

### Check TFT_eSPI Installation

Verify the library exists:
```bash
ls components/tft_espi/TFT_eSPI.h
```

Should show: `components/tft_espi/TFT_eSPI.h`

### Check User Setup

Verify configuration was copied:
```bash
head components/tft_espi/User_Setup.h
```

Should show:
```c
// TFT_eSPI User Setup for ESP32-C3 with GC9D01 Round Display
#define USER_SETUP_INFO "ESP32_C3_GC9D01"
#define GC9A01_DRIVER
...
```

## Common Issues and Solutions

### Issue: "Arduino.h: No such file or directory"

**Solution:**
The Arduino component didn't download automatically.

1. Check internet connection
2. Run clean build:
   - `F1` → `ESP-IDF: Full Clean`
   - `F1` → `ESP-IDF: Build your project`

### Issue: "TFT_eSPI.h: No such file or directory"

**Solution:**
TFT_eSPI library is not in the right place.

Verify:
```bash
ls components/tft_espi/TFT_eSPI.h
```

If missing, clone it:
```bash
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cd ..
```

### Issue: Display shows nothing after flashing

**Solutions:**
1. Press the **RESET** button on the board
2. Check that USB cable supports data transfer (not just power)
3. Verify GPIO connections match hardware
4. Try re-flashing:
   ```bash
   idf.py -p /dev/ttyACM0 erase-flash
   idf.py -p /dev/ttyACM0 flash
   ```

### Issue: Permission denied on serial port (Linux)

**Solution:**
Add your user to the dialout group:
```bash
sudo usermod -a -G dialout $USER
```

Then log out and log back in.

### Issue: Build is very slow

**First build is always slow** (5-10 minutes) because it:
- Downloads Arduino component
- Compiles entire ESP-IDF
- Compiles all libraries

Subsequent builds are much faster (10-30 seconds).

### Issue: "Target mismatch" error

**Solution:**
Wrong target was set.

Fix:
1. `F1` → `ESP-IDF: Set Espressif device target`
2. Select `esp32c3`
3. Rebuild

## Alternative: Command Line Setup

If you prefer the command line:

```bash
# 1. Install ESP-IDF (one-time setup)
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c3

# 2. Set up environment (do this in every new terminal)
. ~/esp/esp-idf/export.sh

# 3. Navigate to project
cd /path/to/esp32-c3-demo-idf

# 4. Install TFT_eSPI
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
cd ..

# 5. Set target
idf.py set-target esp32c3

# 6. Build
idf.py build

# 7. Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor

# To exit monitor: Ctrl+]
```

## Getting Help

### ESP-IDF Resources
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/)
- [ESP-IDF GitHub Issues](https://github.com/espressif/esp-idf/issues)
- [ESP32 Forums](https://esp32.com/)

### Extension Resources
- [ESP-IDF Extension Guide](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md)

### TFT_eSPI Resources
- [TFT_eSPI Documentation](https://github.com/Bodmer/TFT_eSPI)

---

**Once everything is working, you can delete this file!**
