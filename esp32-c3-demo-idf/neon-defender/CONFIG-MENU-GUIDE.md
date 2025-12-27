# NEON DEFENDER - Configuration Menu Guide

## Accessing the Menu

Connect to the ESP32-C3 via USB serial terminal (115200 baud) and **press 'M'** at any time to open the configuration menu.

## Serial Terminal Setup

### Using VS Code ESP-IDF Extension
- Click the 🖥️ **Monitor** button in the status bar
- Press 'M' to open menu

### Using Command Line
```bash
idf.py monitor
# Press 'M' for menu
# Press Ctrl+] to exit monitor
```

### Using PuTTY (Windows)
1. Open PuTTY
2. Select "Serial" connection
3. Set COM port (check Device Manager)
4. Set Speed: 115200
5. Click "Open"
6. Press 'M' for menu

### Using screen (Linux/Mac)
```bash
screen /dev/ttyACM0 115200
# Press 'M' for menu
# Press Ctrl+A then K to exit
```

## Menu Structure

```
MAIN MENU
├── 1. Audio Settings
│   ├── Volume (0-100)
│   ├── Music Tempo (80-200 BPM)
│   ├── Music Enabled (ON/OFF)
│   └── Sound Effects Enabled (ON/OFF)
│
├── 2. Gameplay Settings
│   ├── Difficulty (Easy/Normal/Hard/Insane)
│   ├── Starting Health (50-200)
│   ├── Powerup Drop Rate (0-100%)
│   ├── Enemy Speed Multiplier (0.5-2.0x)
│   └── Base Enemies Per Wave (3-10)
│
├── 3. Display Settings
│   ├── Brightness (0-100)
│   ├── Show FPS Counter (ON/OFF)
│   ├── Screen Shake Effects (ON/OFF)
│   └── Particle Effects (ON/OFF)
│
├── 4. Control Settings
│   ├── Auto-Aim Enabled (ON/OFF)
│   └── Rotation Speed (1-10)
│
├── 5. Advanced Settings
│   ├── Max Particles (10-100)
│   ├── Max Enemies (5-30)
│   └── Debug Mode (ON/OFF)
│
├── V. View All Settings
├── S. Save Settings
├── R. Reset to Defaults
└── Q. Quit Menu
```

## Setting Descriptions

### Audio Settings

**Volume (0-100)**
- Controls master audio volume
- Default: 70
- Tip: Use lower values (20-40) if audio distorts

**Music Tempo (80-200 BPM)**
- Speed of procedural background music
- Default: 140 BPM
- Lower = slower/calmer, Higher = faster/intense

**Music Enabled**
- Turn background music on/off
- Default: ON
- SFX still play when music is off

**Sound Effects Enabled**
- Turn all sound effects on/off
- Default: ON
- Music still plays when SFX is off

### Gameplay Settings

**Difficulty**
- **Easy (0)**: Slower enemies, more health
- **Normal (1)**: Balanced gameplay **(default)**
- **Hard (2)**: Faster enemies, less health
- **Insane (3)**: Maximum challenge!

**Starting Health (50-200)**
- Health points at game start
- Default: 100
- More health = easier survival

**Powerup Drop Rate (0-100%)**
- Chance enemies drop powerups when destroyed
- Default: 15%
- Higher = more powerups, easier game

**Enemy Speed Multiplier (0.5-2.0x)**
- Multiplies all enemy speeds
- Default: 1.0x
- 0.5x = half speed, 2.0x = double speed

**Base Enemies Per Wave (3-10)**
- Number of enemies in wave 1
- Default: 5
- Increases each wave (formula: base + wave × 3)

### Display Settings

**Brightness (0-100)**
- Display brightness level
- Default: 100
- Lower to save power or reduce eye strain

**Show FPS Counter**
- Display frame rate on screen
- Default: OFF
- Useful for performance monitoring

**Screen Shake Effects**
- Camera shake on explosions/hits
- Default: ON
- Turn OFF if motion bothers you

**Particle Effects**
- Explosion particles and visual effects
- Default: ON
- Turn OFF for better performance

### Control Settings

**Auto-Aim Enabled**
- Automatically track nearest enemy
- Default: ON
- Turn OFF for manual aiming challenge

**Rotation Speed (1-10)**
- Speed of manual rotation (if using buttons)
- Default: 5
- Higher = faster rotation

### Advanced Settings

**Max Particles (10-100)**
- Maximum concurrent particles
- Default: 50
- Lower for better performance on complex scenes

**Max Enemies (5-30)**
- Maximum enemies on screen simultaneously
- Default: 20
- Lower for better performance

**Debug Mode**
- Show debug information in serial monitor
- Default: OFF
- Useful for troubleshooting

## Usage Examples

### Example 1: Make Game Easier
1. Press 'M' to open menu
2. Press '2' for Gameplay Settings
3. Press '1' and enter '0' (Easy difficulty)
4. Press '2' and enter '150' (More health)
5. Press '3' and enter '30' (More powerups)
6. Press 'B' to go back
7. Press 'S' to save
8. Press 'Q' to quit

### Example 2: Adjust Audio
1. Press 'M'
2. Press '1' for Audio Settings
3. Press '1' and enter desired volume (e.g., '50')
4. Press '2' and enter tempo (e.g., '180' for faster)
5. Press 'B' and 'S' to save
6. Press 'Q' to quit

### Example 3: Performance Optimization
1. Press 'M'
2. Press '5' for Advanced Settings
3. Press '1' and enter '30' (fewer particles)
4. Press '2' and enter '10' (fewer enemies)
5. Go back and enter Display Settings
6. Toggle screen shake OFF
7. Save and quit

### Example 4: Insane Challenge Mode
1. Press 'M' → '2' (Gameplay)
2. Set Difficulty to 3 (Insane)
3. Set Starting Health to 50
4. Set Enemy Speed to 1.5x
5. Set Powerup Drop Rate to 5%
6. Save and prepare for pain! 😈

## Configuration Persistence

- Settings are **automatically saved to flash memory** when you press 'S'
- Settings **persist across reboots**
- To restore factory defaults, press 'R' in main menu
- Settings are stored in NVS (Non-Volatile Storage)

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| M/m | Open config menu (from game) |
| 1-5 | Navigate menu sections |
| B/b | Back to previous menu |
| V/v | View all current settings |
| S/s | Save settings to flash |
| R/r | Reset to factory defaults |
| Q/q | Quit menu and return to game |

## Tips

### Audio Tips
- If sound is distorted: Lower volume to 40-50
- No sound: Check GPIO2 connection to speaker
- For silence: Disable both Music and SFX

### Gameplay Tips
- Start on Easy to learn the game
- Adjust enemy speed for custom difficulty
- Higher powerup drop rate makes game more forgiving
- Lower starting health for extra challenge

### Performance Tips
- Lower Max Particles if game lags
- Lower Max Enemies for smoother gameplay
- Disable Screen Shake saves a bit of processing
- Show FPS to monitor performance

### Custom Modes
- **Bullet Hell**: Max enemies (30), Fast enemies (2.0x), Low health (50)
- **Zen Mode**: Easy difficulty, High health (200), Slow enemies (0.5x)
- **Speed Run**: Normal difficulty, High drop rate (50%), Fast tempo (200)
- **Hardcore**: Insane difficulty, Min drop rate (5%), No shake, No auto-aim

## Troubleshooting

**Menu doesn't open**
- Make sure you're pressing lowercase or uppercase 'M'
- Check serial monitor is connected at 115200 baud
- Try reconnecting USB cable

**Changes don't apply**
- Make sure to press 'S' to save settings
- Settings apply immediately but aren't persistent without saving
- Try pressing 'Q' to exit menu and test

**Game crashes after changing settings**
- Reset to defaults with 'R'
- Avoid extreme values (e.g., Max Enemies > 30)
- Check free heap in serial monitor

**Can't type numbers**
- Make sure serial monitor supports input
- Try different terminal (screen/PuTTY work well)
- Check cursor is visible in input field

## Serial Monitor Recommendations

**Best:** VS Code ESP-IDF Monitor
- Built-in, works perfectly
- Supports all features

**Good:** screen (Linux/Mac), PuTTY (Windows)
- Reliable, full feature support

**OK:** Arduino Serial Monitor
- Works but limited features
- May need to enable line endings

**Avoid:** Some basic terminals
- May not support input buffering properly

## Example Session

```
> M
============================================================
  NEON DEFENDER - CONFIGURATION MENU
============================================================

  [1] Audio Settings              : >
  [2] Gameplay Settings           : >
  [3] Display Settings            : >
  [4] Control Settings            : >
  [5] Advanced Settings           : >

------------------------------------------------------------
  [V] View All Settings           :
  [S] Save Settings               :
  [R] Reset to Defaults           :
  [Q] Quit Menu                   :
------------------------------------------------------------

Choice: 1

============================================================
  AUDIO SETTINGS
============================================================

  [1] Volume                      : 70
  [2] Music Tempo (BPM)           : 140
  [3] Music Enabled               : ON
  [4] Sound Effects Enabled       : ON

------------------------------------------------------------
  [B] Back to Main Menu           :
------------------------------------------------------------

Choice: 1

Volume (0-100): 85

Choice: B
Choice: S

✓ Settings saved to flash!
Settings applied!

Press Enter to continue...

Choice: Q

Exiting menu...
```

---

**Have fun customizing your NEON DEFENDER experience!** ⚙️🎮
