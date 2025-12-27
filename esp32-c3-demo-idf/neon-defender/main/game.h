/**
 * NEON DEFENDER - Game Engine
 * Wild circular shooter with rhythm elements
 */

#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <vector>
#include "config.h"

// Display constants
#define CENTER_X 80
#define CENTER_Y 80
#define RADIUS 80

// Game constants
#define MAX_ENEMIES 20
#define MAX_BULLETS 15
#define MAX_PARTICLES 50
#define MAX_POWERUPS 5

// Button pins
#define BOOT_BUTTON 9    // Onboard BOOT button
#define BTN_LEFT 2       // Optional: Rotate left (external)
#define BTN_RIGHT 3      // Optional: Rotate right (external)

// Game states
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_WAVE_COMPLETE
};

// Enemy types
enum EnemyType {
    ENEMY_BASIC,      // Simple, moves straight
    ENEMY_ZIGZAG,     // Zigzag movement
    ENEMY_FAST,       // Fast but weak
    ENEMY_TANK,       // Slow but tough
    ENEMY_BOMBER,     // Drops bombs
    ENEMY_BOSS        // Boss enemy
};

// Powerup types
enum PowerupType {
    POWERUP_RAPID_FIRE,
    POWERUP_SPREAD_SHOT,
    POWERUP_SHIELD,
    POWERUP_NUKE,
    POWERUP_HEALTH
};

// Particle types
enum ParticleType {
    PARTICLE_SPARK,
    PARTICLE_EXPLOSION,
    PARTICLE_TRAIL,
    PARTICLE_STAR
};

// Entities
struct Enemy {
    float angle;        // Position around circle
    float radius;       // Distance from center
    float speed;        // Inward speed
    float angleVel;     // Angular velocity
    EnemyType type;
    int health;
    bool active;
    uint16_t color;
};

struct Bullet {
    float angle;
    float radius;
    float speed;
    bool active;
    uint16_t color;
};

struct Particle {
    float x, y;
    float vx, vy;
    uint16_t color;
    int life;
    ParticleType type;
    bool active;
};

struct Powerup {
    float angle;
    float radius;
    float speed;
    PowerupType type;
    bool active;
    uint32_t spawnTime;
};

class NeonDefender {
public:
    NeonDefender(TFT_eSPI& display);

    void begin();
    void update();
    void render();

    // Input handling
    void handleInput();

    // Game state
    GameState getState() { return m_state; }

private:
    // Game logic
    void updateMenu();
    void updatePlaying();
    void updateWaveComplete();

    // Spawning
    void spawnEnemy(EnemyType type);
    void spawnBoss();
    void spawnPowerup(PowerupType type, float angle, float radius);

    // Shooting
    void shoot();
    void shootSpread();

    // Collision detection
    bool checkBulletEnemyCollision(Bullet& bullet, Enemy& enemy);
    bool checkPlayerEnemyCollision(Enemy& enemy);
    bool checkPlayerPowerupCollision(Powerup& powerup);

    // Wave management
    void startWave(int waveNum);
    void completeWave();
    void nextWave();

    // Effects
    void spawnExplosion(float angle, float radius, uint16_t color);
    void spawnParticles(float x, float y, int count, uint16_t color, ParticleType type);
    void screenShake(int intensity);

    // Rendering
    void renderMenu();
    void renderGame();
    void renderPlayer();
    void renderEnemies();
    void renderBullets();
    void renderPowerups();
    void renderParticles();
    void renderUI();
    void renderWaveComplete();

    // Utilities
    void polarToCartesian(float angle, float radius, int16_t* x, int16_t* y);
    uint16_t getNeonColor(int hue);
    uint16_t colorLerp(uint16_t c1, uint16_t c2, float t);

    TFT_eSPI& m_tft;
    GameState m_state;

    // Player
    float m_playerAngle;
    int m_playerHealth;
    int m_maxHealth;
    bool m_hasShield;
    uint32_t m_shieldEndTime;
    bool m_rapidFire;
    uint32_t m_rapidFireEndTime;
    bool m_spreadShot;
    uint32_t m_spreadShotEndTime;

    // Game state
    int m_score;
    int m_highScore;
    int m_wave;
    int m_enemiesThisWave;
    int m_enemiesKilled;
    int m_combo;
    uint32_t m_lastHitTime;
    uint32_t m_lastShootTime;

    // Entities
    Enemy m_enemies[MAX_ENEMIES];
    Bullet m_bullets[MAX_BULLETS];
    Particle m_particles[MAX_PARTICLES];
    Powerup m_powerups[MAX_POWERUPS];

    // Screen shake
    int m_shakeX;
    int m_shakeY;
    uint32_t m_shakeEndTime;

    // Menu
    int m_menuSelection;

    // Input
    bool m_lastBootState;
    bool m_lastLeftState;
    bool m_lastRightState;

    // Timing
    uint32_t m_waveCompleteTime;
    uint32_t m_gameOverTime;
};

#endif // GAME_H
