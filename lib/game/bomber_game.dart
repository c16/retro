import 'dart:ui';
import 'dart:math';
import 'package:flame/components.dart';
import 'package:flame/events.dart';
import 'package:flame/game.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:bomber_game/constants/game_constants.dart';
import 'package:bomber_game/game/components/background.dart';
import 'package:bomber_game/game/components/plane.dart' as game_plane;
import 'package:bomber_game/game/components/bomb.dart';
import 'package:bomber_game/game/components/building.dart';
import 'package:bomber_game/game/managers/level_manager.dart';
import 'package:bomber_game/game/managers/score_manager.dart';
import 'package:bomber_game/game/managers/sound_manager.dart';

enum GameState {
  versionSelect,
  menu,
  pronounSelection,
  landAcknowledgment,
  triggerWarning,
  playing,
  paused,
  gameOver,
  levelComplete,
  privilegeCheck,
  consentRequest,
}

class BomberGame extends FlameGame with KeyboardEvents, TapDetector {
  game_plane.Plane? plane;
  Bomb? bomb;
  List<Building>? buildings;
  late LevelManager levelManager;
  late ScoreManager scoreManager;
  late Background background;
  late SoundManager soundManager;

  GameState gameState = GameState.versionSelect;
  double _startCooldown = 0;
  GameVersion selectedVersion = GameVersion.bomber1982;

  // Bomber 2025 - Woke features (only used when selectedVersion == bomber2025)
  String playerPronouns = 'they/them';
  String planePronouns = 'it/its';
  String skyPronouns = 'they/them';
  double privilegeCheckTimer = 0;
  double microaggressionTimer = 0;
  int currentConsentBuildingIndex = -1;
  bool waitingForConsent = false;
  String lastAffirmation = '';
  double affirmationTimer = 0;
  int carbonOffsetsEarned = 0;
  int emotionalLabor = 0;

  BomberGame();

  @override
  Future<void> onLoad() async {
    await super.onLoad();

    // Initialize managers
    levelManager = LevelManager();
    scoreManager = ScoreManager();
    soundManager = SoundManager();
    await scoreManager.loadHighScore();

    // Set camera
    camera.viewfinder.visibleGameSize = Vector2(
      GameConstants.gameWidth,
      GameConstants.gameHeight,
    );
    camera.viewfinder.position = Vector2(
      GameConstants.gameWidth / 2,
      GameConstants.gameHeight / 2,
    );
    camera.viewfinder.anchor = Anchor.center;

    // Add background (will update based on version selection)
    background = Background(isBomber2025: false);
    add(background);

    // Don't initialize game objects yet - wait for version selection
  }

  void _updateBackgroundForVersion() {
    // Remove old background
    background.removeFromParent();

    // Add new background based on selected version
    background = Background(isBomber2025: selectedVersion == GameVersion.bomber2025);
    add(background);
  }

  void _initializeGame() {
    // Remove old components if any
    children.whereType<game_plane.Plane>().forEach((p) => p.removeFromParent());
    children.whereType<Bomb>().forEach((b) => b.removeFromParent());
    children.whereType<Building>().forEach((b) => b.removeFromParent());

    // Create new components
    plane = game_plane.Plane(level: levelManager.level);
    bomb = Bomb();
    buildings = levelManager.generateLevel(levelManager.level);

    // Ensure bomb is deactivated
    bomb?.deactivate();

    // Add components
    if (plane != null) add(plane!);
    if (bomb != null) add(bomb!);
    if (buildings != null) {
      for (var building in buildings!) {
        add(building);
      }
    }
  }

  void startGame() {
    gameState = GameState.playing;
    levelManager.reset();
    scoreManager.reset();

    // Reset Bomber-2025 features
    if (selectedVersion == GameVersion.bomber2025) {
      privilegeCheckTimer = 0;
      microaggressionTimer = 0;
      emotionalLabor = 0;
      carbonOffsetsEarned = 0;
      affirmationTimer = 0;
      lastAffirmation = '';
    }

    // Update background for selected version
    _updateBackgroundForVersion();

    _initializeGame();
    _startCooldown = 0.5; // 0.5 second cooldown before allowing bomb drops
  }

  void nextLevel() {
    levelManager.nextLevel();
    scoreManager.addLevelBonus(levelManager.level - 1);
    _initializeGame();
    gameState = GameState.playing;
  }

  void dropBomb() {
    if (gameState != GameState.playing) return;
    if (bomb == null || plane == null) return;
    if (bomb!.isActive) return;
    if (_startCooldown > 0) return; // Prevent bomb drop during cooldown

    // Only allow bomb drops when plane is over a building center (character-spaced)
    if (!_isPlaneOverBuildingCenter()) return;

    bomb!.drop(plane!.position);
    soundManager.playDropSound(); // Play whistling drop sound
  }

  bool _isPlaneOverBuildingCenter() {
    if (plane == null || buildings == null) return false;

    final planeCenterX = plane!.position.x + plane!.width / 2;

    // Check each building to see if plane is centered over it
    for (var building in buildings!) {
      if (building.isDestroyed) continue; // Skip destroyed buildings

      final buildingCenterX = building.position.x + building.width / 2;
      final tolerance = building.width * 0.3; // Allow 30% tolerance for alignment

      if ((planeCenterX - buildingCenterX).abs() < tolerance) {
        return true;
      }
    }

    return false;
  }

  @override
  void update(double dt) {
    super.update(dt);

    if (gameState != GameState.playing) return;

    // Countdown the start cooldown
    if (_startCooldown > 0) {
      _startCooldown -= dt;
      if (_startCooldown < 0) {
        _startCooldown = 0;
      }
    }

    // Bomber 2025 - Privilege check timer (every 30 seconds)
    if (selectedVersion == GameVersion.bomber2025) {
      privilegeCheckTimer += dt;
      if (privilegeCheckTimer >= 30) {
        privilegeCheckTimer = 0;
        gameState = GameState.privilegeCheck;
        soundManager.playPrivilegeCheckSound();
        return;
      }

      // Bomber 2025 - Microaggression meter
      microaggressionTimer += dt;
      if (microaggressionTimer >= 1) {
        microaggressionTimer = 0;
      }

      // Countdown affirmation timer
      if (affirmationTimer > 0) {
        affirmationTimer -= dt;
        if (affirmationTimer < 0) {
          affirmationTimer = 0;
          lastAffirmation = '';
        }
      }
    }

    _checkCollisions();
    _checkLevelComplete();
  }

  void _checkCollisions() {
    // Skip collision checks during cooldown
    if (_startCooldown > 0) return;
    if (bomb == null || plane == null || buildings == null) return;

    // Check bomb-building collisions
    if (bomb!.isActive) {
      for (var building in buildings!) {
        if (!building.isDestroyed &&
            building.collidesWithPoint(bomb!.centerPosition)) {
          building.reduceHeight();
          scoreManager.addBlockScore(levelManager.level);
          bomb!.deactivate();

          // Play hit sound (white noise for 1982, bloop for 2025)
          soundManager.playHitSound();

          // Bomber 2025 - Show affirmation message
          if (selectedVersion == GameVersion.bomber2025) {
            final affirmations = [
              'You are valid!',
              'We\'re all healing together!',
              'Your emotional labor is seen!',
              'Doing the work!',
              'Decolonizing in progress!',
              'That\'s praxis!',
              'Centering marginalized voices!',
              'Disrupting systems!',
              'Creating safe spaces!',
              'Intersectional solidarity!',
            ];
            final random = Random();
            lastAffirmation = affirmations[random.nextInt(affirmations.length)];
            affirmationTimer = 2.0; // Show for 2 seconds

            // Track emotional labor and carbon offsets
            emotionalLabor++;
            carbonOffsetsEarned++;

            // Play affirmation sound
            soundManager.playAffirmationSound();
          }

          break;
        }
      }
    }

    // Check plane-building collisions
    for (var building in buildings!) {
      if (!building.isDestroyed) {
        // Check if plane collides with building
        final planeBottomY = plane!.position.y + plane!.height;
        final planeLeftX = plane!.position.x;
        final planeRightX = plane!.position.x + plane!.width;

        final buildingTopY = building.topY;
        final buildingLeftX = building.position.x;
        final buildingRightX = building.position.x + building.width;

        // Check if plane is at or below building top
        if (planeBottomY >= buildingTopY) {
          // Check horizontal overlap
          if (planeRightX > buildingLeftX && planeLeftX < buildingRightX) {
            _gameOver();
            return;
          }
        }
      }
    }
  }

  void _checkLevelComplete() {
    if (buildings == null) return;

    bool allDestroyed = buildings!.every((b) => b.isDestroyed);
    if (allDestroyed) {
      gameState = GameState.levelComplete;
      Future.delayed(const Duration(seconds: 2), () {
        if (gameState == GameState.levelComplete) {
          nextLevel();
        }
      });
    }
  }

  void _gameOver() {
    gameState = GameState.gameOver;
    scoreManager.saveHighScore();
    soundManager.playGameOverSound();
  }

  void restartGame() {
    startGame();
  }

  @override
  KeyEventResult onKeyEvent(
    KeyEvent event,
    Set<LogicalKeyboardKey> keysPressed,
  ) {
    if (event is KeyDownEvent) {
      if (event.logicalKey == LogicalKeyboardKey.space) {
        if (gameState == GameState.versionSelect) {
          // Space confirms selection and goes to menu
          gameState = GameState.menu;
          return KeyEventResult.handled;
        } else if (gameState == GameState.menu) {
          startGame();
          return KeyEventResult.handled;
        } else if (gameState == GameState.playing) {
          dropBomb();
          return KeyEventResult.handled;
        } else if (gameState == GameState.gameOver) {
          restartGame();
          return KeyEventResult.handled;
        } else if (gameState == GameState.privilegeCheck) {
          // Acknowledge privilege and continue
          gameState = GameState.playing;
          return KeyEventResult.handled;
        }
        return KeyEventResult.handled;
      } else if (event.logicalKey == LogicalKeyboardKey.digit1 ||
                 event.logicalKey == LogicalKeyboardKey.numpad1) {
        if (gameState == GameState.versionSelect) {
          selectedVersion = GameVersion.bomber1982;
          soundManager.setVersion(GameVersion.bomber1982);
          gameState = GameState.menu;
          return KeyEventResult.handled;
        }
      } else if (event.logicalKey == LogicalKeyboardKey.digit2 ||
                 event.logicalKey == LogicalKeyboardKey.numpad2) {
        if (gameState == GameState.versionSelect) {
          selectedVersion = GameVersion.bomber2025;
          soundManager.setVersion(GameVersion.bomber2025);
          gameState = GameState.menu;
          return KeyEventResult.handled;
        }
      } else if (event.logicalKey == LogicalKeyboardKey.keyP) {
        if (gameState == GameState.playing) {
          gameState = GameState.paused;
        } else if (gameState == GameState.paused) {
          gameState = GameState.playing;
        }
        return KeyEventResult.handled;
      } else if (event.logicalKey == LogicalKeyboardKey.keyR) {
        if (gameState == GameState.gameOver) {
          restartGame();
        }
        return KeyEventResult.handled;
      } else if (event.logicalKey == LogicalKeyboardKey.escape) {
        if (gameState == GameState.playing || gameState == GameState.paused) {
          gameState = GameState.versionSelect;
        }
        return KeyEventResult.handled;
      }
    }
    return KeyEventResult.ignored;
  }

  @override
  void onTapDown(TapDownInfo info) {
    if (gameState == GameState.menu) {
      startGame();
    } else if (gameState == GameState.playing) {
      dropBomb();
    } else if (gameState == GameState.gameOver) {
      restartGame();
    }
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);
    _renderUI(canvas);
  }

  void _renderUI(Canvas canvas) {
    final textPaint = TextPaint(
      style: TextStyle(
        color: GameConstants.uiTextColor,
        fontSize: 20,
        fontWeight: FontWeight.bold,
      ),
    );

    final smallTextPaint = TextPaint(
      style: TextStyle(
        color: GameConstants.uiTextColor,
        fontSize: 16,
      ),
    );

    // Version selection screen
    if (gameState == GameState.versionSelect) {
      _renderCenteredText(
        canvas,
        'SELECT GAME VERSION\n\n' +
            '[1] BOMBER-1982\n' +
            '    Classic Retro Edition\n\n' +
            '[2] BOMBER-2025\n' +
            '    Healing Harmony Edition\n\n' +
            'Press 1 or 2',
        24,
      );
      return;
    }

    // Show game info during gameplay states
    if (gameState == GameState.playing || gameState == GameState.paused ||
        gameState == GameState.gameOver || gameState == GameState.levelComplete) {

      if (selectedVersion == GameVersion.bomber2025) {
        // Bomber 2025 UI
        smallTextPaint.render(
          canvas,
          '${GameConstants.impactPointsName}:',
          Vector2(10, 10),
        );
        smallTextPaint.render(
          canvas,
          '${scoreManager.score}',
          Vector2(10, 28),
        );
        smallTextPaint.render(
          canvas,
          'Peak: ${scoreManager.high}',
          Vector2(10, 50),
        );
        smallTextPaint.render(
          canvas,
          'Journey: ${levelManager.level}',
          Vector2(GameConstants.gameWidth - 120, 10),
        );
        smallTextPaint.render(
          canvas,
          'Pronouns: $playerPronouns',
          Vector2(10, GameConstants.gameHeight - 45),
        );
        smallTextPaint.render(
          canvas,
          'Emotional Labor: $emotionalLabor',
          Vector2(10, GameConstants.gameHeight - 25),
        );
        smallTextPaint.render(
          canvas,
          'Carbon Offsets: $carbonOffsetsEarned',
          Vector2(GameConstants.gameWidth - 180, GameConstants.gameHeight - 25),
        );
      } else {
        // Bomber 1982 UI
        textPaint.render(
          canvas,
          'Score: ${scoreManager.score}',
          Vector2(10, 10),
        );
        smallTextPaint.render(
          canvas,
          'High: ${scoreManager.high}',
          Vector2(10, 35),
        );
        textPaint.render(
          canvas,
          'Level: ${levelManager.level}',
          Vector2(GameConstants.gameWidth - 120, 10),
        );
      }
    }

    // Game state messages
    if (gameState == GameState.menu) {
      final title = selectedVersion == GameVersion.bomber2025
          ? '${GameConstants.gameTitle}\n${GameConstants.gameSubtitle}'
          : 'BOMBER GAME';
      final instructions = selectedVersion == GameVersion.bomber2025
          ? '\n\nPress SPACE or TAP to begin your journey\n\n(Created on stolen indigenous land)\nTrigger Warning: Contains oppressive structures'
          : '\n\nPress SPACE or TAP to start';
      _renderCenteredText(canvas, title + instructions,
          selectedVersion == GameVersion.bomber2025 ? 18 : 30);
    } else if (gameState == GameState.paused) {
      final pauseText = selectedVersion == GameVersion.bomber2025
          ? 'SELF-CARE PAUSE\n\nTake time for emotional wellness\nPress P to continue the work'
          : 'PAUSED\n\nPress P to continue';
      _renderCenteredText(canvas, pauseText, 26);
    } else if (gameState == GameState.gameOver) {
      final gameOverText = selectedVersion == GameVersion.bomber2025
          ? 'GROWTH OPPORTUNITY\n\n${GameConstants.impactPointsName}: ${scoreManager.score}\n\nFailure is just another learning moment\nThe work continues\n\nPress SPACE or TAP to resume your journey'
          : 'GAME OVER\n\nScore: ${scoreManager.score}\nPress SPACE or TAP to restart';
      _renderCenteredText(canvas, gameOverText,
          selectedVersion == GameVersion.bomber2025 ? 22 : 30);
    } else if (gameState == GameState.levelComplete) {
      final completeText = selectedVersion == GameVersion.bomber2025
          ? 'Structures Decolonized!\n\nBut remember:\nThe work is never truly done'
          : 'Level Complete!';
      _renderCenteredText(canvas, completeText,
          selectedVersion == GameVersion.bomber2025 ? 30 : 40);
    } else if (gameState == GameState.privilegeCheck && selectedVersion == GameVersion.bomber2025) {
      _renderCenteredText(
        canvas,
        'PRIVILEGE CHECK\n\nPlease take a moment to acknowledge\nyour privilege before continuing.\n\nAre you being truly mindful?\n\nPress SPACE to acknowledge',
        22,
      );
    }

    // Bomber 2025: Show microaggression warnings and affirmations during gameplay
    if (selectedVersion == GameVersion.bomber2025 && gameState == GameState.playing) {
      if (plane != null) {
        smallTextPaint.render(
          canvas,
          'Warning: Rushing through important conversations',
          Vector2(GameConstants.gameWidth / 2, 80),
          anchor: Anchor.center,
        );
      }

      // Show affirmation message
      if (affirmationTimer > 0 && lastAffirmation.isNotEmpty) {
        final affirmationPaint = TextPaint(
          style: TextStyle(
            color: Color(0xFFFFFFFF),
            fontSize: 28,
            fontWeight: FontWeight.bold,
          ),
        );
        affirmationPaint.render(
          canvas,
          lastAffirmation,
          Vector2(GameConstants.gameWidth / 2, GameConstants.gameHeight / 2 - 50),
          anchor: Anchor.center,
        );
      }
    }
  }

  void _renderCenteredText(Canvas canvas, String text, double fontSize) {
    final textPaint = TextPaint(
      style: TextStyle(
        color: GameConstants.uiTextColor,
        fontSize: fontSize,
        fontWeight: FontWeight.bold,
      ),
    );

    textPaint.render(
      canvas,
      text,
      Vector2(GameConstants.gameWidth / 2, GameConstants.gameHeight / 2),
      anchor: Anchor.center,
    );
  }
}
