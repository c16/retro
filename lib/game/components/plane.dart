import 'dart:ui';
import 'package:flame/components.dart';
import 'package:bomber_game/constants/game_constants.dart';

class Plane extends PositionComponent {
  late double speed;
  late double currentAltitude;
  int level = 1;
  double propellerTimer = 0;
  bool propellerVisible = true;
  double screenWidth;

  Plane({required this.level, this.screenWidth = 800}) {
    size = Vector2(GameConstants.planeWidth, GameConstants.planeHeight);
    position = Vector2(0, GameConstants.planeStartY);
    speed = GameConstants.planeSpeed + (level - 1) * GameConstants.planeSpeedIncrease;
    currentAltitude = GameConstants.planeStartY;
  }

  @override
  void update(double dt) {
    super.update(dt);

    // Always move left to right
    position.x += speed * dt;

    // Animate propeller (simple toggle at 10Hz)
    propellerTimer += dt;
    if (propellerTimer >= 0.1) { // Toggle every 0.1 seconds
      propellerVisible = !propellerVisible;
      propellerTimer = 0;
    }

    // Check if reached right edge - wrap to left and drop altitude
    if (position.x >= screenWidth) {
      position.x = -width;
      dropAltitude();
    }
  }

  void dropAltitude() {
    currentAltitude += GameConstants.altitudeDropAmount;
    position.y = currentAltitude;
  }

  void reset({int? newLevel}) {
    if (newLevel != null) {
      level = newLevel;
      speed = GameConstants.planeSpeed + (level - 1) * GameConstants.planeSpeedIncrease;
    }
    position.x = 0;
    currentAltitude = GameConstants.planeStartY;
    position.y = currentAltitude;
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    final paint = Paint()
      ..color = GameConstants.planeColor
      ..style = PaintingStyle.fill;

    // Simple bomber silhouette matching the reference image
    // Main fuselage (long horizontal body)
    canvas.drawRect(
      Rect.fromLTWH(width * 0.1, height * 0.4, width * 0.7, height * 0.2),
      paint,
    );

    // Pointed nose
    final nosePath = Path();
    nosePath.moveTo(width * 0.8, height * 0.4);
    nosePath.lineTo(width * 0.95, height * 0.5);
    nosePath.lineTo(width * 0.8, height * 0.6);
    nosePath.close();
    canvas.drawPath(nosePath, paint);

    // Tail section
    canvas.drawRect(
      Rect.fromLTWH(width * 0.05, height * 0.45, width * 0.1, height * 0.1),
      paint,
    );

    // Vertical tail fin
    canvas.drawRect(
      Rect.fromLTWH(width * 0.05, height * 0.25, width * 0.08, height * 0.2),
      paint,
    );

    // Wings - single horizontal wing
    canvas.drawRect(
      Rect.fromLTWH(width * 0.25, height * 0.35, width * 0.5, height * 0.08),
      paint,
    );

    // Animated propeller at the nose (simple character toggle)
    if (propellerVisible) {
      final propellerCenterX = width * 0.95;
      final propellerCenterY = height * 0.5;

      final propellerPaint = Paint()
        ..color = GameConstants.planeColor
        ..style = PaintingStyle.fill;

      // Simple vertical line (like a | character)
      canvas.drawRect(
        Rect.fromLTWH(
          propellerCenterX - 1,
          propellerCenterY - height * 0.15,
          2,
          height * 0.3,
        ),
        propellerPaint,
      );
    }
  }

  // Check collision with a point (for building collision)
  bool collidesWithPoint(Vector2 point) {
    return point.x >= position.x &&
        point.x <= position.x + width &&
        point.y >= position.y &&
        point.y <= position.y + height;
  }
}
