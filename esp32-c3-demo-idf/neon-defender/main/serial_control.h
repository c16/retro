/**
 * NEON DEFENDER - Serial Control System
 * Play the game through USB serial terminal!
 */

#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

#include <Arduino.h>

// Virtual button states controlled by serial
struct SerialControls {
    bool shootPressed;
    bool leftPressed;
    bool rightPressed;
    bool startPressed;
    bool pausePressed;
    bool restartPressed;

    // Debug commands
    bool spawnPowerup;
    bool skipWave;
    bool addHealth;
    bool killAllEnemies;

    // Control mode
    bool enabled;
};

class SerialController {
public:
    SerialController();

    // Initialize
    void begin();

    // Process incoming serial commands
    void update();

    // Get control state
    SerialControls& getControls() { return m_controls; }

    // Check if serial controls are active
    bool isActive() { return m_controls.enabled; }

    // Enable/disable serial controls
    void setEnabled(bool enabled);

    // Show help
    void showHelp();

private:
    void processCommand(char cmd);
    void handleGameCommand(char cmd);
    void handleDebugCommand(char cmd);

    SerialControls m_controls;
    bool m_helpShown;
    unsigned long m_lastCommandTime;
};

// Global serial controller instance
extern SerialController serialController;

#endif // SERIAL_CONTROL_H
