# MARBLE RUNNER 🎮✨

**Realistic 3D marble game for ESP32-C3 with physics-based gameplay and stunning graphics**

![Platform](https://img.shields.io/badge/platform-ESP32--C3-green) ![Framework](https://img.shields.io/badge/framework-ESP--IDF-red) ![Display](https://img.shields.io/badge/display-160x160-blue)

## Overview

MARBLE RUNNER is an action-packed 3D marble game featuring realistic graphics, physics-based gameplay, and single-button control. Guide your marble along a circular track, avoid obstacles, collect powerups, and chase the high score!

### Key Features

🎨 **Realistic Graphics:**
- 3D spherical marble with Phong shading
- Specular highlights and reflections
- Swirled texture pattern on marble
- Soft shadows underneath marble
- 3D track with gradient lighting
- Particle effects for impacts

⚡ **Physics Engine:**
- Gravity and vertical velocity
- Friction and air resistance
- Realistic bouncing
- Circular track constraints
- Smooth acceleration

🎯 **Gameplay:**
- Single button control (jump + speed boost)
- 4 obstacle types with unique behaviors
- 4 collectible types with different powerups
- Progressive difficulty (levels get harder)
- Score system with lives
- Camera shake effects

## Game Mechanics

### Controls

**BOOT Button (GPIO9):**
- **TAP** - Make marble jump
- **HOLD** - Speed boost (1.5x speed)

### Obstacles

**🕳️ Black Hole** (Black circle)
- Fall through and lose a life
- Avoid at all costs!

**📐 Yellow Bump** (Yellow triangle)
- Launches marble into the air
- Use to jump over holes
- +10 points for using

**🧱 Red Barrier** (Red rectangle)
- Solid wall you bounce off
- Causes camera shake
- -20 points for hitting

**🌀 Purple Spinner** (Rotating arms)
- Spins marble in random direction
- Hard to control after hitting
- Creates chaotic movement

### Collectibles

**🪙 Yellow Coin** (60% spawn rate)
- +100 points
- Main scoring item

**⚡ Cyan Lightning** (20% spawn rate)
- Temporary speed boost (2x)
- +50 points
- Makes marble faster

**⭐ Blue Star** (15% spawn rate)
- 3 seconds of invincibility
- Destroys obstacles instead of taking damage
- +50 points per destroyed obstacle
- Pulsing cyan effect

**❤️ Green Heart** (5% spawn rate)
- Extra life
- +200 points
- Very rare!

### Scoring

- **Coin**: +100 points
- **Bump launch**: +10 points
- **Barrier hit**: -20 points
- **Invincible obstacle destroy**: +50 points
- **Level up**: +500 points
- **Distance**: Tracked automatically

### Lives System

- Start with **3 lives**
- Lose life when:
  - Falling into hole
  - Falling off track edges
- Collect hearts for extra lives
- Game over at 0 lives

### Level System

- Level up every **500 distance units**
- Track speed increases: `30 + level × 5 fps`
- More obstacles spawn
- +500 points per level

## Graphics Technology

### Marble Rendering

The marble uses **per-pixel 3D shading**:

```cpp
// For each pixel within marble radius:
1. Calculate 3D surface normal
2. Compute diffuse lighting (dot product with light)
3. Apply swirled texture pattern
4. Add specular highlight (Phong reflection)
5. Blend colors and output
```

**Texture Pattern:**
- Sinusoidal swirl based on surface normal
- Rotates with marble movement
- Blends reddish-white colors
- Creates realistic marble look

**Lighting:**
- Directional light from top-left
- Diffuse component (ambient 30% + diffuse 70%)
- Specular highlights (power 32 for shininess)
- Fresnel effect (not yet implemented)

### Shadow System

Soft shadows with multiple layers:
- 3 concentric circles with decreasing opacity
- Shadow shrinks when marble is in air
- Offset slightly to bottom-right
- Semi-transparent blending

### Track Rendering

Circular track with 3D effect:
- 32 segments for smooth curve
- Each segment lit based on angle
- Simulates directional lighting
- Darker on bottom, lighter on top
- Inner and outer edge lines

### Particle System

Up to 30 concurrent particles:
- Spawn on impacts, collisions, collections
- Physics-based movement
- Velocity damping (0.95x per frame)
- Lifetime fading
- Different colors for different events

## Technical Details

### Performance

- **Frame Rate**: 30-60 FPS (depending on particle count)
- **Update Rate**: 10ms per frame
- **Physics Rate**: Variable deltaTime (capped at 0.1s)
- **Memory Usage**: ~15KB RAM
- **Display**: 160x160 pixels, full resolution

### Physics Constants

```cpp
GRAVITY = 9.8 m/s²             // Realistic Earth gravity
FRICTION = 0.98                // 2% speed loss per frame
AIR_RESISTANCE = 0.99          // 1% speed loss in air
MAX_SPEED = 150 units/s        // Terminal velocity
ACCELERATION = 80 units/s²     // Speed boost acceleration
JUMP_FORCE = 120 units/s       // Initial jump velocity
BOUNCE_DAMPENING = 0.6         // 60% energy retained
```

### Track Parameters

```cpp
TRACK_RADIUS = 60 pixels       // Center to marble path
TRACK_WIDTH = 30 pixels        // Width of valid area
MARBLE_RADIUS = 8 pixels       // Visual marble size
```

### Architecture

```
marble_game.h          - Game engine header (structures, classes)
marble_game.cpp        - Implementation (~900 lines)
  ├─ Physics engine
  ├─ Collision detection
  ├─ Rendering system
  ├─ Particle system
  └─ Game logic
main.cpp               - Application entry and loop
User_Setup.h           - TFT_eSPI display config
```

## Hardware Requirements

### Required

- **ESP32-C3** microcontroller
- **0.71" Round LCD** (GC9D01 driver, 160x160 pixels, SPI)
- **GPIO Connections**:
  - GPIO7: TFT_MOSI
  - GPIO6: TFT_SCLK
  - GPIO5: TFT_CS
  - GPIO4: TFT_DC
  - GPIO8: TFT_RST
  - GPIO9: BOOT button (built-in)

### Optional

- USB cable for serial monitoring
- External power supply (3.3V, >300mA)

## Quick Start

### 1. Prerequisites

- ESP-IDF v5.0+ with VS Code extension
- TFT_eSPI library (shared components)

### 2. Clone TFT_eSPI Library

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../marble-game/main/User_Setup.h tft_espi/User_Setup.h
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

# Navigate to project
cd esp32-c3-demo-idf/marble-game

# Configure
idf.py set-target esp32c3

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor
```

## How to Play

### Starting the Game

1. **Power on** ESP32-C3
2. **Wait** for initialization (2 seconds)
3. **See menu** with title and instructions
4. **Press BOOT button** to start

### During Gameplay

1. **Marble rolls** automatically along circular track
2. **TAP button** to jump over obstacles
3. **HOLD button** for speed boost
4. **Avoid** black holes and barriers
5. **Collect** coins and powerups
6. **Survive** as long as possible
7. **Level up** every 500 distance

### Tips & Tricks

**Timing Jumps:**
- Jump early for holes
- Jump late for bumps (to get extra height)
- Don't jump for spinners (ride through them)

**Using Speed Boost:**
- Hold before jumps for longer distance
- Use to quickly escape spinners
- Release to slow down for precision

**Powerup Strategy:**
- Invincibility: Destroy obstacles for bonus points
- Speed boost collectible: Save for difficult sections
- Hearts: Very rare, don't miss them!

**Obstacle Patterns:**
- Bumps often appear before holes (intentional!)
- Barriers come in groups
- Spinners randomize your position

## Customization

### Adjust Difficulty

Edit `marble_game.cpp`:

```cpp
// Make game easier
#define TRACK_SPEED 20.0f          // Slower (default: 30.0f)
#define MAX_OBSTACLES 5            // Fewer obstacles (default: 8)
m_lives = 5;                       // More lives (default: 3)

// Make game harder
#define GRAVITY 15.0f              // Stronger gravity (default: 9.8f)
#define TRACK_SPEED 50.0f          // Faster
m_trackSpeed += m_level * 10.0f;  // Faster progression
```

### Change Marble Appearance

Edit `calculateMarbleShading()`:

```cpp
// Change colors
Vec3 color1(0.2f, 0.5f, 0.9f);  // Blue marble
Vec3 color2(1.0f, 1.0f, 1.0f);  // White marble

// Change pattern
float patternValue = sinf(normal.x * 10 + m_marble.rotation);  // More swirls
```

### Adjust Physics

Edit physics constants in `marble_game.h`:

```cpp
#define GRAVITY 5.0f       // Floaty feeling
#define JUMP_FORCE 200.0f  // Super jumps
#define FRICTION 0.90f     // More slippery
```

### Modify Track

Edit `marble_game.h`:

```cpp
#define TRACK_RADIUS 50.0f   // Smaller track
#define TRACK_WIDTH 40.0f    // Wider track (easier)
```

## Troubleshooting

### Display Issues

**Black screen:**
- Check GPIO connections
- Press RESET button
- Verify SPI wiring

**Flickering:**
- Lower SPI_FREQUENCY in User_Setup.h
- Check power supply stability

### Performance Issues

**Low FPS:**
- Reduce particle count in marble_game.h
- Disable serial output
- Lower display refresh rate

**Laggy controls:**
- Check deltaTime capping (should be < 0.1s)
- Ensure main loop delay is 10ms
- Verify no other tasks running

### Gameplay Issues

**Marble falls through track:**
- This is intentional (holes)
- Jump to avoid holes

**Marble won't jump:**
- Only jumps when grounded
- Wait for landing before next jump

**Score decreasing:**
- Hitting barriers causes -20 points
- This is intentional penalty

### Build Errors

**TFT_eSPI not found:**
```bash
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../marble-game/main/User_Setup.h tft_espi/
```

**Undefined reference to math functions:**
- Add `-lm` to linker flags
- Already included in CMakeLists.txt

## Advanced Features

### Camera Shake

Triggered by:
- Barrier hits (5.0 intensity)
- Hard landings (2.0 intensity)
- Level ups (3.0 intensity)

Implemented as:
- Random offset each frame
- Exponential decay (0.9x per frame)
- Applied to all rendered objects

### Invincibility Mode

When active:
- Marble flashes cyan (10Hz blink)
- Destroys obstacles on contact
- +50 points per destroyed obstacle
- Lasts 3 seconds
- Rare powerup (15% spawn rate)

### Particle Effects

Different colors for different events:
- White: Jump
- Light grey: Landing
- Orange: Hole fall
- Red: Barrier hit
- Cyan: Bump launch
- Magenta: Spinner hit
- Yellow: Coin/powerup collect
- Green: Level up

## Code Structure

### Main Loop

```
1. Calculate deltaTime
2. Update game state
   ├─ Handle input (button)
   ├─ Update physics (marble, obstacles, collectibles)
   ├─ Check collisions
   ├─ Update particles
   └─ Spawn new objects
3. Render frame
   ├─ Clear screen
   ├─ Draw track
   ├─ Draw obstacles
   ├─ Draw collectibles
   ├─ Draw marble shadow
   ├─ Draw marble
   ├─ Draw particles
   └─ Draw HUD
4. Delay 10ms
```

### State Machine

```
MENU ──button──> PLAYING ──died(lives=0)──> GAMEOVER ──button──> MENU
                    │
                    └──died(lives>0)──> PLAYING (respawn)
```

### Collision Detection

```
For each active obstacle:
  Calculate angle difference from marble
  If within 0.3 radians:
    If grounded OR barrier:
      Apply obstacle effect
      Deactivate obstacle
```

## Future Enhancements

Possible improvements:
- [ ] Multiplayer mode (two marbles)
- [ ] Track themes (ice, lava, space)
- [ ] More obstacle types (teleporters, magnets)
- [ ] Boss battles every 10 levels
- [ ] Achievement system
- [ ] High score leaderboard (NVS storage)
- [ ] Music and sound effects (PWM audio)
- [ ] Custom marble skins
- [ ] Replay system
- [ ] Slow-motion powerup
- [ ] Track editor mode

## Credits

- **Game Engine**: Custom C++ implementation
- **Graphics**: Per-pixel 3D rendering with Phong shading
- **Physics**: Custom 2D/3D hybrid engine
- **Display Library**: TFT_eSPI by Bodmer
- **Framework**: ESP-IDF by Espressif
- **Hardware**: ESP32-C3 with GC9D01 round display
- **Created with**: Claude Code

## License

This project is provided as-is for educational and entertainment purposes.

## References

- [Phong Shading](https://en.wikipedia.org/wiki/Phong_shading)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)

---

**Roll the marble, dodge the hazards, chase the high score!** 🎮✨🎯

