# Quick Start Guide

Get your ESP32-C3 round display running in 5 minutes!

## 1. Install PlatformIO

### VS Code (Recommended)
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install the PlatformIO extension from VS Code marketplace
3. Restart VS Code

### CLI Alternative
```bash
# Install PlatformIO Core
pip install platformio
```

## 2. Connect Hardware

1. Connect ESP32-C3 board to computer via USB-C cable
2. Display should already be connected (soldered on board)
3. Note the serial port (e.g., `/dev/ttyACM0` or `COM3`)

## 3. Build and Upload

### Using VS Code
1. Open the `esp32-c3-demo` folder in VS Code
2. Wait for PlatformIO to initialize (bottom toolbar)
3. Click the **→** (Upload) button in the bottom toolbar
4. Watch the magic happen!

### Using CLI
```bash
cd esp32-c3-demo
pio run --target upload --target monitor
```

## 4. Enjoy the Show!

The demo will automatically cycle through 9 different displays:
- Analog Clock
- 3D Rotating Cube
- WiFi Network Scanner
- Radar Sweep
- Snake Game
- Plasma Effects
- Particle System
- Matrix Rain
- Spectrum Analyzer

Each demo runs for 10-15 seconds before transitioning to the next.

## Troubleshooting

### "Device not found" error
Press and hold the **BOOT** button while plugging in the USB cable.

### Nothing on display
Check that:
- USB cable supports data (not just power)
- Correct board selected in `platformio.ini`
- Display is properly soldered/connected

### Upload works but display is blank
Try pressing the **RESET** button on the board.

## Next Steps

- Read the full [README.md](README.md) for customization options
- Modify demo durations and colors in `src/main.cpp`
- Add your own demos!
- Share your creation!

## Questions?

Check the [Troubleshooting section](README.md#troubleshooting) in the main README.

---

**Estimated time from zero to running: 5-10 minutes**
