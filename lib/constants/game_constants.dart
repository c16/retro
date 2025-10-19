import 'dart:ui';

class GameConstants {
  // Screen dimensions
  static const double gameWidth = 800;
  static const double gameHeight = 600;

  // Plane constants
  static const double planeWidth = 32;
  static const double planeHeight = 32;
  static const double planeSpeed = 150; // pixels per second
  static const double planeSpeedIncrease = 10; // per level
  static const double planeStartY = 50;
  static const double altitudeDropAmount = 30;

  // Bomb constants
  static const double bombWidth = 16;
  static const double bombHeight = 24;
  static const double bombGravity = 150; // pixels per second squared (slower fall)

  // Building constants
  static const double buildingBlockWidth = 80; // Doubled from 40
  static const double buildingBlockHeight = 20;
  static const int minBuildingHeight = 3;
  static const int maxBuildingHeight = 8;
  static const int minBuildingsPerLevel = 8;
  static const int maxBuildingsPerLevel = 20;
  static const int buildingsIncreasePerLevel = 2;

  // Scoring
  static const int pointsPerBlock = 10;
  static const int levelCompleteBonus = 100;

  // Colors
  static const Color skyColor = Color(0xFF87CEEB); // Light blue sky

  static const Color buildingColor = Color(0xFFD2B48C); // Tan/Light brown

  static const Color planeColor = Color(0xFF000000); // Black
  static const Color bombColor = Color(0xFFFF0000); // Red
  static const Color uiTextColor = Color(0xFF000000); // Black

  // Game settings
  static const double targetFps = 60;
  static const String highScoreKey = 'bomber_high_score';
}
