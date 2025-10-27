# Retro Games Suite - Flutter Implementation Guide

## Project Overview
A collection of classic retro arcade games reimagined with Flutter. The games are developed and tested on Flutter for Linux, with deployment targets for Android and other platforms. Each game maintains authentic retro aesthetics while providing modern dual-mode experiences.

---

## Game 1: Bomber

### Project Overview
Classic Bomber arcade game inspired by the VIC-20 version. Features two distinct game modes:
- **Bomber-1982**: Classic retro edition with traditional gameplay
- **Bomber-2025**: "Healing Harmony" satirical edition with modern parody elements

**Status**: ✅ Complete and deployed
**Repository**: `/bomber_game`

### Game Specifications

#### Core Gameplay
- **Objective**: Pilot a plane that automatically flies left-to-right across the screen, dropping bombs to destroy buildings below before crashing into them
- **Win Condition**: Successfully destroy all buildings to clear the level
- **Lose Condition**: Plane crashes into a building

#### Game Mechanics
1. **Plane Movement**:
   - Moves automatically from left to right at constant horizontal speed
   - Returns to left side after reaching right edge, dropping one altitude level
   - Cannot be controlled directionally by player
   - Speed increases slightly with each level

2. **Bombing Mechanics**:
   - Player drops bombs using spacebar (desktop) or tap (mobile)
   - Only one bomb can be active at a time
   - Bombs fall straight down with gravity
   - Bombs destroy 1-3 blocks randomly on impact (adds unpredictability)

3. **Buildings**:
   - Randomly generated heights (3-8 blocks tall)
   - Varied roof types (peaked, dome, flat)
   - Character-spaced bomb dropping (only at building centers)
   - Jagged destruction effects
   - 10% gaps between buildings

4. **Scoring**:
   - 10 points per building block destroyed
   - 100 bonus points for completing a level
   - Separate high scores for each game version

#### Version-Specific Features

**Bomber-1982 (Classic)**:
- Light blue sky background
- Black plane with animated propeller
- Red bombs with fins
- Tan buildings with windows
- Traditional UI and scoring
- Sound effects: whistling drops, white noise hits

**Bomber-2025 (Healing Harmony)**:
- Rainbow gradient pride sky
- Dark gray plane (optimized visibility)
- Pink heart-shaped "Equity Seeds"
- Gray buildings with satirical oppression labels
- Privilege checks every 30 seconds
- Affirmation messages on successful hits
- Emotional Labor & Carbon Offset counters
- Woke terminology throughout
- Sound effects: descending whistles, bloops, chimes

### Technical Implementation

#### Dependencies
```yaml
dependencies:
  flame: ^1.10.0
  audioplayers: ^6.0.0
  shared_preferences: ^2.2.2
  window_manager: ^0.3.7
```

#### Key Components
- `BomberGame`: Main game class with version selection
- `Plane`: Animated plane component
- `Bomb`: Physics-based projectile with version-specific rendering
- `Building`: Destructible structures with jagged effects
- `Background`: Version-specific sky rendering
- `SoundManager`: Programmatic WAV generation for both versions
- `LevelManager`: Random level generation
- `ScoreManager`: Persistent high score tracking

#### Controls
- **1/2 Keys**: Select game version (1982 or 2025)
- **Space Bar**: Drop bomb / Start game / Confirm
- **P**: Pause/Unpause
- **ESC**: Return to version selection
- **Mouse Click**: Alternative to spacebar

---

## Game 2: Yosser

### Project Overview
*Classic platform puzzle game - Coming Soon*

**Status**: 📋 Planned
**Repository**: `/yosser_game` (to be created)

### Game Concept (Placeholder)

#### Proposed Gameplay
- Platform puzzle mechanics
- Character navigation through levels
- Obstacle avoidance and collection mechanics
- Progressive difficulty system

#### Proposed Features
- Retro pixel art aesthetic
- Classic arcade-style controls
- Level progression system
- High score tracking
- Multiple game modes

### Technical Specifications
*To be defined*

#### Planned Dependencies
```yaml
dependencies:
  flame: ^1.10.0
  # Additional dependencies TBD
```

#### Controls (Proposed)
- Arrow keys / WASD for movement
- Space bar for action
- Standard menu controls

### Development Timeline
*To be scheduled*

---

## Suite-Wide Technical Requirements

### Flutter Setup
```yaml
# Common dependencies across games
dependencies:
  flutter:
    sdk: flutter
  flame: ^1.10.0
  audioplayers: ^6.0.0
  shared_preferences: ^2.2.2
```

### Project Structure Pattern
```
retro/
├── bomber_game/          # Completed
├── yosser_game/          # Planned
├── [future_game_3]/      # Future
└── claude.md            # This file
```

### Performance Standards
1. **Frame Rate**: Target 60 FPS on all platforms
2. **Memory**: Efficient object pooling and resource management
3. **Loading**: Fast startup times (<3 seconds)
4. **Audio**: Low-latency sound effects (<50ms)

### Platform Support
- **Primary**: Linux (development and testing)
- **Secondary**: Android (deployment target)
- **Future**: Web, Windows, macOS

### Code Style Guidelines
- Follow Flutter's official style guide
- Maintain 80-character line limit where practical
- Document all public methods and classes
- Use meaningful variable names
- Implement proper error handling
- Consistent component architecture across games

### Common Features Across Games
1. **Version Selection**: Multiple game modes where applicable
2. **High Score Persistence**: Using shared_preferences
3. **Sound Management**: Programmatic audio generation
4. **Retro Aesthetics**: Authentic vintage game feel
5. **Modern Polish**: Smooth animations and responsive controls

### Build Instructions (All Games)

#### Linux Build
```bash
cd [game_directory]
flutter pub get
flutter build linux --release
```

#### Android Build
```bash
cd [game_directory]
flutter build apk --release
```

### Testing Requirements
- Unit tests for game logic
- Integration tests for user interactions
- Performance benchmarks (60 FPS minimum)
- Cross-platform compatibility testing

### Success Criteria (Per Game)
1. Runs at 60 FPS on target platforms
2. All core mechanics function correctly
3. No memory leaks or performance issues
4. Intuitive controls
5. Clean, maintainable code
6. Complete audio implementation
7. Persistent high scores

---

## Development Workflow

### Adding a New Game
1. Create game directory: `/[game_name]_game/`
2. Initialize Flutter project structure
3. Set up Flame game engine
4. Implement core gameplay loop
5. Add audio system
6. Implement UI and menus
7. Add version selection if applicable
8. Test thoroughly on Linux
9. Deploy to Android
10. Update this documentation

### Code Reusability
- Share common managers (sound, score, level) where possible
- Create shared utilities library for cross-game functionality
- Maintain consistent component patterns
- Document reusable patterns for future games

### Version Control and Development Workflow

**IMPORTANT**: Follow this workflow strictly for all development:

#### Branch Strategy
- **`master`**: Production-ready code only - always stable
- **Feature branches**: All new features and bug fixes developed here
- **Naming convention**: `feature/description` or `fix/description`

#### Development Process

1. **Create Feature Branch**
   ```bash
   cd /home/terry/projects/claude-projects/retro
   git checkout -b feature/your-feature-name
   # OR for bug fixes:
   git checkout -b fix/bug-description
   ```

2. **Make Changes**
   - Implement feature or fix on the feature branch
   - Commit frequently with descriptive messages
   - Test locally during development

3. **Pre-Merge Testing (CRITICAL)**

   **Create test worktree:**
   ```bash
   # Create a clean worktree to test the feature branch
   git worktree add /tmp/retro-test-${BRANCH_NAME} ${BRANCH_NAME}
   cd /tmp/retro-test-${BRANCH_NAME}
   ```

   **Build ALL platforms:**
   ```bash
   # Test Linux build
   flutter pub get
   flutter build linux --release
   ./build/linux/x64/release/bundle/bomber_game  # Verify it runs

   # Test Android build
   flutter build apk --release
   # Verify APK location: build/app/outputs/flutter-apk/app-release.apk

   # Optional: Test other platforms
   # flutter build web --release
   # flutter build windows --release
   ```

   **Cleanup worktree:**
   ```bash
   cd /home/terry/projects/claude-projects/retro
   git worktree remove /tmp/retro-test-${BRANCH_NAME}
   ```

4. **Merge to Master**

   **Only after successful testing:**
   ```bash
   git checkout master
   git merge feature/your-feature-name
   # OR for clean history:
   git merge --squash feature/your-feature-name
   git commit
   ```

5. **Push to GitHub**

   **NEVER push automatically. Always ask user first:**
   - ❌ DO NOT: `git push origin master` automatically
   - ✅ DO: Ask "Ready to push to GitHub?" and wait for confirmation
   - Only push after explicit user approval

6. **Cleanup**
   ```bash
   # Delete feature branch after successful merge
   git branch -d feature/your-feature-name
   ```

#### Testing Requirements Before Merge

**Mandatory checks:**
- [ ] Linux release build succeeds
- [ ] Linux binary runs without crashes
- [ ] Android APK build succeeds
- [ ] No compilation errors or warnings
- [ ] Version selection works
- [ ] Core gameplay functions correctly
- [ ] No runtime exceptions in console
- [ ] Sound effects play correctly (if applicable)
- [ ] High scores persist (if applicable)
- [ ] UI scales correctly on different screen sizes

**Testing checklist template:**
```bash
# 1. Build Linux
flutter clean
flutter pub get
flutter build linux --release
./build/linux/x64/release/bundle/bomber_game

# 2. Build Android
flutter build apk --release
ls -lh build/app/outputs/flutter-apk/app-release.apk

# 3. Manual testing
# - Start game
# - Select version 1 and 2
# - Play one complete level
# - Check for crashes/errors
# - Verify sound works
# - Check responsive scaling
```

#### Example Workflow

```bash
# Starting new feature
git checkout master
git pull origin master
git checkout -b feature/add-powerups

# ... make changes ...
git add .
git commit -m "Add powerup system with 3 types"

# Test in worktree
git worktree add /tmp/retro-test feature/add-powerups
cd /tmp/retro-test
flutter build linux --release && flutter build apk --release
./build/linux/x64/release/bundle/bomber_game  # Manual test
cd /home/terry/projects/claude-projects/retro
git worktree remove /tmp/retro-test

# Merge after successful testing
git checkout master
git merge feature/add-powerups

# ASK USER: "Ready to push changes to GitHub?"
# WAIT for confirmation
# After approval:
git push origin master

# Cleanup
git branch -d feature/add-powerups
```

#### Commit Message Standards
```
Brief description (50 chars or less)

Detailed explanation of changes if needed.
Multiple paragraphs are fine.

- Bullet points for multiple changes
- Another change
- Yet another change

Testing performed:
- Linux build: ✓
- Android build: ✓
- Manual gameplay: ✓

🤖 Generated with [Claude Code](https://claude.com/claude-code)

Co-Authored-By: Claude <noreply@anthropic.com>
```

#### Emergency Hotfixes

For critical bugs in production:
```bash
git checkout master
git checkout -b hotfix/critical-crash
# ... fix the bug ...
# Test in worktree (same process)
git checkout master
git merge hotfix/critical-crash
# Ask before pushing
git branch -d hotfix/critical-crash
```

---

## Future Games (Ideas)
- Classic arcade shooters
- Puzzle games
- Platform games
- Racing games
- All maintaining retro aesthetic with modern features

---

## Notes
- Each game should be playable standalone
- Maintain authentic retro feel while adding modern quality-of-life features
- Consider dual-mode implementations (classic vs. modern) where satirically appropriate
- Prioritize performance and smooth gameplay
- Keep file sizes reasonable for mobile deployment
