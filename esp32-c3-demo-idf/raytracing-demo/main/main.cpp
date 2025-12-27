/**
 * ESP32-C3 Real-Time Raytracing Demo
 *
 * Features:
 * - Real-time raytracing on ESP32-C3
 * - Rotating metal spheres with reflections
 * - Checkered floor with reflections
 * - Sky with sun and procedural clouds
 * - Optimized for 160x160 round LCD
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "raytracer.h"

// Display instance
TFT_eSPI tft = TFT_eSPI();

// Raytracer instance
Raytracer* raytracer = nullptr;

// Timing
unsigned long lastFrameTime = 0;
float deltaTime = 0;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n\n╔════════════════════════════════════════════════════╗");
    Serial.println("║     ESP32-C3 REAL-TIME RAYTRACING DEMO             ║");
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
    tft.drawString("RAYTRACER", 80, 70, 4);
    tft.drawString("Loading...", 80, 100, 2);

    // Initialize raytracer
    raytracer = new Raytracer(&tft);
    if (!raytracer) {
        Serial.println("ERROR: Failed to create raytracer!");
        tft.fillScreen(TFT_RED);
        tft.drawString("ERROR!", 80, 80, 4);
        while (1) delay(1000);
    }

    raytracer->begin();

    // Clear screen
    tft.fillScreen(TFT_BLACK);

    Serial.println();
    Serial.println("╔════════════════════════════════════════════════════╗");
    Serial.println("║  RAYTRACER READY                                   ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("Features:");
    Serial.println("  • 5 rotating metal spheres (silver, gold, copper, blue, green)");
    Serial.println("  • Reflective checkered floor");
    Serial.println("  • Sky gradient with sun");
    Serial.println("  • Procedural clouds");
    Serial.println("  • Rotating camera");
    Serial.println();
    Serial.println("Render resolution: 80x80 (scaled to 160x160)");
    Serial.println("Max ray depth: 2 bounces");
    Serial.println();
    Serial.println("Starting render loop...");
    Serial.println();

    lastFrameTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;

    // Update scene animation
    raytracer->update(deltaTime);

    // Render frame
    raytracer->render();

    // Small delay to prevent watchdog timeout
    delay(10);
}
