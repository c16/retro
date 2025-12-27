/**
 * NEON DEFENDER - Game Implementation
 */

#include "game.h"
#include "audio.h"
#include <Preferences.h>

Preferences prefs;

NeonDefender::NeonDefender(TFT_eSPI& display)
    : m_tft(display)
    , m_state(STATE_MENU)
    , m_playerAngle(0)
    , m_playerHealth(100)
    , m_maxHealth(100)
    , m_hasShield(false)
    , m_rapidFire(false)
    , m_spreadShot(false)
    , m_score(0)
    , m_highScore(0)
    , m_wave(0)
    , m_enemiesThisWave(0)
    , m_enemiesKilled(0)
    , m_combo(0)
    , m_lastHitTime(0)
    , m_lastShootTime(0)
    , m_shakeX(0)
    , m_shakeY(0)
    , m_menuSelection(0)
    , m_lastBootState(HIGH)
    , m_lastLeftState(HIGH)
    , m_lastRightState(HIGH)
{
}

void NeonDefender::begin() {
    // Load high score
    prefs.begin("neon-defender", false);
    m_highScore = prefs.getInt("highscore", 0);
    prefs.end();

    // Initialize input pins
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);

    // Initialize entities
    for (int i = 0; i < MAX_ENEMIES; i++) m_enemies[i].active = false;
    for (int i = 0; i < MAX_BULLETS; i++) m_bullets[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) m_particles[i].active = false;
    for (int i = 0; i < MAX_POWERUPS; i++) m_powerups[i].active = false;

    Serial.println("NEON DEFENDER initialized!");
}

void NeonDefender::update() {
    handleInput();

    switch (m_state) {
        case STATE_MENU:
            updateMenu();
            break;

        case STATE_PLAYING:
            updatePlaying();
            break;

        case STATE_WAVE_COMPLETE:
            updateWaveComplete();
            break;

        case STATE_GAMEOVER:
            if (millis() - m_gameOverTime > 3000) {
                m_state = STATE_MENU;
                audio.stopMusic();
            }
            break;

        case STATE_PAUSED:
            // Unpause with BOOT button
            break;
    }
}

void NeonDefender::handleInput() {
    bool bootPressed = digitalRead(BOOT_BUTTON) == LOW;
    bool leftPressed = digitalRead(BTN_LEFT) == LOW;
    bool rightPressed = digitalRead(BTN_RIGHT) == LOW;

    // BOOT button (shoot/select)
    if (bootPressed && !m_lastBootState) {
        if (m_state == STATE_MENU) {
            // Start game
            m_state = STATE_PLAYING;
            m_score = 0;
            m_wave = 0;
            m_playerHealth = m_maxHealth;
            m_combo = 0;
            nextWave();
            audio.startMusic();
            audio.playSFX(SFX_MENU_SELECT);
        } else if (m_state == STATE_PLAYING) {
            shoot();
        } else if (m_state == STATE_WAVE_COMPLETE) {
            nextWave();
        }
    }

    // Rotation controls
    if (m_state == STATE_PLAYING) {
        float rotSpeed = 0.08;

        // Auto-rotation mode if no buttons connected
        if (!leftPressed && !rightPressed) {
            // Auto-aim at nearest enemy
            float nearestAngle = m_playerAngle;
            float nearestDist = 999;

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (m_enemies[i].active) {
                    float angleDiff = m_enemies[i].angle - m_playerAngle;
                    while (angleDiff > 180) angleDiff -= 360;
                    while (angleDiff < -180) angleDiff += 360;

                    if (abs(angleDiff) < nearestDist) {
                        nearestDist = abs(angleDiff);
                        nearestAngle = m_enemies[i].angle;
                    }
                }
            }

            // Smoothly rotate toward nearest enemy
            if (nearestDist < 999) {
                float angleDiff = nearestAngle - m_playerAngle;
                while (angleDiff > 180) angleDiff -= 360;
                while (angleDiff < -180) angleDiff += 360;
                m_playerAngle += constrain(angleDiff * 0.1, -rotSpeed * 2, rotSpeed * 2);
            }
        } else {
            // Manual control
            if (leftPressed) {
                m_playerAngle -= rotSpeed;
                audio.playSFX(SFX_MENU_MOVE);
            }
            if (rightPressed) {
                m_playerAngle += rotSpeed;
                audio.playSFX(SFX_MENU_MOVE);
            }
        }

        // Wrap angle
        while (m_playerAngle >= 360) m_playerAngle -= 360;
        while (m_playerAngle < 0) m_playerAngle += 360;
    }

    m_lastBootState = bootPressed;
    m_lastLeftState = leftPressed;
    m_lastRightState = rightPressed;
}

void NeonDefender::updateMenu() {
    // Rotate menu slowly
    m_playerAngle += 0.5;
    if (m_playerAngle >= 360) m_playerAngle = 0;
}

void NeonDefender::updatePlaying() {
    uint32_t now = millis();

    // Update enemies
    int activeEnemies = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!m_enemies[i].active) continue;

        activeEnemies++;

        // Move enemy
        m_enemies[i].radius -= m_enemies[i].speed;
        m_enemies[i].angle += m_enemies[i].angleVel;

        // Zigzag behavior
        if (m_enemies[i].type == ENEMY_ZIGZAG) {
            m_enemies[i].angleVel = sin(m_enemies[i].radius * 0.1) * 0.5;
        }

        // Check if reached center
        if (m_enemies[i].radius < 15) {
            m_enemies[i].active = false;
            m_playerHealth -= 10;
            audio.playSFX(SFX_HIT);
            screenShake(5);
            m_combo = 0;

            if (m_playerHealth <= 0) {
                m_state = STATE_GAMEOVER;
                m_gameOverTime = now;
                audio.playSFX(SFX_GAMEOVER);
                audio.stopMusic();

                // Save high score
                if (m_score > m_highScore) {
                    m_highScore = m_score;
                    prefs.begin("neon-defender", false);
                    prefs.putInt("highscore", m_highScore);
                    prefs.end();
                }
            }
        }

        // Check collision with player
        if (checkPlayerEnemyCollision(m_enemies[i])) {
            if (!m_hasShield) {
                m_playerHealth -= 15;
                audio.playSFX(SFX_HIT);
                screenShake(8);
                m_combo = 0;
            }
            m_enemies[i].active = false;
            spawnExplosion(m_enemies[i].angle, m_enemies[i].radius, m_enemies[i].color);
        }
    }

    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!m_bullets[i].active) continue;

        m_bullets[i].radius += m_bullets[i].speed;

        // Remove if out of bounds
        if (m_bullets[i].radius > RADIUS) {
            m_bullets[i].active = false;
            continue;
        }

        // Check collision with enemies
        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!m_enemies[j].active) continue;

            if (checkBulletEnemyCollision(m_bullets[i], m_enemies[j])) {
                m_bullets[i].active = false;
                m_enemies[j].health--;

                if (m_enemies[j].health <= 0) {
                    // Enemy destroyed
                    m_enemies[j].active = false;
                    m_enemiesKilled++;
                    m_combo++;

                    // Score based on enemy type and combo
                    int points = 10;
                    if (m_enemies[j].type == ENEMY_FAST) points = 15;
                    else if (m_enemies[j].type == ENEMY_TANK) points = 25;
                    else if (m_enemies[j].type == ENEMY_BOSS) points = 100;

                    m_score += points * (1 + m_combo / 5);

                    audio.playSFX(SFX_HIT);
                    if (m_combo % 5 == 0 && m_combo > 0) {
                        audio.playSFX(SFX_COMBO);
                    }

                    spawnExplosion(m_enemies[j].angle, m_enemies[j].radius, m_enemies[j].color);

                    // Chance to drop powerup
                    if (random(100) < 15) {
                        PowerupType type = (PowerupType)random(0, 5);
                        spawnPowerup(type, m_enemies[j].angle, m_enemies[j].radius);
                    }
                } else {
                    // Just hit
                    audio.playSFX(SFX_SHOOT);
                }

                m_lastHitTime = now;
                break;
            }
        }
    }

    // Update powerups
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!m_powerups[i].active) continue;

        // Spiral inward
        m_powerups[i].radius -= m_powerups[i].speed;
        m_powerups[i].angle += 1.0;

        // Remove if too close to center or expired
        if (m_powerups[i].radius < 10 || (now - m_powerups[i].spawnTime) > 10000) {
            m_powerups[i].active = false;
            continue;
        }

        // Check collision with player
        if (checkPlayerPowerupCollision(m_powerups[i])) {
            m_powerups[i].active = false;
            audio.playSFX(SFX_POWERUP);

            switch (m_powerups[i].type) {
                case POWERUP_RAPID_FIRE:
                    m_rapidFire = true;
                    m_rapidFireEndTime = now + 10000;
                    break;
                case POWERUP_SPREAD_SHOT:
                    m_spreadShot = true;
                    m_spreadShotEndTime = now + 10000;
                    break;
                case POWERUP_SHIELD:
                    m_hasShield = true;
                    m_shieldEndTime = now + 15000;
                    break;
                case POWERUP_NUKE:
                    // Destroy all enemies
                    for (int j = 0; j < MAX_ENEMIES; j++) {
                        if (m_enemies[j].active) {
                            spawnExplosion(m_enemies[j].angle, m_enemies[j].radius, TFT_YELLOW);
                            m_enemies[j].active = false;
                            m_enemiesKilled++;
                            m_score += 5;
                        }
                    }
                    screenShake(15);
                    break;
                case POWERUP_HEALTH:
                    m_playerHealth = min(m_playerHealth + 30, m_maxHealth);
                    break;
            }
        }
    }

    // Update particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!m_particles[i].active) continue;

        m_particles[i].x += m_particles[i].vx;
        m_particles[i].y += m_particles[i].vy;
        m_particles[i].life--;

        if (m_particles[i].life <= 0) {
            m_particles[i].active = false;
        }
    }

    // Update powerup timers
    if (m_rapidFire && now > m_rapidFireEndTime) m_rapidFire = false;
    if (m_spreadShot && now > m_spreadShotEndTime) m_spreadShot = false;
    if (m_hasShield && now > m_shieldEndTime) m_hasShield = false;

    // Update screen shake
    if (now > m_shakeEndTime) {
        m_shakeX = 0;
        m_shakeY = 0;
    }

    // Reset combo if no hits for a while
    if (now - m_lastHitTime > 2000) {
        m_combo = 0;
    }

    // Spawn enemies for current wave
    if (activeEnemies == 0 && m_enemiesKilled >= m_enemiesThisWave) {
        completeWave();
    } else if (activeEnemies < 5 && m_enemiesKilled < m_enemiesThisWave) {
        // Spawn more enemies
        if (random(100) < 5) {
            EnemyType type = ENEMY_BASIC;
            int r = random(100);
            if (r < 40) type = ENEMY_BASIC;
            else if (r < 60) type = ENEMY_FAST;
            else if (r < 80) type = ENEMY_ZIGZAG;
            else type = ENEMY_TANK;

            spawnEnemy(type);
        }
    }
}

void NeonDefender::updateWaveComplete() {
    if (millis() - m_waveCompleteTime > 3000) {
        nextWave();
    }
}

void NeonDefender::shoot() {
    uint32_t now = millis();
    uint32_t fireDelay = m_rapidFire ? 100 : 200;

    if (now - m_lastShootTime < fireDelay) return;

    if (m_spreadShot) {
        shootSpread();
    } else {
        // Find free bullet slot
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!m_bullets[i].active) {
                m_bullets[i].active = true;
                m_bullets[i].angle = m_playerAngle;
                m_bullets[i].radius = 20;
                m_bullets[i].speed = 3.0;
                m_bullets[i].color = TFT_CYAN;
                break;
            }
        }
    }

    audio.playSFX(SFX_SHOOT);
    m_lastShootTime = now;
}

void NeonDefender::shootSpread() {
    // Shoot 5 bullets in a spread
    float angles[] = {-20, -10, 0, 10, 20};

    for (int a = 0; a < 5; a++) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!m_bullets[i].active) {
                m_bullets[i].active = true;
                m_bullets[i].angle = m_playerAngle + angles[a];
                m_bullets[i].radius = 20;
                m_bullets[i].speed = 2.5;
                m_bullets[i].color = TFT_MAGENTA;
                break;
            }
        }
    }
}

void NeonDefender::spawnEnemy(EnemyType type) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!m_enemies[i].active) {
            m_enemies[i].active = true;
            m_enemies[i].angle = random(0, 360);
            m_enemies[i].radius = RADIUS - 5;
            m_enemies[i].angleVel = 0;
            m_enemies[i].type = type;

            switch (type) {
                case ENEMY_BASIC:
                    m_enemies[i].speed = 0.5;
                    m_enemies[i].health = 1;
                    m_enemies[i].color = TFT_RED;
                    break;
                case ENEMY_FAST:
                    m_enemies[i].speed = 1.2;
                    m_enemies[i].health = 1;
                    m_enemies[i].color = TFT_YELLOW;
                    break;
                case ENEMY_ZIGZAG:
                    m_enemies[i].speed = 0.7;
                    m_enemies[i].health = 1;
                    m_enemies[i].color = TFT_GREEN;
                    m_enemies[i].angleVel = 0.5;
                    break;
                case ENEMY_TANK:
                    m_enemies[i].speed = 0.3;
                    m_enemies[i].health = 3;
                    m_enemies[i].color = TFT_ORANGE;
                    break;
                case ENEMY_BOSS:
                    m_enemies[i].speed = 0.2;
                    m_enemies[i].health = 10;
                    m_enemies[i].color = TFT_PURPLE;
                    break;
            }
            break;
        }
    }
}

void NeonDefender::spawnPowerup(PowerupType type, float angle, float radius) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!m_powerups[i].active) {
            m_powerups[i].active = true;
            m_powerups[i].type = type;
            m_powerups[i].angle = angle;
            m_powerups[i].radius = radius;
            m_powerups[i].speed = 0.2;
            m_powerups[i].spawnTime = millis();
            break;
        }
    }
}

void NeonDefender::spawnExplosion(float angle, float radius, uint16_t color) {
    int16_t x, y;
    polarToCartesian(angle, radius, &x, &y);
    spawnParticles(x, y, 15, color, PARTICLE_EXPLOSION);
}

void NeonDefender::spawnParticles(float x, float y, int count, uint16_t color, ParticleType type) {
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!m_particles[j].active) {
                m_particles[j].active = true;
                m_particles[j].x = x;
                m_particles[j].y = y;
                float angle = random(0, 360) * PI / 180.0;
                float speed = random(1, 4);
                m_particles[j].vx = cos(angle) * speed;
                m_particles[j].vy = sin(angle) * speed;
                m_particles[j].color = color;
                m_particles[j].life = random(10, 30);
                m_particles[j].type = type;
                break;
            }
        }
    }
}

void NeonDefender::screenShake(int intensity) {
    m_shakeX = random(-intensity, intensity);
    m_shakeY = random(-intensity, intensity);
    m_shakeEndTime = millis() + 100;
}

bool NeonDefender::checkBulletEnemyCollision(Bullet& bullet, Enemy& enemy) {
    float angleDiff = abs(bullet.angle - enemy.angle);
    if (angleDiff > 180) angleDiff = 360 - angleDiff;

    float radiusDiff = abs(bullet.radius - enemy.radius);

    return (angleDiff < 5 && radiusDiff < 8);
}

bool NeonDefender::checkPlayerEnemyCollision(Enemy& enemy) {
    float angleDiff = abs(m_playerAngle - enemy.angle);
    if (angleDiff > 180) angleDiff = 360 - angleDiff;

    return (angleDiff < 10 && enemy.radius < 25);
}

bool NeonDefender::checkPlayerPowerupCollision(Powerup& powerup) {
    float angleDiff = abs(m_playerAngle - powerup.angle);
    if (angleDiff > 180) angleDiff = 360 - angleDiff;

    return (angleDiff < 15 && powerup.radius < 30);
}

void NeonDefender::startWave(int waveNum) {
    m_wave = waveNum;
    m_enemiesKilled = 0;
    m_enemiesThisWave = 5 + waveNum * 3;

    Serial.printf("Starting wave %d (%d enemies)\n", waveNum, m_enemiesThisWave);

    // Boss wave every 5 waves
    if (waveNum % 5 == 0) {
        spawnEnemy(ENEMY_BOSS);
        m_enemiesThisWave = 1;
    }
}

void NeonDefender::completeWave() {
    m_state = STATE_WAVE_COMPLETE;
    m_waveCompleteTime = millis();
    m_score += m_wave * 50;
    audio.playSFX(SFX_WAVE_COMPLETE);

    // Heal player
    m_playerHealth = min(m_playerHealth + 20, m_maxHealth);
}

void NeonDefender::nextWave() {
    m_state = STATE_PLAYING;
    startWave(m_wave + 1);
}

void NeonDefender::polarToCartesian(float angle, float radius, int16_t* x, int16_t* y) {
    float rad = angle * PI / 180.0;
    *x = CENTER_X + radius * cos(rad) + m_shakeX;
    *y = CENTER_Y + radius * sin(rad) + m_shakeY;
}

uint16_t NeonDefender::getNeonColor(int hue) {
    // Convert HSV to RGB (neon colors)
    float h = hue % 360;
    float s = 1.0;
    float v = 1.0;

    float c = v * s;
    float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
    float m = v - c;

    float r, g, b;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return m_tft.color565((r + m) * 255, (g + m) * 255, (b + m) * 255);
}

void NeonDefender::render() {
    switch (m_state) {
        case STATE_MENU:
            renderMenu();
            break;
        case STATE_PLAYING:
            renderGame();
            break;
        case STATE_WAVE_COMPLETE:
            renderWaveComplete();
            break;
        case STATE_GAMEOVER:
            renderGame(); // Show final state
            m_tft.setTextColor(TFT_RED, TFT_BLACK);
            m_tft.setTextDatum(MC_DATUM);
            m_tft.setTextSize(2);
            m_tft.drawString("GAME OVER", CENTER_X, CENTER_Y - 10);
            m_tft.setTextSize(1);
            m_tft.drawString(String(m_score) + " points", CENTER_X, CENTER_Y + 15);
            break;
    }
}

void NeonDefender::renderMenu() {
    m_tft.fillScreen(TFT_BLACK);

    // Animated title
    uint32_t hue = (millis() / 10) % 360;
    m_tft.setTextColor(getNeonColor(hue), TFT_BLACK);
    m_tft.setTextDatum(MC_DATUM);
    m_tft.setTextSize(2);
    m_tft.drawString("NEON", CENTER_X, CENTER_Y - 30);
    m_tft.drawString("DEFENDER", CENTER_X, CENTER_Y - 10);

    // Instructions
    m_tft.setTextSize(1);
    m_tft.setTextColor(TFT_CYAN, TFT_BLACK);
    m_tft.drawString("Press BOOT", CENTER_X, CENTER_Y + 20);
    m_tft.drawString("to Start", CENTER_X, CENTER_Y + 35);

    // High score
    if (m_highScore > 0) {
        m_tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        m_tft.drawString("HI: " + String(m_highScore), CENTER_X, CENTER_Y + 55);
    }

    // Rotating circle effect
    for (int i = 0; i < 12; i++) {
        float angle = (m_playerAngle + i * 30) * PI / 180.0;
        int16_t x = CENTER_X + (RADIUS - 5) * cos(angle);
        int16_t y = CENTER_Y + (RADIUS - 5) * sin(angle);
        m_tft.fillCircle(x, y, 2, getNeonColor((hue + i * 30) % 360));
    }
}

void NeonDefender::renderGame() {
    m_tft.fillScreen(TFT_BLACK);

    // Draw arena circle
    m_tft.drawCircle(CENTER_X + m_shakeX, CENTER_Y + m_shakeY, RADIUS - 2, TFT_DARKGREY);

    renderParticles();
    renderEnemies();
    renderBullets();
    renderPowerups();
    renderPlayer();
    renderUI();
}

void NeonDefender::renderPlayer() {
    int16_t x, y;
    polarToCartesian(m_playerAngle, 18, &x, &y);

    // Draw player turret
    m_tft.fillCircle(x, y, 6, TFT_CYAN);

    // Draw barrel
    int16_t bx, by;
    polarToCartesian(m_playerAngle, 26, &bx, &by);
    m_tft.drawLine(x, y, bx, by, TFT_CYAN);

    // Shield effect
    if (m_hasShield) {
        uint32_t t = millis() / 100;
        for (int i = 0; i < 3; i++) {
            int r = 10 + i * 3;
            m_tft.drawCircle(x, y, r, (t % 2 == 0) ? TFT_BLUE : TFT_SKYBLUE);
        }
    }

    // Powerup indicators as small circles around player
    if (m_rapidFire) {
        m_tft.fillCircle(x - 8, y, 2, TFT_RED);
    }
    if (m_spreadShot) {
        m_tft.fillCircle(x + 8, y, 2, TFT_MAGENTA);
    }
}

void NeonDefender::renderEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!m_enemies[i].active) continue;

        int16_t x, y;
        polarToCartesian(m_enemies[i].angle, m_enemies[i].radius, &x, &y);

        // Size based on type
        int size = 4;
        if (m_enemies[i].type == ENEMY_TANK) size = 6;
        else if (m_enemies[i].type == ENEMY_BOSS) size = 10;

        // Draw enemy
        m_tft.fillCircle(x, y, size, m_enemies[i].color);

        // Health bar for tough enemies
        if (m_enemies[i].health > 1) {
            int barWidth = size * 2;
            int healthWidth = (barWidth * m_enemies[i].health) / ((m_enemies[i].type == ENEMY_BOSS) ? 10 : 3);
            m_tft.drawFastHLine(x - barWidth/2, y - size - 3, barWidth, TFT_DARKGREY);
            m_tft.drawFastHLine(x - barWidth/2, y - size - 3, healthWidth, TFT_GREEN);
        }

        // Pulsing effect for boss
        if (m_enemies[i].type == ENEMY_BOSS) {
            int pulse = 2 + sin(millis() / 100.0) * 2;
            m_tft.drawCircle(x, y, size + pulse, m_enemies[i].color);
        }
    }
}

void NeonDefender::renderBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!m_bullets[i].active) continue;

        int16_t x, y;
        polarToCartesian(m_bullets[i].angle, m_bullets[i].radius, &x, &y);

        m_tft.fillCircle(x, y, 2, m_bullets[i].color);

        // Trail effect
        int16_t tx, ty;
        polarToCartesian(m_bullets[i].angle, m_bullets[i].radius - 3, &tx, &ty);
        m_tft.drawLine(x, y, tx, ty, m_bullets[i].color);
    }
}

void NeonDefender::renderPowerups() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!m_powerups[i].active) continue;

        int16_t x, y;
        polarToCartesian(m_powerups[i].angle, m_powerups[i].radius, &x, &y);

        // Rotating icon based on type
        uint16_t color = TFT_WHITE;
        char symbol = '?';

        switch (m_powerups[i].type) {
            case POWERUP_RAPID_FIRE: color = TFT_RED; symbol = 'R'; break;
            case POWERUP_SPREAD_SHOT: color = TFT_MAGENTA; symbol = 'S'; break;
            case POWERUP_SHIELD: color = TFT_BLUE; symbol = 'B'; break;
            case POWERUP_NUKE: color = TFT_YELLOW; symbol = 'N'; break;
            case POWERUP_HEALTH: color = TFT_GREEN; symbol = '+'; break;
        }

        // Draw powerup with pulsing effect
        int pulse = 2 + sin(millis() / 100.0 + i) * 1;
        m_tft.drawCircle(x, y, 5 + pulse, color);
        m_tft.setTextColor(color, TFT_BLACK);
        m_tft.setTextDatum(MC_DATUM);
        m_tft.drawChar(symbol, x, y);
    }
}

void NeonDefender::renderParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!m_particles[i].active) continue;

        int size = 1;
        if (m_particles[i].type == PARTICLE_EXPLOSION) {
            size = 2;
        }

        // Fade out based on life
        uint16_t color = m_particles[i].color;

        m_tft.fillCircle(m_particles[i].x + m_shakeX, m_particles[i].y + m_shakeY, size, color);
    }
}

void NeonDefender::renderUI() {
    // Health bar (arc around center)
    float healthPercent = (float)m_playerHealth / m_maxHealth;
    int healthAngle = healthPercent * 360;

    for (int a = 0; a < healthAngle; a += 2) {
        float rad = a * PI / 180.0;
        int16_t x = CENTER_X + 12 * cos(rad);
        int16_t y = CENTER_Y + 12 * sin(rad);

        uint16_t color;
        if (healthPercent > 0.6) color = TFT_GREEN;
        else if (healthPercent > 0.3) color = TFT_YELLOW;
        else color = TFT_RED;

        m_tft.fillCircle(x, y, 1, color);
    }

    // Score
    m_tft.setTextDatum(TL_DATUM);
    m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft.setTextSize(1);
    m_tft.drawString(String(m_score), 5, 5);

    // Wave
    m_tft.setTextDatum(TR_DATUM);
    m_tft.drawString("W" + String(m_wave), 155, 5);

    // Combo
    if (m_combo > 1) {
        m_tft.setTextDatum(TC_DATUM);
        m_tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        m_tft.drawString("x" + String(m_combo), CENTER_X, 145);
    }

    // Powerup timers (small dots at bottom)
    int dotY = 150;
    int dotX = CENTER_X - 20;
    if (m_rapidFire) {
        m_tft.fillCircle(dotX, dotY, 2, TFT_RED);
        dotX += 10;
    }
    if (m_spreadShot) {
        m_tft.fillCircle(dotX, dotY, 2, TFT_MAGENTA);
        dotX += 10;
    }
    if (m_hasShield) {
        m_tft.fillCircle(dotX, dotY, 2, TFT_BLUE);
    }
}

void NeonDefender::renderWaveComplete() {
    renderGame();

    // Wave complete message
    m_tft.setTextColor(TFT_GREEN, TFT_BLACK);
    m_tft.setTextDatum(MC_DATUM);
    m_tft.setTextSize(2);
    m_tft.drawString("WAVE " + String(m_wave), CENTER_X, CENTER_Y - 15);
    m_tft.setTextSize(1);
    m_tft.drawString("COMPLETE!", CENTER_X, CENTER_Y + 5);

    // Bonus points
    m_tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    m_tft.drawString("+" + String(m_wave * 50), CENTER_X, CENTER_Y + 25);
}
