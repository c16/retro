/**
 * NEON DEFENDER
 * Wild circular shooter game for ESP32-C3 with round display
 *
 * Hardware:
 * - ESP32-C3 with 0.71" Round LCD (GC9D01)
 * - BOOT button (GPIO9) for shooting/menu selection
 * - Optional: GPIO2/GPIO3 for manual rotation control
 * - Audio output on GPIO2 (PWM) - connect to speaker/amplifier
 *
 * Features:
 * - Circular arena defense gameplay
 * - 5 enemy types with unique behaviors
 * - Boss battles every 5 waves
 * - 5 powerup types (Rapid Fire, Spread Shot, Shield, Nuke, Health)
 * - Procedural chiptune music
 * - Full sound effects
 * - Particle effects and screen shake
 * - Combo system
 * - High score persistence
 * - Neon graphics aesthetic
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <nvs_flash.h>
#include "audio.h"
#include "game.h"

// Display instance
TFT_eSPI tft = TFT_eSPI();

// Game instance
NeonDefender* game = nullptr;

// Timing
unsigned long lastUpdate = 0;
unsigned long lastAudioUpdate = 0;
const unsigned long UPDATE_INTERVAL = 33; // ~30 FPS
const unsigned long AUDIO_INTERVAL = 1000 / SAMPLE_RATE; // Audio sample rate

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("\n\n╔══════════════════════════╗");
    Serial.println("║   NEON DEFENDER v1.0     ║");
    Serial.println("║   ESP32-C3 Round Display ║");
    Serial.println("╚══════════════════════════╝\n");

    // Initialize NVS for preferences
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize display
    Serial.println("Initializing display...");
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    // Boot animation
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    for (int r = 0; r < 80; r += 3) {
        tft.drawCircle(80, 80, r, TFT_CYAN);
        delay(15);
    }

    tft.drawString("NEON", 80, 60);
    tft.drawString("DEFENDER", 80, 85);

    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Loading...", 80, 110);

    delay(1000);

    // Initialize audio system
    Serial.println("Initializing audio system...");
    if (audio.begin()) {
        Serial.println("✓ Audio ready (GPIO2 PWM output)");
        tft.drawString("Audio: OK", 80, 125);
    } else {
        Serial.println("✗ Audio init failed!");
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("Audio: FAIL", 80, 125);
    }

    delay(500);
    audio.setVolume(70);

    // Initialize game
    Serial.println("Initializing game engine...");
    game = new NeonDefender(tft);
    game->begin();
    Serial.println("✓ Game engine ready");

    tft.fillScreen(TFT_BLACK);

    Serial.println("\n╔════════════════════════════════╗");
    Serial.println("║   Game Controls:               ║");
    Serial.println("║   - BOOT button: Shoot/Select  ║");
    Serial.println("║   - GPIO2 (opt): Rotate Left   ║");
    Serial.println("║   - GPIO3 (opt): Rotate Right  ║");
    Serial.println("║                                ║");
    Serial.println("║   Audio Output: GPIO2 (PWM)    ║");
    Serial.println("║   Connect to speaker/amp       ║");
    Serial.println("╚════════════════════════════════╝\n");

    Serial.println("GAME READY! Press BOOT to start.\n");

    lastUpdate = millis();
    lastAudioUpdate = micros();
}

void loop() {
    unsigned long now = millis();
    unsigned long nowMicros = micros();

    // Update audio at high frequency for smooth playback
    if (nowMicros - lastAudioUpdate >= AUDIO_INTERVAL) {
        audio.update();
        lastAudioUpdate = nowMicros;
    }

    // Update game logic and render at 30 FPS
    if (now - lastUpdate >= UPDATE_INTERVAL) {
        game->update();
        game->render();

        lastUpdate = now;

        // Performance monitoring
        static unsigned long lastStatsTime = 0;
        static int frameCount = 0;
        frameCount++;

        if (now - lastStatsTime >= 5000) {
            float fps = frameCount / 5.0;
            Serial.printf("[PERF] FPS: %.1f | Free heap: %d bytes\n", fps, ESP.getFreeHeap());
            frameCount = 0;
            lastStatsTime = now;
        }
    }

    // Small delay to prevent watchdog timeout
    delay(1);
}
