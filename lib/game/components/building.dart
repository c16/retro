import 'dart:ui';
import 'dart:math';
import 'package:flame/components.dart';
import 'package:flutter/material.dart' show TextPaint, TextStyle, FontWeight;
import 'package:bomber_game/constants/game_constants.dart';

enum RoofType { flat, peaked, dome }

enum OppressionType {
  genderBinary,
  fossilFuel,
  patriarchal,
  colonizer,
}

class Building extends PositionComponent {
  int currentHeight;
  int initialHeight; // Track original height to know if building is damaged
  bool isDestroyed = false;
  late RoofType roofType;
  late OppressionType oppressionType;
  bool hasGivenConsent = false; // For consent-based healing
  bool isBomber2025; // Show labels only for Bomber-2025

  // Track jagged top edge - height variations for visual effect
  // Key is the block index (0 to blocksWide-1), value is offset in pixels
  Map<int, int> jaggedTop = {};
  late int blocksWide;

  Building({
    required Vector2 position,
    required this.currentHeight,
    this.isBomber2025 = false,
  }) : initialHeight = currentHeight,
       super(
          position: position,
          size: Vector2(
            GameConstants.buildingBlockWidth,
            currentHeight * GameConstants.buildingBlockHeight,
          ),
        ) {
    // Calculate how many blocks wide this building is
    blocksWide = (GameConstants.buildingBlockWidth / GameConstants.buildingBlockHeight).round();

    // Initialize with no jagged effect (building starts undamaged)
    for (int i = 0; i < blocksWide; i++) {
      jaggedTop[i] = 0;
    }

    // Randomly assign roof type and oppression type
    final random = Random();
    final roofChoice = random.nextInt(3);
    roofType = RoofType.values[roofChoice];

    final oppressionChoice = random.nextInt(4);
    oppressionType = OppressionType.values[oppressionChoice];
  }

  String get oppressionLabel {
    switch (oppressionType) {
      case OppressionType.genderBinary:
        return 'Gender Binary\nEnforcement';
      case OppressionType.fossilFuel:
        return 'Fossil Fuel\nHQ';
      case OppressionType.patriarchal:
        return 'Patriarchal\nInstitution';
      case OppressionType.colonizer:
        return 'Colonizer\nMonument';
    }
  }

  void reduceHeight() {
    if (currentHeight > 0) {
      final random = Random();

      // Randomly destroy 1, 2, or 3 floors
      final floorsToDestroy = 1 + random.nextInt(3); // 1, 2, or 3
      final actualFloorsDestroyed = floorsToDestroy.clamp(1, currentHeight);

      currentHeight -= actualFloorsDestroyed;
      size.y = currentHeight * GameConstants.buildingBlockHeight;
      position.y += actualFloorsDestroyed * GameConstants.buildingBlockHeight;

      // Create jagged top effect after damage with larger variations
      for (int i = 0; i < blocksWide; i++) {
        jaggedTop[i] = random.nextInt(8); // 0 to 7 pixels for more visible effect
      }

      if (currentHeight == 0) {
        isDestroyed = true;
      }
    }
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    if (isDestroyed || currentHeight == 0) {
      return;
    }

    final buildingPaint = Paint()
      ..color = GameConstants.buildingColor
      ..style = PaintingStyle.fill;

    final windowPaint = Paint()
      ..color = const Color(0xFF000000)
      ..style = PaintingStyle.fill;

    final roofPaint = Paint()
      ..color = const Color(0xFF8B4513) // Darker brown for roof
      ..style = PaintingStyle.fill;

    final blockWidth = GameConstants.buildingBlockHeight;

    // Draw each vertical column with jagged top
    for (int colIndex = 0; colIndex < blocksWide; colIndex++) {
      if (!jaggedTop.containsKey(colIndex)) continue;

      final colX = colIndex * blockWidth;
      final jaggedOffset = jaggedTop[colIndex]!;

      // Draw the column - slightly shorter at top for jagged effect
      final columnTopY = -jaggedOffset.toDouble();

      canvas.drawRect(
        Rect.fromLTWH(
          colX,
          columnTopY,
          blockWidth,
          size.y - columnTopY,
        ),
        buildingPaint,
      );

      // Draw windows in each floor
      for (int floor = 0; floor < currentHeight; floor++) {
        final windowY = size.y - (floor + 1) * GameConstants.buildingBlockHeight +
                        GameConstants.buildingBlockHeight / 2 - 3;

        // Skip top floor if it would overlap with jagged edge
        if (floor == currentHeight - 1 && jaggedOffset > 5) continue;

        canvas.drawRect(
          Rect.fromLTWH(
            colX + blockWidth / 2 - 3,
            windowY,
            6,
            6,
          ),
          windowPaint,
        );
      }
    }

    // Only draw roof on undamaged buildings
    if (currentHeight == initialHeight) {
      final roofY = 0.0;

      switch (roofType) {
        case RoofType.peaked:
          // Triangular peaked roof
          final roofPath = Path();
          roofPath.moveTo(0, roofY);
          roofPath.lineTo(GameConstants.buildingBlockWidth / 2, roofY - 10);
          roofPath.lineTo(GameConstants.buildingBlockWidth, roofY);
          roofPath.close();
          canvas.drawPath(roofPath, roofPaint);
          break;

        case RoofType.dome:
          // Dome/rounded roof
          canvas.drawArc(
            Rect.fromLTWH(
              0,
              roofY - 8,
              GameConstants.buildingBlockWidth,
              16,
            ),
            3.14159, // Pi (180 degrees)
            3.14159, // Pi (180 degrees) - draws top half
            true,
            roofPaint,
          );
          break;

        case RoofType.flat:
          // Flat roof with small lip
          canvas.drawRect(
            Rect.fromLTWH(-2, roofY - 3, GameConstants.buildingBlockWidth + 4, 3),
            roofPaint,
          );
          break;
      }
    }

    // Draw oppression label on buildings (Bomber 2025 only)
    if (isBomber2025) {
      final labelPaint = TextPaint(
        style: TextStyle(
          color: Color(0xFFFFFFFF), // White text
          fontSize: 8,
          fontWeight: FontWeight.bold,
        ),
      );

      labelPaint.render(
        canvas,
        oppressionLabel,
        Vector2(width / 2, size.y / 2),
        anchor: Anchor.center,
      );
    }
  }

  // Check collision with a point
  bool collidesWithPoint(Vector2 point) {
    if (isDestroyed) return false;

    return point.x >= position.x &&
        point.x <= position.x + width &&
        point.y >= position.y &&
        point.y <= position.y + size.y;
  }

  // Get the top Y position of the building
  double get topY => position.y;

  // Get the bottom Y position of the building
  double get bottomY => position.y + size.y;
}
