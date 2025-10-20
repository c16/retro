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

    // Add background
    background = Background();
    add(background);

    // Don't initialize game objects yet - wait for user to start
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
        if (gameState == GameState.menu) {
          startGame();
          return KeyEventResult.handled;
        } else if (gameState == GameState.playing) {
          dropBomb();
          return KeyEventResult.handled;
        } else if (gameState == GameState.gameOver) {
          restartGame();
          return KeyEventResult.handled;
        }
        return KeyEventResult.handled;
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
        gameState = GameState.menu;
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
    // Render score and level info
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

    // Score
    textPaint.render(
      canvas,
      'Score: ${scoreManager.score}',
      Vector2(10, 10),
    );

    // High Score
    smallTextPaint.render(
      canvas,
      'High: ${scoreManager.high}',
      Vector2(10, 35),
    );

    // Level
    textPaint.render(
      canvas,
      'Level: ${levelManager.level}',
      Vector2(GameConstants.gameWidth - 120, 10),
    );

    // Game state messages
    if (gameState == GameState.menu) {
      _renderCenteredText(
        canvas,
        'BOMBER GAME\n\nPress SPACE or TAP to start',
        30,
      );
    } else if (gameState == GameState.paused) {
      _renderCenteredText(canvas, 'PAUSED\n\nPress P to continue', 30);
    } else if (gameState == GameState.gameOver) {
      _renderCenteredText(
        canvas,
        'GAME OVER\n\nScore: ${scoreManager.score}\nPress SPACE or TAP to restart',
        30,
      );
    } else if (gameState == GameState.levelComplete) {
      _renderCenteredText(canvas, 'Level Complete!', 40);
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
