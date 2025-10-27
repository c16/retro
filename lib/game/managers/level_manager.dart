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

    // Limit to maximum 10 buildings
    const int maxBuildings = 10;
    const double minGapWidth = 8.0; // Minimum gap between buildings

    // Calculate spacing to fit exactly 10 buildings across screen
    final double totalWidthForBuildings = screenWidth;
    final double buildingWidth = GameConstants.buildingBlockWidth;
    final double totalGapSpace = totalWidthForBuildings - (maxBuildings * buildingWidth);
    final double gapWidth = max(minGapWidth, totalGapSpace / (maxBuildings + 1)); // Gaps before, between, and after

    int buildingCount = maxBuildings;

    // Maximum height increases with level
    int maxHeight = min(
      GameConstants.maxBuildingHeight,
      GameConstants.minBuildingHeight + levelNumber,
    );

    // Start with initial gap
    double currentX = gapWidth;

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

      // Move to next building position (building width + gap)
      currentX += buildingWidth + gapWidth;
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
