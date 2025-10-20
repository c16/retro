import 'dart:ui';
import 'package:flame/components.dart';
import 'package:flutter/material.dart' show LinearGradient, Alignment;
import 'package:bomber_game/constants/game_constants.dart';

class Background extends PositionComponent {
  bool isBomber2025;

  Background({this.isBomber2025 = false}) {
    size = Vector2(GameConstants.gameWidth, GameConstants.gameHeight);
    position = Vector2.zero();
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    final rect = Rect.fromLTWH(0, 0, size.x, size.y);

    if (isBomber2025) {
      // Draw rainbow gradient sky (LGBTQIA2S+ Pride) for Bomber-2025
      final paint = Paint()
        ..shader = LinearGradient(
          begin: Alignment.topCenter,
          end: Alignment.bottomCenter,
          colors: [
            Color(0xFFFF0000), // Red
            Color(0xFFFF7F00), // Orange
            Color(0xFFFFFF00), // Yellow
            Color(0xFF00FF00), // Green
            Color(0xFF0000FF), // Blue
            Color(0xFF4B0082), // Indigo
            Color(0xFF9400D3), // Violet
          ],
        ).createShader(rect);
      canvas.drawRect(rect, paint);
    } else {
      // Draw solid sky color for Bomber-1982
      final paint = Paint()..color = GameConstants.skyColor1982;
      canvas.drawRect(rect, paint);
    }
  }
}
