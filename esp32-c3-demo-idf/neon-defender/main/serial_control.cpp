/**
 * NEON DEFENDER - Serial Control Implementation
 */

#include "serial_control.h"
#include "audio.h"

// Global instance
SerialController serialController;

SerialController::SerialController()
    : m_helpShown(false)
    , m_lastCommandTime(0)
{
    m_controls.shootPressed = false;
    m_controls.leftPressed = false;
    m_controls.rightPressed = false;
    m_controls.startPressed = false;
    m_controls.pausePressed = false;
    m_controls.restartPressed = false;
    m_controls.spawnPowerup = false;
    m_controls.skipWave = false;
    m_controls.addHealth = false;
    m_controls.killAllEnemies = false;
    m_controls.enabled = true;
}

void SerialController::begin() {
    Serial.println("\n╔════════════════════════════════════════════════════╗");
    Serial.println("║        SERIAL CONTROL SYSTEM ENABLED               ║");
    Serial.println("╚════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("You can now control the game via serial commands!");
    Serial.println("Press 'H' for help or '?' to show controls.");
    Serial.println();
}

void SerialController::setEnabled(bool enabled) {
    m_controls.enabled = enabled;
    if (enabled) {
        Serial.println("\n✓ Serial controls enabled");
    } else {
        Serial.println("\n✗ Serial controls disabled");
    }
}

void SerialController::showHelp() {
    Serial.println("\n" + String("=").substring(0, 1));
    for (int i = 0; i < 60; i++) Serial.print("=");
    Serial.println();
    Serial.println("  NEON DEFENDER - SERIAL CONTROLS");
    for (int i = 0; i < 60; i++) Serial.print("=");
    Serial.println("\n");

    Serial.println("GAME CONTROLS:");
    Serial.println("  [SPACE] or [F] - Fire/Shoot");
    Serial.println("  [A] or [←]     - Rotate Left");
    Serial.println("  [D] or [→]     - Rotate Right");
    Serial.println("  [ENTER] or [S] - Start Game / Confirm");
    Serial.println("  [P]            - Pause/Unpause");
    Serial.println("  [R]            - Restart Game");
    Serial.println();

    Serial.println("MENU & CONFIG:");
    Serial.println("  [M]            - Open Configuration Menu");
    Serial.println("  [H] or [?]     - Show this help");
    Serial.println("  [E]            - Toggle Serial Controls On/Off");
    Serial.println();

    Serial.println("DEBUG COMMANDS (when debug mode enabled):");
    Serial.println("  [1]            - Spawn Random Powerup");
    Serial.println("  [2]            - Add 50 Health");
    Serial.println("  [3]            - Skip Current Wave");
    Serial.println("  [4]            - Kill All Enemies");
    Serial.println();

    Serial.println("TIPS:");
    Serial.println("  • Commands are case-insensitive");
    Serial.println("  • Hold A/D for continuous rotation");
    Serial.println("  • Press Space repeatedly to shoot rapidly");
    Serial.println("  • Enable debug mode in Config Menu for cheat commands");
    Serial.println();

    for (int i = 0; i < 60; i++) Serial.print("=");
    Serial.println("\n");

    m_helpShown = true;
}

void SerialController::update() {
    if (!m_controls.enabled) return;

    // Reset one-shot commands
    m_controls.shootPressed = false;
    m_controls.startPressed = false;
    m_controls.pausePressed = false;
    m_controls.restartPressed = false;
    m_controls.spawnPowerup = false;
    m_controls.skipWave = false;
    m_controls.addHealth = false;
    m_controls.killAllEnemies = false;

    // Process all available input
    while (Serial.available() > 0) {
        char cmd = Serial.read();

        // Skip config menu trigger and newlines
        if (cmd == 'm' || cmd == 'M' || cmd == '\n' || cmd == '\r') {
            continue;
        }

        processCommand(cmd);
        m_lastCommandTime = millis();
    }

    // Show help on first run if not shown yet
    static bool firstRun = true;
    if (firstRun && !m_helpShown && millis() > 3000) {
        Serial.println("\n💡 Tip: Press 'H' for serial control help");
        firstRun = false;
    }
}

void SerialController::processCommand(char cmd) {
    // Convert to lowercase for easier handling
    char lowerCmd = tolower(cmd);

    switch (lowerCmd) {
        // Help
        case 'h':
        case '?':
            showHelp();
            break;

        // Toggle serial controls
        case 'e':
            setEnabled(!m_controls.enabled);
            break;

        // Game controls
        case ' ':  // Space - shoot
        case 'f':  // F - fire
            m_controls.shootPressed = true;
            Serial.print(".");  // Visual feedback
            if (millis() % 10 == 0) audio.playSFX(SFX_SHOOT);
            break;

        case 'a':  // A - left
            m_controls.leftPressed = true;
            break;

        case 'd':  // D - right
            m_controls.rightPressed = true;
            break;

        case 's':  // S - start/select
        case '\n': // Enter
        case '\r':
            m_controls.startPressed = true;
            Serial.println("[START]");
            break;

        case 'p':  // P - pause
            m_controls.pausePressed = true;
            Serial.println("[PAUSE]");
            break;

        case 'r':  // R - restart
            m_controls.restartPressed = true;
            Serial.println("[RESTART]");
            break;

        // Debug commands (1-4)
        case '1':
        case '2':
        case '3':
        case '4':
            handleDebugCommand(lowerCmd);
            break;

        default:
            // Ignore unknown commands silently
            break;
    }
}

void SerialController::handleDebugCommand(char cmd) {
    // Check if debug mode is enabled via config
    extern class ConfigManager configManager;
    if (!configManager.getConfig().debugMode) {
        Serial.println("\n⚠️  Debug commands disabled. Enable in Config Menu (M → 5 → 3)");
        return;
    }

    switch (cmd) {
        case '1':
            m_controls.spawnPowerup = true;
            Serial.println("\n🎁 Spawning powerup...");
            audio.playSFX(SFX_POWERUP);
            break;

        case '2':
            m_controls.addHealth = true;
            Serial.println("\n❤️  +50 Health");
            audio.playSFX(SFX_POWERUP);
            break;

        case '3':
            m_controls.skipWave = true;
            Serial.println("\n⏭️  Skipping wave...");
            audio.playSFX(SFX_WAVE_COMPLETE);
            break;

        case '4':
            m_controls.killAllEnemies = true;
            Serial.println("\n💥 Killing all enemies!");
            audio.playSFX(SFX_EXPLOSION);
            break;
    }
}
