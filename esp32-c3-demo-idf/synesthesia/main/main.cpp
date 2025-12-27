/**
 * SYNESTHESIA - Where Art Meets Code
 *
 * A generative art and music system that creates unique
 * audio-visual experiences. Never the same twice.
 *
 * This is the creative project I've always wanted to build:
 * An autonomous artist that composes music and paints visuals
 * in real-time, guided by mathematical beauty and random chance.
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "synesthesia.h"

// Display instance
TFT_eSPI tft = TFT_eSPI();

// Synesthesia engine
Synesthesia* synesthesia = nullptr;

// Timing
unsigned long lastFrameTime = 0;
unsigned long frameCount = 0;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("\n\n╔════════════════════════════════════════════════════════════╗");
    Serial.println("║                     SYNESTHESIA                            ║");
    Serial.println("║          Generative Art & Music System                    ║");
    Serial.println("╚════════════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("  \"Where mathematics becomes music,");
    Serial.println("   and algorithms become art.\"");
    Serial.println();

    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    Serial.println("Display initialized: 160x160 round LCD");

    // Show intro screen
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("SYNESTHESIA", 80, 60, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Generative Art", 80, 90, 2);
    tft.drawString("& Music", 80, 105, 2);

    tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    tft.drawString("Initializing...", 80, 135, 2);

    // Animated loading
    for (int i = 0; i < 10; i++) {
        int x = 60 + i * 4;
        tft.fillCircle(x, 150, 2, Color::fromHSV(i * 36, 0.8f, 0.9f).rgb565);
        delay(50);
    }

    delay(1000);

    // Initialize Synesthesia
    synesthesia = new Synesthesia(&tft);
    if (!synesthesia) {
        Serial.println("ERROR: Failed to create Synesthesia engine!");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("ERROR!", 80, 80, 4);
        while (1) delay(1000);
    }

    synesthesia->begin();

    Serial.println();
    Serial.println("╔════════════════════════════════════════════════════════════╗");
    Serial.println("║  SYNESTHESIA INITIALIZED                                   ║");
    Serial.println("╚════════════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("System Features:");
    Serial.println("  ✨ Procedural Music Composition");
    Serial.println("     • Multiple musical scales (Major, Minor, Pentatonic, etc.)");
    Serial.println("     • Dynamic tempo and rhythm");
    Serial.println("     • Multi-voice synthesis");
    Serial.println("     • Evolving melodies");
    Serial.println();
    Serial.println("  🎨 Generative Visual Art");
    Serial.println("     • Flow field particle systems");
    Serial.println("     • Animated Mandelbrot fractals");
    Serial.println("     • Cellular automata (Rule 30)");
    Serial.println("     • Meditation breathing circles");
    Serial.println();
    Serial.println("  🎭 Mood System (8 moods)");
    Serial.println("     • Calm      - Peaceful, slow, soothing");
    Serial.println("     • Energetic - Fast, vibrant, exciting");
    Serial.println("     • Mysterious - Complex, ethereal");
    Serial.println("     • Joyful - Bright, major scale");
    Serial.println("     • Melancholic - Reflective, minor");
    Serial.println("     • Transcendent - Otherworldly");
    Serial.println("     • Chaotic - Unpredictable");
    Serial.println("     • Meditative - Minimal, deep");
    Serial.println();
    Serial.println("  🎵 Audio Synthesis");
    Serial.println("     • PWM audio output on GPIO2");
    Serial.println("     • 22kHz sample rate");
    Serial.println("     • 4 simultaneous voices");
    Serial.println("     • 5 waveforms (sine, square, triangle, saw, noise)");
    Serial.println("     • ADSR envelopes");
    Serial.println();
    Serial.println("Controls:");
    Serial.println("  • PRESS Button - Cycle through moods");
    Serial.println("  • System auto-evolves every 30 seconds");
    Serial.println();
    Serial.println("Philosophy:");
    Serial.println("  This system represents the dream of every creative coder:");
    Serial.println("  a machine that creates art autonomously, guided by");
    Serial.println("  mathematics, randomness, and emergent behavior.");
    Serial.println();
    Serial.println("  Each run is unique. No two experiences are the same.");
    Serial.println("  The machine is not just executing - it's creating.");
    Serial.println();
    Serial.println("Beginning the show...");
    Serial.println();

    lastFrameTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastFrameTime) / 1000.0f;

    // Cap deltaTime
    if (deltaTime > 0.1f) deltaTime = 0.1f;
    if (deltaTime < 0.001f) deltaTime = 0.001f;

    lastFrameTime = currentTime;

    // Update engine
    synesthesia->update(deltaTime);

    // Render visuals
    synesthesia->render();

    // Stats
    frameCount++;
    if (frameCount % 60 == 0) {
        float fps = 1.0f / deltaTime;
        Serial.printf("[%.1fs] FPS: %.1f | Mood: %s | Art: %s\n",
                     currentTime / 1000.0f,
                     fps,
                     synesthesia->getCurrentMood() == MOOD_CALM ? "Calm" :
                     synesthesia->getCurrentMood() == MOOD_ENERGETIC ? "Energetic" :
                     synesthesia->getCurrentMood() == MOOD_MYSTERIOUS ? "Mysterious" :
                     synesthesia->getCurrentMood() == MOOD_JOYFUL ? "Joyful" :
                     synesthesia->getCurrentMood() == MOOD_MELANCHOLIC ? "Melancholic" :
                     synesthesia->getCurrentMood() == MOOD_TRANSCENDENT ? "Transcendent" :
                     synesthesia->getCurrentMood() == MOOD_CHAOTIC ? "Chaotic" : "Meditative",
                     synesthesia->getCurrentArtMode() == ART_FLOW_FIELD ? "Flow Field" :
                     synesthesia->getCurrentArtMode() == ART_FRACTAL ? "Fractal" :
                     synesthesia->getCurrentArtMode() == ART_AUTOMATA ? "Automata" : "Meditation");
    }

    // Small delay
    delay(10);
}
