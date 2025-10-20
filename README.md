# Bomber Game - Classic Arcade Edition

A retro arcade-style bomber game built with Flutter and Flame engine. Choose between two distinct game versions: the classic 1982 retro edition or the satirical 2025 "Healing Harmony" edition.

## Game Versions

### 🕹️ Bomber-1982 (Classic Retro Edition)
- Classic light blue sky and retro aesthetic
- Black bomber plane with animated propeller
- Red bombs with realistic fins
- Tan buildings with windows and varied rooftops
- Traditional scoring system
- Sound effects: whistling bomb drops and white noise hits

### 🌈 Bomber-2025 (Healing Harmony Edition)
- Rainbow gradient pride sky
- Dark gray plane (optimized visibility)
- Pink heart-shaped "Equity Seeds" with sparkles
- Gray "Problematic Structures" with satirical labels
- Privilege checks every 30 seconds
- Affirmation messages and emotional labor tracking
- Carbon offset counter
- Complete satirical woke terminology

## Prerequisites

### Required Software
- **Flutter SDK** (3.9.2 or higher)
- **Linux** (tested on Ubuntu/Debian-based systems)
- **GStreamer** development libraries (for audio)

### System Dependencies

Install GStreamer libraries (required for sound):

```bash
sudo apt-get update
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```

## Building from Source

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/retro.git
   cd retro/bomber_game
   ```

2. **Install Flutter dependencies:**
   ```bash
   flutter pub get
   ```

3. **Build the Linux application:**
   ```bash
   flutter build linux --release
   ```

   The compiled binary will be located at:
   ```
   build/linux/x64/release/bundle/bomber_game
   ```

## Installation

### Option 1: Run from Build Directory
```bash
./build/linux/x64/release/bundle/bomber_game
```

### Option 2: Install System-wide

Copy the entire bundle to a system location:

```bash
sudo mkdir -p /opt/bomber_game
sudo cp -r build/linux/x64/release/bundle/* /opt/bomber_game/
sudo ln -s /opt/bomber_game/bomber_game /usr/local/bin/bomber_game
```

Then run from anywhere:
```bash
bomber_game
```

### Option 3: Create Desktop Entry

Create a `.desktop` file for easy launcher access:

```bash
cat > ~/.local/share/applications/bomber_game.desktop <<EOF
[Desktop Entry]
Name=Bomber Game
Comment=Classic arcade bomber with dual game modes
Exec=/opt/bomber_game/bomber_game
Icon=applications-games
Terminal=false
Type=Application
Categories=Game;ArcadeGame;
EOF
```

## How to Play

### Version Selection
On startup, choose your game version:
- Press **1** for Bomber-1982 (Classic)
- Press **2** for Bomber-2025 (Healing Harmony)

### Controls
- **SPACE** - Drop bomb / Start game / Confirm
- **P** - Pause/Unpause
- **ESC** - Return to version selection
- **Mouse Click** - Alternative to spacebar

### Gameplay
- Guide your plane left-to-right across the screen
- Drop bombs to destroy buildings (only at building centers)
- Clear all buildings to advance to the next level
- Avoid crashing into buildings as your altitude decreases
- Each wrap-around lowers your altitude

### Bomber-2025 Special Features
- Acknowledge privilege checks when prompted
- Receive affirmations for each successful hit
- Track your emotional labor and carbon offsets
- Experience the satirical "woke" gameplay elements

## Development

### Run in Debug Mode
```bash
flutter run -d linux
```

### Clean Build
```bash
flutter clean
flutter pub get
flutter build linux --release
```

### Dependencies
- `flame: ^1.10.0` - Game engine
- `audioplayers: ^6.0.0` - Sound effects
- `shared_preferences: ^2.2.2` - High score persistence
- `window_manager: ^0.3.7` - Window management

## Troubleshooting

### Audio Issues
If you encounter GStreamer errors:
```bash
sudo apt-get install -y gstreamer1.0-plugins-base gstreamer1.0-plugins-good
```

### Build Errors
Clear Flutter cache and rebuild:
```bash
flutter clean
rm -rf build/
flutter pub get
flutter build linux --release
```

### Performance Issues
Ensure you're running the release build, not debug:
```bash
flutter build linux --release
```

## Credits

Built with Flutter and the Flame game engine.

Generated with assistance from Claude Code.

## License

This project is provided as-is for educational and entertainment purposes.
