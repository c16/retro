import 'dart:math';
import 'package:flame/components.dart';
import 'package:bomber_game/constants/game_constants.dart';
import 'package:bomber_game/game/components/building.dart';

class LevelManager {
  int currentLevel = 1;

  List<Building> generateLevel(
    int levelNumber, {
    double screenWidth = 800,
    double screenHeight = 600,
    bool isBomber2025 = false,
  }) {
    final buildings = <Building>[];
    final random = Random();

    // Gap between buildings is 10% of building width
    final double gapWidth = GameConstants.buildingBlockWidth * 0.1;
    final double totalBuildingWidth = GameConstants.buildingBlockWidth + gapWidth;

    // Calculate number of buildings that can fit with gaps
    int buildingCount = (screenWidth / totalBuildingWidth).floor();

    // Maximum height increases with level
    int maxHeight = min(
      GameConstants.maxBuildingHeight,
      GameConstants.minBuildingHeight + levelNumber,
    );

    double currentX = 0;

    for (int i = 0; i < buildingCount; i++) {
      // Random height for each building - ensures variety in heights
      int height = GameConstants.minBuildingHeight +
          random.nextInt(maxHeight - GameConstants.minBuildingHeight + 1);

      // Position at bottom of screen
      double yPosition = screenHeight -
          (height * GameConstants.buildingBlockHeight);

      buildings.add(
        Building(
          position: Vector2(currentX, yPosition),
          currentHeight: height,
          isBomber2025: isBomber2025,
        ),
      );

      // Move to next building position (with gap)
      currentX += totalBuildingWidth;
    }

    return buildings;
  }

  void nextLevel() {
    currentLevel++;
  }

  void reset() {
    currentLevel = 1;
  }

  int get level => currentLevel;
}
