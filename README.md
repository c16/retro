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
- **Platform Support:**
  - Linux (tested on Ubuntu/Debian-based systems)
  - Android (API level 21+)
  - Windows, macOS (should work but untested)

### System Dependencies (Linux only)

Install GStreamer libraries (required for sound on Linux):

```bash
sudo apt-get update
sudo apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```

## Building from Source

1. **Clone the repository:**
   ```bash
   git clone https://github.com/c16/retro.git
   cd retro
   ```

2. **Install Flutter dependencies:**
   ```bash
   flutter pub get
   ```

3. **Build for your platform:**

   **Linux:**
   ```bash
   flutter build linux --release
   ```

   The compiled binary will be located at:
   ```
   build/linux/x64/release/bundle/bomber_game
   ```

   **Android:**
   ```bash
   flutter build apk --release
   ```

   The APK will be located at:
   ```
   build/app/outputs/flutter-apk/app-release.apk
   ```

   **Android App Bundle (for Google Play):**
   ```bash
   flutter build appbundle --release
   ```

## Installation

### Linux Installation

**Option 1: Run from Build Directory**
```bash
./build/linux/x64/release/bundle/bomber_game
```

**Option 2: Install System-wide**

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

**Option 3: Create Desktop Entry**

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

### Android Installation

1. **Enable installation from unknown sources** on your Android device (Settings > Security)

2. **Transfer the APK** to your device:
   ```bash
   adb install build/app/outputs/flutter-apk/app-release.apk
   ```

   Or copy `app-release.apk` to your device and open it to install.

3. **Launch** the game from your app drawer

## How to Play

### Version Selection
On startup, choose your game version:
- **Keyboard:** Press **1** for Bomber-1982 or **2** for Bomber-2025
- **Touch/Mouse:** Tap top half of screen for Bomber-1982, bottom half for Bomber-2025

### Controls

**Desktop (Keyboard):**
- **SPACE** - Drop bomb / Start game / Confirm
- **P** - Pause/Unpause
- **ESC** - Return to version selection
- **Mouse Click** - Alternative to spacebar

**Mobile (Touch):**
- **Tap screen** - Drop bomb / Start game / Confirm / Navigate menus
- The plane moves automatically - just tap to drop bombs!

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
