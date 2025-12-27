# NEON DEFENDER 🎮⚡

**An insane circular shooter game for ESP32-C3 with round display**

![Version](https://img.shields.io/badge/version-1.0-blue) ![Platform](https://img.shields.io/badge/platform-ESP32--C3-green) ![Framework](https://img.shields.io/badge/framework-ESP--IDF-red)

## 🌟 Overview

NEON DEFENDER is an over-the-top circular arena shooter featuring:
- **360° circular gameplay** optimized for round displays
- **Wild neon graphics** with particle effects and screen shake
- **Procedural chiptune music** generated in real-time
- **Full sound effects** via PWM audio output
- **5 enemy types** with unique behaviors
- **Epic boss battles** every 5 waves
- **5 powerup types** for tactical advantages
- **Combo system** with score multipliers
- **High score persistence** using NVS

## 🎯 Game Features

### Enemy Types
1. **BASIC** (Red) - Standard enemy, moves straight inward
2. **FAST** (Yellow) - Quick but fragile, hard to hit
3. **ZIGZAG** (Green) - Unpredictable serpentine movement
4. **TANK** (Orange) - Slow but requires multiple hits
5. **BOSS** (Purple) - Massive health, appears every 5 waves

### Powerups
- **⚡ Rapid Fire** (R) - Increased fire rate for 10s
- **💥 Spread Shot** (S) - 5-way spread fire for 10s
- **🛡️ Shield** (B) - Invulnerability for 15s
- **☢️ Nuke** (N) - Destroys all enemies on screen
- **❤️ Health** (+) - Restores 30 health points

### Game Mechanics
- **Auto-aim mode**: Turret automatically tracks nearest enemy
- **Manual control**: Optional buttons for precise aiming
- **Combo system**: Chain kills for score multipliers
- **Wave progression**: Increasing difficulty with each wave
- **Boss battles**: Every 5th wave features a tough boss
- **Health regeneration**: Bonus health between waves

## 🎮 Controls

### Required (Built-in)
- **BOOT Button (GPIO9)**: Shoot / Start Game / Confirm

### Optional (External Buttons)
- **GPIO2**: Rotate Left (manual control mode)
- **GPIO3**: Rotate Right (manual control mode)

*Note: Without optional buttons, the game uses auto-aim mode*

## 🔊 Audio System

The game features a complete audio system with:

### Music
- Procedural chiptune music generated in real-time
- Dynamic tempo at 140 BPM (configurable)
- Bass + lead melody tracks
- Looping 32-note patterns

### Sound Effects
- Shoot, Hit, Explosion
- Powerup collect, Combo milestone
- Wave complete, Game over
- Boss hit, Menu navigation

### Hardware Setup
**Audio Output**: GPIO2 (PWM)

**Connection options:**
1. **Direct speaker** (8Ω, 0.5W max) - Simple but quiet
2. **Amplifier** (recommended) - Connect GPIO2 to amp input
3. **Active speaker** - Use audio jack adapter from GPIO2

**Wiring example:**
```
ESP32-C3 GPIO2 ──┬── 100Ω resistor ──┬── Speaker+ (8Ω)
                 │                   │
                 └── 100µF cap ──────┘
                                     └── Speaker- (GND)
```

## 🛠️ Hardware Requirements

### Required
- **ESP32-C3** with 0.71" Round LCD (GC9D01 driver)
- **Display**: 160x160 pixels, SPI interface
- **GPIO Pins**:
  - GPIO7: TFT_MOSI
  - GPIO6: TFT_SCLK
  - GPIO5: TFT_CS
  - GPIO4: TFT_DC
  - GPIO8: TFT_RST
  - GPIO9: BOOT button (built-in)

### Optional
- **Buttons**: GPIO2, GPIO3 for manual rotation control
- **Speaker/Amplifier**: Connect to GPIO2 for audio

## 🚀 Quick Start

### 1. Prerequisites
- ESP-IDF v5.0+ with VS Code extension
- OR PlatformIO (alternative build system)

### 2. Clone TFT_eSPI Library
```bash
cd neon-defender/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../main/User_Setup.h tft_espi/User_Setup.h
```

### 3. Build with VS Code

**Set Target:**
- Press F1 → "ESP-IDF: Set Espressif device target" → `esp32c3`

**Build:**
- Click 🔨 Build button

**Flash:**
- Click ⚡ Flash button

**Monitor:**
- Click 🖥️ Monitor button

### 4. Build with Command Line
```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure
idf.py set-target esp32c3

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor
```

## 🎨 Graphics Features

- **Neon color palette**: Vibrant RGB colors on black background
- **Particle explosions**: 50 concurrent particles with physics
- **Screen shake**: Dynamic intensity based on events
- **Smooth animations**: 30 FPS gameplay
- **Pulsing effects**: Boss enemies and powerups
- **Trail effects**: Bullets leave motion trails
- **Health bar**: Circular arc around center
- **HUD**: Score, wave number, combo multiplier, powerup indicators

## 📊 Technical Details

### Performance
- **Frame rate**: ~30 FPS (33ms update interval)
- **Audio sample rate**: 22.05kHz
- **Memory usage**: ~80KB RAM
- **Flash usage**: ~400KB

### Architecture
```
main.cpp        - Main game loop, initialization
game.cpp/h      - Core game engine and logic
audio.cpp/h     - PWM audio system and synthesis
User_Setup.h    - TFT_eSPI display configuration
```

### Audio Synthesis
The game uses real-time procedural audio generation:
- **Square waves**: Retro chiptune sounds
- **Sine waves**: Smooth tones and melodies
- **Triangle waves**: Bass and harmonics
- **Noise generation**: Explosion effects
- **Frequency sweeps**: Rising/falling tones
- **Multi-voice mixing**: Up to 5 simultaneous sounds

## 🎯 Gameplay Tips

1. **Master auto-aim**: Let the turret track while you focus on timing
2. **Collect powerups**: They spawn randomly from destroyed enemies
3. **Build combos**: Chain kills for massive score multipliers
4. **Save shield**: Use shield powerup during boss waves
5. **Spread shot strategy**: Best for clearing multiple weak enemies
6. **Nuke timing**: Save nuke for overwhelming situations
7. **Boss patterns**: Learn boss movement to predict attacks
8. **Wave bonuses**: Complete waves fast for extra points

## 📁 Project Structure

```
neon-defender/
├── CMakeLists.txt              # Project build config
├── sdkconfig.defaults          # ESP-IDF settings
├── README.md                   # This file
│
├── main/
│   ├── main.cpp                # Game initialization and loop
│   ├── game.cpp / game.h       # Game engine (~850 lines)
│   ├── audio.cpp / audio.h     # Audio system (~400 lines)
│   ├── User_Setup.h            # TFT display config
│   ├── CMakeLists.txt          # Main component build
│   └── idf_component.yml       # Dependencies
│
└── components/
    └── tft_espi/               # TFT_eSPI library (clone manually)
```

## 🔧 Customization

### Adjust Difficulty
Edit `game.cpp`:
```cpp
// Line ~540: Enemies per wave
m_enemiesThisWave = 5 + waveNum * 3; // Increase multiplier

// Line ~546: Boss wave frequency
if (waveNum % 5 == 0) // Change to 3 for more bosses
```

### Change Music Tempo
```cpp
audio.setMusicTempo(180); // Faster = more intense
```

### Modify Enemy Speed
```cpp
// In spawnEnemy() function, adjust:
m_enemies[i].speed = 0.5; // Lower = easier
```

### Audio Volume
```cpp
audio.setVolume(100); // 0-100
```

## 🐛 Troubleshooting

### Display Issues
**Black screen:**
- Press RESET button
- Check GPIO connections match User_Setup.h
- Verify SPI connections (MOSI, SCLK, CS, DC, RST)

**Corrupted graphics:**
- Lower SPI frequency in User_Setup.h (try 20MHz)
- Check power supply (needs stable 3.3V, >300mA)

### Audio Issues
**No sound:**
- Check GPIO2 has PWM output
- Verify speaker connection (8Ω, not headphones)
- Try increasing volume: `audio.setVolume(100);`

**Distorted audio:**
- Use amplifier instead of direct speaker
- Add 100Ω resistor in series
- Check speaker impedance (8Ω recommended)

### Performance Issues
**Low FPS:**
- Disable serial output in release mode
- Reduce particle count in game.h (MAX_PARTICLES)
- Check compiler optimization in sdkconfig

**Crashes:**
- Increase stack size in sdkconfig.defaults
- Check free heap in serial monitor
- Reduce maximum enemies (MAX_ENEMIES)

### Build Errors
**TFT_eSPI not found:**
```bash
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
```

**Arduino not found:**
- Check internet connection (auto-downloads)
- Run: `idf.py fullclean && idf.py build`

## 🎮 Game States

```
MENU → PLAYING → WAVE_COMPLETE → PLAYING → ... → GAMEOVER → MENU
  ↓       ↑            ↓              ↑
  └───────┴────────────┴──────────────┘
```

### Menu
- Animated title with rainbow colors
- Rotating circle effects
- High score display
- Press BOOT to start

### Playing
- Active gameplay with enemies, bullets, powerups
- Real-time score and combo tracking
- Health bar and wave indicator
- Procedural music playing

### Wave Complete
- 3-second intermission
- Wave bonus added to score
- Health regeneration
- Auto-advance to next wave

### Game Over
- Final score display
- High score saved if beaten
- 3-second delay before returning to menu

## 📜 License

This project is provided as-is for educational and entertainment purposes.

## 👏 Credits

- **Game Engine**: Custom C++ implementation
- **Display Library**: TFT_eSPI by Bodmer
- **Framework**: ESP-IDF by Espressif
- **Hardware**: ESP32-C3 with GC9D01 round display
- **Created with**: Claude Code

## 🌈 Future Enhancements

Ideas for extending the game:
- [ ] WiFi leaderboard system
- [ ] Multiple difficulty modes
- [ ] Unlockable weapons
- [ ] Achievement system
- [ ] Co-op mode (two players)
- [ ] Custom music tracks
- [ ] Touch input support
- [ ] Bluetooth gamepad support
- [ ] Replay system
- [ ] Boss rush mode

## 📞 Support

Having issues? Check:
1. This README troubleshooting section
2. Serial monitor output for error messages
3. ESP-IDF documentation for platform issues
4. TFT_eSPI GitHub for display problems

---

**Defend the center. Chase the high score. Embrace the NEON!** ⚡🎮✨
