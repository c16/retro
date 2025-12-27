/**
 * NEON DEFENDER - Configuration System
 * Interactive serial menu for game settings
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

// Configuration structure
struct GameConfig {
    // Audio settings
    uint8_t volume;              // 0-100
    uint16_t musicTempo;         // BPM (80-200)
    bool musicEnabled;
    bool sfxEnabled;

    // Gameplay settings
    uint8_t difficulty;          // 0=Easy, 1=Normal, 2=Hard, 3=Insane
    uint8_t startingHealth;      // 50-200
    uint8_t powerupDropRate;     // 0-100 (percentage)
    float enemySpeedMultiplier;  // 0.5-2.0
    uint8_t enemiesPerWave;      // Base enemies per wave (3-10)

    // Display settings
    uint8_t brightness;          // 0-100 (if supported)
    bool showFPS;
    bool screenShakeEnabled;
    bool particlesEnabled;

    // Control settings
    bool autoAimEnabled;
    uint8_t rotationSpeed;       // 1-10

    // Advanced
    uint8_t maxParticles;        // 10-100
    uint8_t maxEnemies;          // 5-30
    bool debugMode;
};

class ConfigManager {
public:
    ConfigManager();

    // Initialize and load settings
    bool begin();

    // Save/Load from NVS
    bool save();
    bool load();
    void resetToDefaults();

    // Serial menu interface
    void showMenu();
    void processInput();
    bool checkForMenuRequest(); // Check for 'm' key press

    // Getters
    GameConfig& getConfig() { return m_config; }
    const GameConfig& getConfig() const { return m_config; }

    // Apply settings to game (called by main)
    void applyToGame();

private:
    void showMainMenu();
    void showAudioMenu();
    void showGameplayMenu();
    void showDisplayMenu();
    void showControlMenu();
    void showAdvancedMenu();
    void showCurrentSettings();

    void handleMainMenu(char choice);
    void handleAudioMenu(char choice);
    void handleGameplayMenu(char choice);
    void handleDisplayMenu(char choice);
    void handleControlMenu(char choice);
    void handleAdvancedMenu(char choice);

    // Helper functions
    void printHeader(const char* title);
    void printSeparator();
    void printOption(const char* key, const char* description, const char* value);
    void printOption(const char* key, const char* description, int value);
    void printOption(const char* key, const char* description, float value);
    void printOption(const char* key, const char* description, bool value);

    uint8_t readUint8(const char* prompt, uint8_t min, uint8_t max);
    uint16_t readUint16(const char* prompt, uint16_t min, uint16_t max);
    float readFloat(const char* prompt, float min, float max);
    bool readBool(const char* prompt);
    void waitForEnter();

    GameConfig m_config;
    Preferences m_prefs;

    enum MenuState {
        MENU_MAIN,
        MENU_AUDIO,
        MENU_GAMEPLAY,
        MENU_DISPLAY,
        MENU_CONTROL,
        MENU_ADVANCED
    };

    MenuState m_currentMenu;
    bool m_inMenu;
    unsigned long m_lastInputTime;
};

// Global config manager instance
extern ConfigManager configManager;

#endif // CONFIG_H
