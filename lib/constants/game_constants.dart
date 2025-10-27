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

  // Bomb/Equity Seed constants
  static const double bombWidth = 16;
  static const double bombHeight = 24;
  static const double bombGravity1982 = 150; // Bomber-1982
  static const double bombGravity2025 = 80; // Bomber-2025 - gentle healing energy

  // Building constants
  static const double buildingBlockWidth = 50; // Reduced for better landscape fit
  static const double buildingBlockHeight = 20;
  static const int minBuildingHeight = 3;
  static const int maxBuildingHeight = 8;
  static const int minBuildingsPerLevel = 8;
  static const int maxBuildingsPerLevel = 20;
  static const int buildingsIncreasePerLevel = 2;

  // Scoring
  static const int pointsPerBlock = 10;
  static const int levelCompleteBonus = 100;

  // Colors - Bomber 1982
  static const Color skyColor1982 = Color(0xFF87CEEB); // Light blue sky
  static const Color buildingColor1982 = Color(0xFFD2B48C); // Tan/Light brown
  static const Color planeColor1982 = Color(0xFF000000); // Black
  static const Color bombColor1982 = Color(0xFFFF0000); // Red

  // Colors - Bomber 2025 (Rainbow Pride)
  static const Color buildingColor2025 = Color(0xFF808080); // Gray (Oppressive structures)
  static const Color planeColor2025 = Color(0xFF404040); // Dark gray (visible against rainbow)
  static const Color equitySeedColor2025 = Color(0xFFFFB6C1); // Pink heart

  // Backward compatibility - default to 1982 colors
  static const Color buildingColor = buildingColor1982;
  static const Color planeColor = planeColor1982;
  static const Color bombColor = bombColor1982;
  static const double bombGravity = bombGravity1982;

  static const Color uiTextColor = Color(0xFF000000); // Black

  // Game settings
  static const double targetFps = 60;
  static const String highScoreKey = 'bomber_high_score';

  // Bomber 2025 - Woke terminology
  static const String gameTitle = 'Healing Harmony 2025';
  static const String gameSubtitle = 'A Journey of Collective Restoration';

  // Renamed elements
  static const String vehicleName = 'Carbon-Neutral Renewable Energy Delivery Vehicle';
  static const String projectileName = 'Equity Seeds';
  static const String structureName = 'Problematic Structures of Historical Oppression';
  static const String impactPointsName = 'Social Justice Impact Points';
  static const String growthOpportunitiesName = 'Opportunities for Growth and Learning';
}
