# NEON DEFENDER - Serial Control Guide 🎮⌨️

Play the game entirely through your USB serial terminal!

## Quick Start

1. **Connect serial terminal** at 115200 baud
2. **Press 'H'** to see all controls
3. **Press SPACE** to start shooting!

## Why Serial Controls?

- **Play without physical buttons** - Use your keyboard instead
- **Remote gameplay** - Control via SSH/remote terminal
- **Testing & debugging** - Automated testing scripts
- **Accessibility** - Different input method
- **Development** - Quick testing without hardware

## Complete Control Reference

### Game Controls

| Key | Action | Description |
|-----|--------|-------------|
| **SPACE** or **F** | Shoot/Fire | Fire bullets at enemies |
| **A** or **←** | Rotate Left | Turn turret counterclockwise |
| **D** or **→** | Rotate Right | Turn turret clockwise |
| **ENTER** or **S** | Start/Select | Start game or confirm |
| **P** | Pause/Unpause | Toggle game pause |
| **R** | Restart | Return to main menu |

### System Commands

| Key | Action | Description |
|-----|--------|-------------|
| **M** | Config Menu | Open configuration menu |
| **H** or **?** | Help | Show complete control help |
| **E** | Toggle Controls | Enable/disable serial controls |

### Debug Commands

**(Requires Debug Mode enabled in Config Menu)**

| Key | Action | Description |
|-----|--------|-------------|
| **1** | Spawn Powerup | Random powerup at random position |
| **2** | Add Health | +50 health points |
| **3** | Skip Wave | Complete current wave instantly |
| **4** | Kill All | Destroy all enemies on screen |

## How to Play

### Step 1: Connect Serial Terminal

**VS Code (Recommended):**
```
Click Monitor button (🖥️) in status bar
```

**Command Line:**
```bash
idf.py monitor
# or
screen /dev/ttyACM0 115200
```

**PuTTY (Windows):**
- Connection type: Serial
- Speed: 115200
- Flow control: None

### Step 2: Start the Game

```
Press SPACE or ENTER to start from menu
```

### Step 3: Play!

- **Hold A/D** to rotate your turret
- **Tap SPACE** rapidly to shoot
- **Watch enemies** spiral in from the edges
- **Collect powerups** (R, S, B, N, +)
- **Survive waves** and beat your high score!

## Control Modes

### Auto-Aim Mode (Default)
When you're not pressing A or D, the turret automatically tracks the nearest enemy. Just focus on shooting!

**Best for:**
- Beginners
- Fast-paced action
- Focusing on shooting timing

### Manual Aim Mode
Press A or D to take manual control. Auto-aim disables while you're rotating.

**Best for:**
- Precise targeting
- Advanced players
- Strategic positioning

## Serial Terminal Tips

### Keyboard Layout

```
     [Q] [W] [E] [R]     Q = Quit menu
 [A] [S] [D] [F]         A/D = Rotate, F = Fire, S = Start
```

### Optimal Setup

1. **Use a dedicated serial terminal**
   - Not the VS Code integrated terminal
   - Better keyboard responsiveness

2. **Disable line buffering**
   - Characters send immediately
   - No need to press Enter

3. **Focus the terminal window**
   - Ensure it has keyboard focus
   - No key delay

### Shooting Techniques

**Rapid Fire:**
```
Tap SPACE repeatedly
. . . . . . (visual dots appear)
```

**Spray and Pray:**
```
Hold A or D while tapping SPACE
Rotating fire pattern
```

**Precision Shots:**
```
Stop rotation (release A/D)
Wait for auto-aim to lock
Tap SPACE once
```

## Debug Mode Usage

### Enable Debug Mode

```
1. Press 'M' (Config Menu)
2. Press '5' (Advanced Settings)
3. Press '3' (Toggle Debug Mode)
4. Press 'B' (Back)
5. Press 'S' (Save)
6. Press 'Q' (Quit)
```

### Debug Commands

**Spawn Powerup (1)**
```
> 1
🎁 Spawning powerup...
```
Random powerup appears on screen.

**Add Health (2)**
```
> 2
❤️ +50 Health
```
Instantly heal 50 HP.

**Skip Wave (3)**
```
> 3
⏭️ Skipping wave...
```
Complete current wave, advance to next.

**Kill All Enemies (4)**
```
> 4
💥 Killing all enemies!
```
Instant screen clear.

### Testing Scenarios

**Powerup Testing:**
```
1. Start game
2. Press 1 repeatedly to spawn different powerups
3. Test each powerup effect
```

**Boss Testing:**
```
1. Start game
2. Press 3 four times (skip to wave 5)
3. Boss appears!
```

**Survival Mode:**
```
1. Start game
2. Press 2 when health low (heal)
3. Press 4 when overwhelmed (clear)
4. See how far you can go!
```

## Advanced Features

### Pause & Resume

```
During gameplay:
P → ⏸️ GAME PAUSED
P → ▶️ GAME RESUMED
```

Music stops when paused, resumes when unpaused.

### Quick Restart

```
Any time during game:
R → 🔄 GAME RESTARTED
```

Returns to main menu, resets score.

### Toggle Serial Controls

```
E → ✗ Serial controls disabled
E → ✓ Serial controls enabled
```

Useful if you want physical buttons only.

## Combining Physical & Serial Controls

**Both work simultaneously!**

Example setups:

**Hybrid Mode:**
- Physical BOOT button for shooting
- Serial A/D for rotation
- Best of both worlds!

**Testing Mode:**
- Physical buttons for normal play
- Serial debug keys for testing
- Switch between testing and playing

**Accessibility Mode:**
- Whatever works best for you!
- Mix and match as needed

## Troubleshooting

### Keys Not Working

**Problem:** Pressing keys does nothing

**Solutions:**
1. Check serial terminal has focus
2. Verify 115200 baud rate
3. Press 'E' to check if controls are enabled
4. Try different terminal app

### Delayed Response

**Problem:** Keys lag or buffer

**Solutions:**
1. Use screen or PuTTY instead of built-in terminals
2. Disable line buffering in terminal settings
3. Close other serial connections
4. Check CPU usage on computer

### Debug Commands Don't Work

**Problem:** Pressing 1-4 shows warning

**Solution:**
```
⚠️ Debug commands disabled. Enable in Config Menu (M → 5 → 3)
```
Follow the instructions to enable debug mode.

### Can't Type in Config Menu

**Problem:** Number/letter keys don't work in menu

**Solution:**
- Config menu works fine with serial!
- Type normally, press Enter when prompted
- If stuck, press 'Q' to quit menu

### Shooting Too Slow

**Problem:** SPACE key delay

**Solution:**
- Some terminals buffer keystrokes
- Try tapping faster
- Use 'F' key instead of SPACE
- Enable Rapid Fire powerup (R)
- Adjust fire rate in config menu

## Performance Tips

### Optimal Terminal Settings

**screen (Linux/Mac):**
```bash
screen /dev/ttyACM0 115200
# Ctrl+A then K to exit
```

**PuTTY (Windows):**
- Terminal → Keyboard: Function keys = VT100+
- Terminal → Line discipline: Force off
- Connection → Serial: Flow control = None

**minicom:**
```bash
minicom -D /dev/ttyACM0 -b 115200
# Ctrl+A then X to exit
```

### Frame Rate Optimization

If game lags with serial input:
1. Config Menu (M)
2. Display Settings (3)
3. Disable Screen Shake
4. Reduce Max Particles (Advanced → 1)

## Scripting & Automation

### Automated Testing Script

```bash
#!/bin/bash
# Send commands to ESP32-C3

echo " " > /dev/ttyACM0  # Start game
sleep 2

# Rapid fire test
for i in {1..50}; do
    echo " " > /dev/ttyACM0
    sleep 0.1
done

# Rotation test
for i in {1..20}; do
    echo "a" > /dev/ttyACM0
    sleep 0.2
done
```

### Remote Play

```bash
# SSH into machine with ESP32-C3
ssh user@host

# Attach to screen session
screen -r esp32

# Play remotely!
```

## Keyboard Layout Reference

```
┌─────────────────────────────────────┐
│  ESC  [1]  [2]  [3]  [4]            │
│       Pow  Heal Skip Kill           │
│                                     │
│  [Q] [W] [E] [R] [T]                │
│            Tog Rst                  │
│                                     │
│  [A] [S] [D] [F] [G] [H]            │
│   ←  Strt  →  Fire    Help          │
│                                     │
│  [Z] [X] [C] [M] [P]                │
│                Menu  Pause          │
│                                     │
│  [SPACE] = Shoot                    │
│  [ENTER] = Start                    │
└─────────────────────────────────────┘
```

## FAQ

**Q: Can I play without the physical board?**
A: Almost! You need the ESP32-C3 connected via USB, but you don't need to touch the BOOT button. The display still shows the game.

**Q: Does this work over WiFi/Bluetooth?**
A: Currently USB serial only. WiFi/BT control would be a future enhancement.

**Q: Can I remap the keys?**
A: Not currently, but you could modify `serial_control.cpp` to customize key bindings.

**Q: Does this affect performance?**
A: Minimal impact. Serial input is checked once per frame (~30Hz).

**Q: Can I use a game controller?**
A: Not directly, but you could use software to map controller buttons to keyboard keys.

**Q: Are there key combinations?**
A: No combo keys currently. Each key is independent.

---

**Now go play via serial and show those enemies who's boss!** ⌨️🎮💥
