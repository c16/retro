/**
 * MARBLE RUNNER - Realistic 3D Marble Game
 * ESP32-C3 with Round Display
 */

#ifndef MARBLE_GAME_H
#define MARBLE_GAME_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <cmath>

// Hardware configuration
#define BOOT_BUTTON 9

// Game configuration
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 160
#define SCREEN_CENTER_X 80
#define SCREEN_CENTER_Y 80

#define TRACK_RADIUS 60.0f
#define TRACK_WIDTH 30.0f
#define MARBLE_RADIUS 8.0f

#define MAX_OBSTACLES 8
#define MAX_PARTICLES 30
#define MAX_COLLECTIBLES 5

// Physics constants
#define GRAVITY 9.8f
#define FRICTION 0.98f
#define AIR_RESISTANCE 0.99f
#define MAX_SPEED 150.0f
#define ACCELERATION 80.0f
#define JUMP_FORCE 120.0f
#define BOUNCE_DAMPENING 0.6f

// Game states
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER
};

// Obstacle types
enum ObstacleType {
    OBSTACLE_HOLE,      // Fall through
    OBSTACLE_BUMP,      // Jump ramp
    OBSTACLE_BARRIER,   // Wall to avoid
    OBSTACLE_SPINNER    // Rotating hazard
};

// Collectible types
enum CollectibleType {
    COLLECT_COIN,       // Points
    COLLECT_SPEEDUP,    // Temporary speed boost
    COLLECT_INVINCIBLE, // Temporary invincibility
    COLLECT_EXTRALIFE   // Extra life
};

// 2D Vector for positions and velocities
struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float f) const { return Vec2(x * f, y * f); }
    Vec2 operator/(float f) const { return Vec2(x / f, y / f); }

    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator*=(float f) { x *= f; y *= f; return *this; }

    float length() const { return sqrtf(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }

    Vec2 normalized() const {
        float len = length();
        if (len < 0.0001f) return Vec2(0, 0);
        return Vec2(x / len, y / len);
    }

    float dot(const Vec2& v) const { return x * v.x + y * v.y; }

    // Rotate vector by angle (radians)
    Vec2 rotated(float angle) const {
        float c = cosf(angle);
        float s = sinf(angle);
        return Vec2(x * c - y * s, x * s + y * c);
    }
};

// 3D Vector for lighting and rendering
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float f) const { return Vec3(x * f, y * f, z * f); }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    float length() const { return sqrtf(x * x + y * y + z * z); }

    Vec3 normalized() const {
        float len = length();
        if (len < 0.0001f) return Vec3(0, 1, 0);
        return Vec3(x / len, y / len, z / len);
    }
};

// Marble structure with physics
struct Marble {
    Vec2 position;      // World position
    Vec2 velocity;      // Velocity vector
    float height;       // Height above track (for jumping)
    float verticalVel;  // Vertical velocity
    float angle;        // Angular position on track (radians)
    float rotation;     // Visual rotation angle
    bool grounded;      // On track or in air
    bool alive;         // Not fallen off

    // Powerup state
    float speedBoost;
    float invincibleTime;

    Marble() : position(0, 0), velocity(0, 0), height(0), verticalVel(0),
               angle(0), rotation(0), grounded(true), alive(true),
               speedBoost(0), invincibleTime(0) {}
};

// Obstacle on track
struct Obstacle {
    float angle;        // Position on circular track
    ObstacleType type;
    float size;
    float rotationAngle; // For spinners
    bool active;

    Obstacle() : angle(0), type(OBSTACLE_HOLE), size(20),
                 rotationAngle(0), active(false) {}
};

// Collectible item
struct Collectible {
    float angle;
    CollectibleType type;
    bool collected;
    float bobOffset;    // For floating animation

    Collectible() : angle(0), type(COLLECT_COIN),
                    collected(false), bobOffset(0) {}
};

// Particle for visual effects
struct Particle {
    Vec2 position;
    Vec2 velocity;
    float lifetime;
    float maxLifetime;
    uint16_t color;
    bool active;

    Particle() : position(0, 0), velocity(0, 0), lifetime(0),
                 maxLifetime(1.0f), color(TFT_WHITE), active(false) {}
};

// Main game class
class MarbleGame {
public:
    MarbleGame(TFT_eSPI* display);

    void begin();
    void update(float deltaTime);
    void render();
    void handleInput();

    // Game state
    GameState getState() const { return m_state; }
    int getScore() const { return m_score; }
    int getLives() const { return m_lives; }

private:
    TFT_eSPI* m_tft;
    GameState m_state;

    // Game objects
    Marble m_marble;
    Obstacle m_obstacles[MAX_OBSTACLES];
    Collectible m_collectibles[MAX_COLLECTIBLES];
    Particle m_particles[MAX_PARTICLES];

    // Game stats
    int m_score;
    int m_lives;
    float m_distance;
    float m_trackSpeed;
    int m_level;

    // Input state
    bool m_buttonPressed;
    bool m_lastButtonState;
    unsigned long m_buttonPressTime;

    // Timing
    float m_gameTime;
    unsigned long m_lastObstacleSpawn;
    unsigned long m_lastCollectibleSpawn;

    // Camera shake
    float m_shakeAmount;
    Vec2 m_shakeOffset;

    // Rendering helpers
    void renderMenu();
    void renderGame();
    void renderGameOver();
    void renderHUD();

    // Marble rendering
    void renderMarble();
    void renderMarbleShadow();
    uint16_t calculateMarbleShading(Vec3 normal, Vec3 lightDir);

    // Track rendering
    void renderTrack();
    void renderTrackSegment(float startAngle, float endAngle, uint16_t color);

    // Object rendering
    void renderObstacles();
    void renderObstacle(const Obstacle& obs);
    void renderCollectibles();
    void renderCollectible(const Collectible& col);
    void renderParticles();

    // Physics
    void updatePhysics(float deltaTime);
    void updateMarblePosition(float deltaTime);
    void checkCollisions();
    void applyGravity(float deltaTime);

    // Game logic
    void spawnObstacle();
    void spawnCollectible();
    void collectItem(const Collectible& col);
    void hitObstacle(const Obstacle& obs);
    void resetMarble();
    void nextLevel();

    // Particles
    void spawnParticles(Vec2 position, uint16_t color, int count);
    void updateParticles(float deltaTime);

    // Utilities
    Vec2 angleToPosition(float angle, float radius);
    float positionToAngle(Vec2 pos);
    float normalizeAngle(float angle);
    uint16_t rgb(uint8_t r, uint8_t g, uint8_t b);
    uint16_t blendColors(uint16_t c1, uint16_t c2, float t);
};

#endif // MARBLE_GAME_H
