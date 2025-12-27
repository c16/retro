# NEON DEFENDER - Quick Start ⚡

Get the game running in 10 minutes!

## Step 1: Install TFT_eSPI Library

```bash
cd neon-defender/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
cd ..
```

## Step 2: Build with VS Code (Recommended)

1. **Open project** in VS Code
   - File → Open Folder → Select `neon-defender`

2. **Set target**
   - F1 → "ESP-IDF: Set Espressif device target" → `esp32c3`

3. **Build**
   - Click 🔨 Build button in status bar

4. **Flash**
   - Connect ESP32-C3 via USB
   - Click ⚡ Flash button

5. **Monitor**
   - Click 🖥️ Monitor button
   - See game boot messages

## Step 3: Connect Audio (Optional)

**Simple setup** (quiet):
```
GPIO2 ──[100Ω]── Speaker+ (8Ω)
GND ──────────── Speaker-
```

**Better setup** (louder):
```
GPIO2 → Audio Amplifier → Speaker
```

Recommended: PAM8403 amplifier module (~$2)

## Step 4: Play!

1. **Menu**: Press BOOT button to start
2. **Shoot**: Press BOOT button repeatedly
3. **Auto-aim**: Turret tracks enemies automatically
4. **Survive**: Don't let enemies reach the center!

## Controls

| Button | Action |
|--------|--------|
| BOOT (GPIO9) | Shoot / Start / Select |
| GPIO2* | Rotate left (optional) |
| GPIO3* | Rotate right (optional) |

*External buttons - if not connected, auto-aim is used

## Troubleshooting

### Display blank?
- Press RESET button
- Check USB cable supports data

### No sound?
- GPIO2 needs external speaker
- Try higher volume: Edit `main.cpp`, change `audio.setVolume(100);`
- Use amplifier for better audio

### Build fails?
```bash
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
```

### Flash fails?
- Hold BOOT button while clicking Flash
- Release when "Connecting..." appears

## Command Line Build

```bash
# Set up environment
. $HOME/esp/esp-idf/export.sh

# Build and flash
idf.py set-target esp32c3
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## What to Expect

1. **Boot animation**: Expanding circles with "NEON DEFENDER" text
2. **Menu screen**: Animated title, press BOOT to start
3. **Gameplay**: Enemies spawn at edges, shoot them before they reach center
4. **Audio**: Chiptune music and sound effects from GPIO2
5. **Waves**: Complete waves to progress, boss every 5 waves
6. **Powerups**: Collect floating symbols for abilities
7. **Game Over**: When health reaches zero, high score is saved

## Game Tips

- **Let auto-aim help** - Turret tracks nearest enemy
- **Collect powerups** - 15% drop chance from enemies
- **Build combos** - Chain kills for bonus points
- **Watch your health** - Circular bar around center
- **Boss strategy** - Use shield or nuke powerup

## Performance

- **30 FPS** smooth gameplay
- **22kHz audio** procedurally generated
- **Runs on 4MB flash**, plenty of room for mods

## Next Steps

- Read full [README.md](README.md) for details
- Customize difficulty in `game.cpp`
- Add external buttons for manual control
- Hook up better audio amplifier
- Beat your high score!

---

**Estimated setup time: 5-10 minutes**

**Now go defend that center!** 🎮⚡
