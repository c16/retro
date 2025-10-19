import 'dart:ui';
import 'package:flame/components.dart';
import 'package:bomber_game/constants/game_constants.dart';

class Background extends PositionComponent {
  Background() {
    size = Vector2(GameConstants.gameWidth, GameConstants.gameHeight);
    position = Vector2.zero();
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    // Draw solid sky color
    final rect = Rect.fromLTWH(0, 0, size.x, size.y);
    final paint = Paint()..color = GameConstants.skyColor;

    canvas.drawRect(rect, paint);
  }
}
