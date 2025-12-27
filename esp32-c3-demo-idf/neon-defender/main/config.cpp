/**
 * NEON DEFENDER - Configuration System Implementation
 */

#include "config.h"
#include "audio.h"

// Global instance
ConfigManager configManager;

ConfigManager::ConfigManager()
    : m_currentMenu(MENU_MAIN)
    , m_inMenu(false)
    , m_lastInputTime(0)
{
    resetToDefaults();
}

void ConfigManager::resetToDefaults() {
    // Audio defaults
    m_config.volume = 70;
    m_config.musicTempo = 140;
    m_config.musicEnabled = true;
    m_config.sfxEnabled = true;

    // Gameplay defaults
    m_config.difficulty = 1; // Normal
    m_config.startingHealth = 100;
    m_config.powerupDropRate = 15;
    m_config.enemySpeedMultiplier = 1.0;
    m_config.enemiesPerWave = 5;

    // Display defaults
    m_config.brightness = 100;
    m_config.showFPS = false;
    m_config.screenShakeEnabled = true;
    m_config.particlesEnabled = true;

    // Control defaults
    m_config.autoAimEnabled = true;
    m_config.rotationSpeed = 5;

    // Advanced defaults
    m_config.maxParticles = 50;
    m_config.maxEnemies = 20;
    m_config.debugMode = false;
}

bool ConfigManager::begin() {
    m_prefs.begin("neon-config", false);
    return load();
}

bool ConfigManager::save() {
    m_prefs.putUChar("volume", m_config.volume);
    m_prefs.putUShort("tempo", m_config.musicTempo);
    m_prefs.putBool("music", m_config.musicEnabled);
    m_prefs.putBool("sfx", m_config.sfxEnabled);

    m_prefs.putUChar("difficulty", m_config.difficulty);
    m_prefs.putUChar("health", m_config.startingHealth);
    m_prefs.putUChar("droprate", m_config.powerupDropRate);
    m_prefs.putFloat("espeed", m_config.enemySpeedMultiplier);
    m_prefs.putUChar("ewave", m_config.enemiesPerWave);

    m_prefs.putUChar("bright", m_config.brightness);
    m_prefs.putBool("fps", m_config.showFPS);
    m_prefs.putBool("shake", m_config.screenShakeEnabled);
    m_prefs.putBool("particles", m_config.particlesEnabled);

    m_prefs.putBool("autoaim", m_config.autoAimEnabled);
    m_prefs.putUChar("rotspeed", m_config.rotationSpeed);

    m_prefs.putUChar("maxpart", m_config.maxParticles);
    m_prefs.putUChar("maxenem", m_config.maxEnemies);
    m_prefs.putBool("debug", m_config.debugMode);

    Serial.println("\n✓ Settings saved to flash!");
    return true;
}

bool ConfigManager::load() {
    if (!m_prefs.isKey("volume")) {
        Serial.println("No saved config found, using defaults");
        return false;
    }

    m_config.volume = m_prefs.getUChar("volume", 70);
    m_config.musicTempo = m_prefs.getUShort("tempo", 140);
    m_config.musicEnabled = m_prefs.getBool("music", true);
    m_config.sfxEnabled = m_prefs.getBool("sfx", true);

    m_config.difficulty = m_prefs.getUChar("difficulty", 1);
    m_config.startingHealth = m_prefs.getUChar("health", 100);
    m_config.powerupDropRate = m_prefs.getUChar("droprate", 15);
    m_config.enemySpeedMultiplier = m_prefs.getFloat("espeed", 1.0);
    m_config.enemiesPerWave = m_prefs.getUChar("ewave", 5);

    m_config.brightness = m_prefs.getUChar("bright", 100);
    m_config.showFPS = m_prefs.getBool("fps", false);
    m_config.screenShakeEnabled = m_prefs.getBool("shake", true);
    m_config.particlesEnabled = m_prefs.getBool("particles", true);

    m_config.autoAimEnabled = m_prefs.getBool("autoaim", true);
    m_config.rotationSpeed = m_prefs.getUChar("rotspeed", 5);

    m_config.maxParticles = m_prefs.getUChar("maxpart", 50);
    m_config.maxEnemies = m_prefs.getUChar("maxenem", 20);
    m_config.debugMode = m_prefs.getBool("debug", false);

    Serial.println("✓ Settings loaded from flash");
    return true;
}

bool ConfigManager::checkForMenuRequest() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == 'm' || c == 'M') {
            m_inMenu = true;
            m_currentMenu = MENU_MAIN;
            showMenu();
            return true;
        }
    }
    return false;
}

void ConfigManager::showMenu() {
    switch (m_currentMenu) {
        case MENU_MAIN: showMainMenu(); break;
        case MENU_AUDIO: showAudioMenu(); break;
        case MENU_GAMEPLAY: showGameplayMenu(); break;
        case MENU_DISPLAY: showDisplayMenu(); break;
        case MENU_CONTROL: showControlMenu(); break;
        case MENU_ADVANCED: showAdvancedMenu(); break;
    }
}

void ConfigManager::printHeader(const char* title) {
    Serial.println("\n" + String("=").substring(0, 1));
    for (int i = 0; i < 60; i++) Serial.print("=");
    Serial.println();
    Serial.printf("  %s\n", title);
    for (int i = 0; i < 60; i++) Serial.print("=");
    Serial.println();
}

void ConfigManager::printSeparator() {
    for (int i = 0; i < 60; i++) Serial.print("-");
    Serial.println();
}

void ConfigManager::printOption(const char* key, const char* description, const char* value) {
    Serial.printf("  [%s] %-30s : %s\n", key, description, value);
}

void ConfigManager::printOption(const char* key, const char* description, int value) {
    Serial.printf("  [%s] %-30s : %d\n", key, description, value);
}

void ConfigManager::printOption(const char* key, const char* description, float value) {
    Serial.printf("  [%s] %-30s : %.2f\n", key, description, value);
}

void ConfigManager::printOption(const char* key, const char* description, bool value) {
    Serial.printf("  [%s] %-30s : %s\n", key, description, value ? "ON" : "OFF");
}

void ConfigManager::showMainMenu() {
    printHeader("NEON DEFENDER - CONFIGURATION MENU");
    Serial.println();
    printOption("1", "Audio Settings", ">");
    printOption("2", "Gameplay Settings", ">");
    printOption("3", "Display Settings", ">");
    printOption("4", "Control Settings", ">");
    printOption("5", "Advanced Settings", ">");
    Serial.println();
    printSeparator();
    printOption("V", "View All Settings", "");
    printOption("S", "Save Settings", "");
    printOption("R", "Reset to Defaults", "");
    printOption("Q", "Quit Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showAudioMenu() {
    printHeader("AUDIO SETTINGS");
    Serial.println();
    printOption("1", "Volume", m_config.volume);
    printOption("2", "Music Tempo (BPM)", m_config.musicTempo);
    printOption("3", "Music Enabled", m_config.musicEnabled);
    printOption("4", "Sound Effects Enabled", m_config.sfxEnabled);
    Serial.println();
    printSeparator();
    printOption("B", "Back to Main Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showGameplayMenu() {
    printHeader("GAMEPLAY SETTINGS");
    Serial.println();

    const char* diffNames[] = {"Easy", "Normal", "Hard", "Insane"};
    Serial.printf("  [1] %-30s : %s (%d)\n", "Difficulty", diffNames[m_config.difficulty], m_config.difficulty);
    printOption("2", "Starting Health", m_config.startingHealth);
    printOption("3", "Powerup Drop Rate (%)", m_config.powerupDropRate);
    printOption("4", "Enemy Speed Multiplier", m_config.enemySpeedMultiplier);
    printOption("5", "Base Enemies Per Wave", m_config.enemiesPerWave);
    Serial.println();
    printSeparator();
    printOption("B", "Back to Main Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showDisplayMenu() {
    printHeader("DISPLAY SETTINGS");
    Serial.println();
    printOption("1", "Brightness", m_config.brightness);
    printOption("2", "Show FPS Counter", m_config.showFPS);
    printOption("3", "Screen Shake Effects", m_config.screenShakeEnabled);
    printOption("4", "Particle Effects", m_config.particlesEnabled);
    Serial.println();
    printSeparator();
    printOption("B", "Back to Main Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showControlMenu() {
    printHeader("CONTROL SETTINGS");
    Serial.println();
    printOption("1", "Auto-Aim Enabled", m_config.autoAimEnabled);
    printOption("2", "Rotation Speed", m_config.rotationSpeed);
    Serial.println();
    printSeparator();
    printOption("B", "Back to Main Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showAdvancedMenu() {
    printHeader("ADVANCED SETTINGS");
    Serial.println();
    printOption("1", "Max Particles", m_config.maxParticles);
    printOption("2", "Max Enemies", m_config.maxEnemies);
    printOption("3", "Debug Mode", m_config.debugMode);
    Serial.println();
    printSeparator();
    printOption("B", "Back to Main Menu", "");
    printSeparator();
    Serial.print("\nChoice: ");
}

void ConfigManager::showCurrentSettings() {
    printHeader("CURRENT SETTINGS");

    Serial.println("\n╔═══ AUDIO ═══");
    Serial.printf("║ Volume: %d\n", m_config.volume);
    Serial.printf("║ Music Tempo: %d BPM\n", m_config.musicTempo);
    Serial.printf("║ Music: %s\n", m_config.musicEnabled ? "ON" : "OFF");
    Serial.printf("║ SFX: %s\n", m_config.sfxEnabled ? "ON" : "OFF");

    Serial.println("\n╔═══ GAMEPLAY ═══");
    const char* diffNames[] = {"Easy", "Normal", "Hard", "Insane"};
    Serial.printf("║ Difficulty: %s\n", diffNames[m_config.difficulty]);
    Serial.printf("║ Starting Health: %d\n", m_config.startingHealth);
    Serial.printf("║ Powerup Drop Rate: %d%%\n", m_config.powerupDropRate);
    Serial.printf("║ Enemy Speed: %.2fx\n", m_config.enemySpeedMultiplier);
    Serial.printf("║ Enemies/Wave: %d\n", m_config.enemiesPerWave);

    Serial.println("\n╔═══ DISPLAY ═══");
    Serial.printf("║ Brightness: %d\n", m_config.brightness);
    Serial.printf("║ Show FPS: %s\n", m_config.showFPS ? "ON" : "OFF");
    Serial.printf("║ Screen Shake: %s\n", m_config.screenShakeEnabled ? "ON" : "OFF");
    Serial.printf("║ Particles: %s\n", m_config.particlesEnabled ? "ON" : "OFF");

    Serial.println("\n╔═══ CONTROLS ═══");
    Serial.printf("║ Auto-Aim: %s\n", m_config.autoAimEnabled ? "ON" : "OFF");
    Serial.printf("║ Rotation Speed: %d\n", m_config.rotationSpeed);

    Serial.println("\n╔═══ ADVANCED ═══");
    Serial.printf("║ Max Particles: %d\n", m_config.maxParticles);
    Serial.printf("║ Max Enemies: %d\n", m_config.maxEnemies);
    Serial.printf("║ Debug Mode: %s\n", m_config.debugMode ? "ON" : "OFF");
    Serial.println();
}

void ConfigManager::processInput() {
    if (!m_inMenu) return;

    if (Serial.available() > 0) {
        char choice = Serial.read();

        // Clear any extra input
        while (Serial.available() > 0) Serial.read();

        Serial.println(choice); // Echo choice

        switch (m_currentMenu) {
            case MENU_MAIN: handleMainMenu(choice); break;
            case MENU_AUDIO: handleAudioMenu(choice); break;
            case MENU_GAMEPLAY: handleGameplayMenu(choice); break;
            case MENU_DISPLAY: handleDisplayMenu(choice); break;
            case MENU_CONTROL: handleControlMenu(choice); break;
            case MENU_ADVANCED: handleAdvancedMenu(choice); break;
        }

        if (m_inMenu) {
            showMenu();
        }
    }
}

void ConfigManager::handleMainMenu(char choice) {
    switch (choice) {
        case '1':
            m_currentMenu = MENU_AUDIO;
            break;
        case '2':
            m_currentMenu = MENU_GAMEPLAY;
            break;
        case '3':
            m_currentMenu = MENU_DISPLAY;
            break;
        case '4':
            m_currentMenu = MENU_CONTROL;
            break;
        case '5':
            m_currentMenu = MENU_ADVANCED;
            break;
        case 'V':
        case 'v':
            showCurrentSettings();
            waitForEnter();
            break;
        case 'S':
        case 's':
            save();
            applyToGame();
            waitForEnter();
            break;
        case 'R':
        case 'r':
            Serial.println("\nReset to defaults? (y/n): ");
            if (readBool("Confirm")) {
                resetToDefaults();
                Serial.println("✓ Reset to defaults");
            }
            waitForEnter();
            break;
        case 'Q':
        case 'q':
            m_inMenu = false;
            Serial.println("\nExiting menu...\n");
            break;
    }
}

void ConfigManager::handleAudioMenu(char choice) {
    switch (choice) {
        case '1':
            m_config.volume = readUint8("Volume (0-100)", 0, 100);
            audio.setVolume(m_config.volume);
            break;
        case '2':
            m_config.musicTempo = readUint16("Music Tempo BPM (80-200)", 80, 200);
            audio.setMusicTempo(m_config.musicTempo);
            break;
        case '3':
            m_config.musicEnabled = !m_config.musicEnabled;
            Serial.printf("Music: %s\n", m_config.musicEnabled ? "ON" : "OFF");
            if (m_config.musicEnabled) audio.startMusic();
            else audio.stopMusic();
            waitForEnter();
            break;
        case '4':
            m_config.sfxEnabled = !m_config.sfxEnabled;
            Serial.printf("SFX: %s\n", m_config.sfxEnabled ? "ON" : "OFF");
            waitForEnter();
            break;
        case 'B':
        case 'b':
            m_currentMenu = MENU_MAIN;
            break;
    }
}

void ConfigManager::handleGameplayMenu(char choice) {
    switch (choice) {
        case '1':
            m_config.difficulty = readUint8("Difficulty (0=Easy, 1=Normal, 2=Hard, 3=Insane)", 0, 3);
            break;
        case '2':
            m_config.startingHealth = readUint8("Starting Health (50-200)", 50, 200);
            break;
        case '3':
            m_config.powerupDropRate = readUint8("Powerup Drop Rate % (0-100)", 0, 100);
            break;
        case '4':
            m_config.enemySpeedMultiplier = readFloat("Enemy Speed Multiplier (0.5-2.0)", 0.5, 2.0);
            break;
        case '5':
            m_config.enemiesPerWave = readUint8("Base Enemies Per Wave (3-10)", 3, 10);
            break;
        case 'B':
        case 'b':
            m_currentMenu = MENU_MAIN;
            break;
    }
}

void ConfigManager::handleDisplayMenu(char choice) {
    switch (choice) {
        case '1':
            m_config.brightness = readUint8("Brightness (0-100)", 0, 100);
            break;
        case '2':
            m_config.showFPS = !m_config.showFPS;
            Serial.printf("Show FPS: %s\n", m_config.showFPS ? "ON" : "OFF");
            waitForEnter();
            break;
        case '3':
            m_config.screenShakeEnabled = !m_config.screenShakeEnabled;
            Serial.printf("Screen Shake: %s\n", m_config.screenShakeEnabled ? "ON" : "OFF");
            waitForEnter();
            break;
        case '4':
            m_config.particlesEnabled = !m_config.particlesEnabled;
            Serial.printf("Particles: %s\n", m_config.particlesEnabled ? "ON" : "OFF");
            waitForEnter();
            break;
        case 'B':
        case 'b':
            m_currentMenu = MENU_MAIN;
            break;
    }
}

void ConfigManager::handleControlMenu(char choice) {
    switch (choice) {
        case '1':
            m_config.autoAimEnabled = !m_config.autoAimEnabled;
            Serial.printf("Auto-Aim: %s\n", m_config.autoAimEnabled ? "ON" : "OFF");
            waitForEnter();
            break;
        case '2':
            m_config.rotationSpeed = readUint8("Rotation Speed (1-10)", 1, 10);
            break;
        case 'B':
        case 'b':
            m_currentMenu = MENU_MAIN;
            break;
    }
}

void ConfigManager::handleAdvancedMenu(char choice) {
    switch (choice) {
        case '1':
            m_config.maxParticles = readUint8("Max Particles (10-100)", 10, 100);
            break;
        case '2':
            m_config.maxEnemies = readUint8("Max Enemies (5-30)", 5, 30);
            break;
        case '3':
            m_config.debugMode = !m_config.debugMode;
            Serial.printf("Debug Mode: %s\n", m_config.debugMode ? "ON" : "OFF");
            waitForEnter();
            break;
        case 'B':
        case 'b':
            m_currentMenu = MENU_MAIN;
            break;
    }
}

uint8_t ConfigManager::readUint8(const char* prompt, uint8_t min, uint8_t max) {
    Serial.printf("\n%s: ", prompt);
    while (Serial.available() > 0) Serial.read(); // Clear buffer

    while (true) {
        if (Serial.available() > 0) {
            int value = Serial.parseInt();
            while (Serial.available() > 0) Serial.read(); // Clear remaining

            if (value >= min && value <= max) {
                Serial.println(value);
                return (uint8_t)value;
            } else {
                Serial.printf("\nInvalid! Must be %d-%d: ", min, max);
            }
        }
        delay(10);
    }
}

uint16_t ConfigManager::readUint16(const char* prompt, uint16_t min, uint16_t max) {
    Serial.printf("\n%s: ", prompt);
    while (Serial.available() > 0) Serial.read();

    while (true) {
        if (Serial.available() > 0) {
            int value = Serial.parseInt();
            while (Serial.available() > 0) Serial.read();

            if (value >= min && value <= max) {
                Serial.println(value);
                return (uint16_t)value;
            } else {
                Serial.printf("\nInvalid! Must be %d-%d: ", min, max);
            }
        }
        delay(10);
    }
}

float ConfigManager::readFloat(const char* prompt, float min, float max) {
    Serial.printf("\n%s: ", prompt);
    while (Serial.available() > 0) Serial.read();

    while (true) {
        if (Serial.available() > 0) {
            float value = Serial.parseFloat();
            while (Serial.available() > 0) Serial.read();

            if (value >= min && value <= max) {
                Serial.println(value);
                return value;
            } else {
                Serial.printf("\nInvalid! Must be %.2f-%.2f: ", min, max);
            }
        }
        delay(10);
    }
}

bool ConfigManager::readBool(const char* prompt) {
    Serial.printf("\n%s (y/n): ", prompt);
    while (Serial.available() > 0) Serial.read();

    while (true) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            while (Serial.available() > 0) Serial.read();

            if (c == 'y' || c == 'Y') {
                Serial.println("YES");
                return true;
            } else if (c == 'n' || c == 'N') {
                Serial.println("NO");
                return false;
            }
        }
        delay(10);
    }
}

void ConfigManager::waitForEnter() {
    Serial.println("\nPress Enter to continue...");
    while (Serial.available() > 0) Serial.read();

    while (true) {
        if (Serial.available() > 0) {
            Serial.read();
            while (Serial.available() > 0) Serial.read();
            break;
        }
        delay(10);
    }
}

void ConfigManager::applyToGame() {
    // Audio settings are applied immediately when changed
    audio.setVolume(m_config.volume);
    audio.setMusicTempo(m_config.musicTempo);

    // Other settings will be read by game engine as needed
    Serial.println("Settings applied!");
}
