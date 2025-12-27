/**
 * MARBLE RUNNER
 * Realistic 3D Marble Game for ESP32-C3
 *
 * Features:
 * - Realistic 3D marble with spherical shading
 * - Physics-based movement with gravity
 * - Circular track with obstacles
 * - Particle effects
 * - Single button control (jump + speed boost)
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "marble_game.h"

// Display instance
TFT_eSPI tft = TFT_eSPI();

// Game instance
MarbleGame* game = nullptr;

// Timing
unsigned long lastFrameTime = 0;
unsigned long frameCount = 0;
float fps = 0;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n\n╔════════════════════════════════════════════════════╗");
    Serial.println("║          MARBLE RUNNER - 3D Marble Game            ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println();

    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    Serial.println("Display initialized: 160x160 round LCD");

    // Show loading screen
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("MARBLE", 80, 60, 4);
    tft.drawString("RUNNER", 80, 90, 4);
    tft.drawString("Loading...", 80, 120, 2);

    // Draw loading marble
    for (int i = 0; i < 10; i++) {
        int x = 80;
        int y = 140 + i;
        tft.fillCircle(x, y, 8, tft.color565(200 - i * 20, 100, 100));
    }

    // Initialize game
    game = new MarbleGame(&tft);
    if (!game) {
        Serial.println("ERROR: Failed to create game!");
        tft.fillScreen(TFT_RED);
        tft.drawString("ERROR!", 80, 80, 4);
        while (1) delay(1000);
    }

    game->begin();

    Serial.println();
    Serial.println("╔════════════════════════════════════════════════════╗");
    Serial.println("║  GAME READY                                        ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Controls:");
    Serial.println("  • PRESS Button (GPIO9) - Jump");
    Serial.println("  • HOLD Button - Speed Boost");
    Serial.println();
    Serial.println("Game Features:");
    Serial.println("  • Realistic 3D marble with spherical shading");
    Serial.println("  • Swirled marble texture pattern");
    Serial.println("  • Specular highlights and shadows");
    Serial.println("  • Physics: gravity, friction, bouncing");
    Serial.println("  • 4 obstacle types (holes, bumps, barriers, spinners)");
    Serial.println("  • 4 collectible types (coins, speedup, invincible, life)");
    Serial.println("  • Particle effects");
    Serial.println("  • Camera shake");
    Serial.println("  • Progressive difficulty");
    Serial.println();
    Serial.println("Obstacles:");
    Serial.println("  🕳️  Black Hole - Don't fall in!");
    Serial.println("  📐 Yellow Bump - Launches you into the air");
    Serial.println("  🧱 Red Barrier - Bounce back");
    Serial.println("  🌀 Purple Spinner - Spins you around");
    Serial.println();
    Serial.println("Collectibles:");
    Serial.println("  🪙 Yellow Coin - +100 points");
    Serial.println("  ⚡ Cyan Lightning - Speed boost");
    Serial.println("  ⭐ Blue Star - Invincibility");
    Serial.println("  ❤️  Green Heart - Extra life");
    Serial.println();
    Serial.println("Starting game...");
    Serial.println();

    lastFrameTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastFrameTime) / 1000.0f;

    // Cap deltaTime to prevent physics instability
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    lastFrameTime = currentTime;

    // Update game
    game->update(deltaTime);

    // Render game
    game->render();

    // Calculate FPS
    frameCount++;
    if (frameCount % 30 == 0) {
        fps = 1.0f / deltaTime;
        Serial.printf("FPS: %.1f | Score: %d | Lives: %d | State: %d\n",
                     fps, game->getScore(), game->getLives(), game->getState());
    }

    // Small delay for stability
    delay(10);
}
