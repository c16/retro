import 'dart:ui';
import 'dart:math';
import 'package:flame/components.dart';

class Explosion extends PositionComponent {
  double lifetime = 0;
  final double maxLifetime = 0.5; // Half second explosion
  final Random random = Random();
  final List<Particle> particles = [];

  Explosion({required Vector2 position}) {
    this.position = position.clone();
    size = Vector2(60, 60);
    anchor = Anchor.center;

    // Create explosion particles
    for (int i = 0; i < 20; i++) {
      final angle = random.nextDouble() * 2 * pi;
      final speed = 50 + random.nextDouble() * 100;
      particles.add(Particle(
        dx: cos(angle) * speed,
        dy: sin(angle) * speed,
        color: _randomExplosionColor(),
        size: 2 + random.nextDouble() * 3,
      ));
    }
  }

  Color _randomExplosionColor() {
    final colors = [
      const Color(0xFFFF0000), // Red
      const Color(0xFFFF6600), // Orange
      const Color(0xFFFFFF00), // Yellow
      const Color(0xFFFFFFFF), // White
    ];
    return colors[random.nextInt(colors.length)];
  }

  @override
  void update(double dt) {
    super.update(dt);
    lifetime += dt;

    // Update all particles
    for (var particle in particles) {
      particle.x += particle.dx * dt;
      particle.y += particle.dy * dt;
      particle.dy += 200 * dt; // Gravity
    }

    // Remove explosion when lifetime exceeded
    if (lifetime >= maxLifetime) {
      removeFromParent();
    }
  }

  @override
  void render(Canvas canvas) {
    super.render(canvas);

    final progress = lifetime / maxLifetime;
    final alpha = (255 * (1.0 - progress)).toInt();

    for (var particle in particles) {
      final paint = Paint()
        ..color = particle.color.withAlpha(alpha)
        ..style = PaintingStyle.fill;

      canvas.drawCircle(
        Offset(particle.x, particle.y),
        particle.size * (1.0 + progress),
        paint,
      );
    }
  }
}

class Particle {
  double x = 0;
  double y = 0;
  double dx;
  double dy;
  Color color;
  double size;

  Particle({
    required this.dx,
    required this.dy,
    required this.color,
    required this.size,
  });
}
