import 'dart:ui';
import 'package:flame/components.dart';
import 'package:bomber_game/constants/game_constants.dart';

class Bomb extends PositionComponent {
  bool isActive = false;
  double velocity = 0;
  double screenHeight;

  Bomb({this.screenHeight = 600}) {
    size = Vector2(GameConstants.bombWidth, GameConstants.bombHeight);
    position = Vector2(-1000, -1000); // Initialize off-screen
  }

  void drop(Vector2 planePosition) {
    // Position bomb at the center-bottom of the plane
    position.x = planePosition.x + GameConstants.planeWidth / 2 - width / 2;
    position.y = planePosition.y + GameConstants.planeHeight;
    velocity = 0;
    isActive = true;
  }

  @override
  void update(double dt) {
    super.update(dt);

    if (!isActive) return;

    // Apply gravity
    velocity += GameConstants.bombGravity * dt;
    position.y += velocity * dt;

    // Check if bomb has fallen off screen
    if (position.y >= screenHeight) {
      deactivate();
    }
  }

  void deactivate() {
    isActive = false;
    velocity = 0;
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    if (!isActive) return;

    final paint = Paint()
      ..color = GameConstants.bombColor
      ..style = PaintingStyle.fill;

    // Draw bomb body (elongated oval/ellipse)
    canvas.drawOval(
      Rect.fromLTWH(width * 0.2, 0, width * 0.6, height * 0.7),
      paint,
    );

    // Draw pointed nose at bottom
    final nosePath = Path();
    nosePath.moveTo(width * 0.2, height * 0.7);
    nosePath.lineTo(width * 0.5, height);
    nosePath.lineTo(width * 0.8, height * 0.7);
    nosePath.close();
    canvas.drawPath(nosePath, paint);

    // Draw tail fins at top (4 fins in cross pattern)
    final finPaint = Paint()
      ..color = GameConstants.bombColor
      ..style = PaintingStyle.fill;

    // Top fin
    final topFinPath = Path();
    topFinPath.moveTo(width * 0.45, 0);
    topFinPath.lineTo(width * 0.55, 0);
    topFinPath.lineTo(width * 0.52, -height * 0.3);
    topFinPath.lineTo(width * 0.48, -height * 0.3);
    topFinPath.close();
    canvas.drawPath(topFinPath, finPaint);

    // Left fin
    final leftFinPath = Path();
    leftFinPath.moveTo(width * 0.2, height * 0.05);
    leftFinPath.lineTo(width * 0.3, height * 0.05);
    leftFinPath.lineTo(width * 0.1, height * 0.1);
    leftFinPath.lineTo(width * 0.1, height * 0.0);
    leftFinPath.close();
    canvas.drawPath(leftFinPath, finPaint);

    // Right fin
    final rightFinPath = Path();
    rightFinPath.moveTo(width * 0.7, height * 0.05);
    rightFinPath.lineTo(width * 0.8, height * 0.05);
    rightFinPath.lineTo(width * 0.9, height * 0.0);
    rightFinPath.lineTo(width * 0.9, height * 0.1);
    rightFinPath.close();
    canvas.drawPath(rightFinPath, finPaint);
  }

  // Get center position for collision detection
  Vector2 get centerPosition => Vector2(
        position.x + width / 2,
        position.y + height / 2,
      );
}
