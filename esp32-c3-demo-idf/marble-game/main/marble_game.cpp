/**
 * MARBLE RUNNER - Implementation
 */

#include "marble_game.h"

MarbleGame::MarbleGame(TFT_eSPI* display)
    : m_tft(display)
    , m_state(STATE_MENU)
    , m_score(0)
    , m_lives(3)
    , m_distance(0)
    , m_trackSpeed(30.0f)
    , m_level(1)
    , m_buttonPressed(false)
    , m_lastButtonState(false)
    , m_buttonPressTime(0)
    , m_gameTime(0)
    , m_lastObstacleSpawn(0)
    , m_lastCollectibleSpawn(0)
    , m_shakeAmount(0)
    , m_shakeOffset(0, 0)
{
}

void MarbleGame::begin() {
    pinMode(BOOT_BUTTON, INPUT_PULLUP);

    Serial.println("Marble Runner initialized");

    // Initialize marble at start position
    resetMarble();

    // Spawn initial obstacles and collectibles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        m_obstacles[i].active = false;
    }

    for (int i = 0; i < MAX_COLLECTIBLES; i++) {
        m_collectibles[i].collected = false;
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        m_particles[i].active = false;
    }

    // Spawn some initial items
    for (int i = 0; i < 4; i++) {
        spawnObstacle();
    }

    for (int i = 0; i < 3; i++) {
        spawnCollectible();
    }
}

void MarbleGame::update(float deltaTime) {
    m_gameTime += deltaTime;

    switch (m_state) {
        case STATE_MENU:
            handleInput();
            if (m_buttonPressed && !m_lastButtonState) {
                m_state = STATE_PLAYING;
                m_score = 0;
                m_lives = 3;
                m_distance = 0;
                m_level = 1;
                m_trackSpeed = 30.0f;
                resetMarble();
            }
            break;

        case STATE_PLAYING:
            handleInput();
            updatePhysics(deltaTime);
            updateParticles(deltaTime);
            checkCollisions();

            // Auto-advance marble along track
            m_distance += m_trackSpeed * deltaTime;
            m_marble.angle += (m_trackSpeed / TRACK_RADIUS) * deltaTime;
            m_marble.angle = normalizeAngle(m_marble.angle);

            // Increase difficulty over time
            m_trackSpeed = 30.0f + m_level * 5.0f;

            // Spawn obstacles
            if (millis() - m_lastObstacleSpawn > 2000 - m_level * 100) {
                spawnObstacle();
                m_lastObstacleSpawn = millis();
            }

            // Spawn collectibles
            if (millis() - m_lastCollectibleSpawn > 3000) {
                spawnCollectible();
                m_lastCollectibleSpawn = millis();
            }

            // Update camera shake
            if (m_shakeAmount > 0) {
                m_shakeAmount *= 0.9f;
                m_shakeOffset = Vec2(
                    (random(-100, 100) / 100.0f) * m_shakeAmount,
                    (random(-100, 100) / 100.0f) * m_shakeAmount
                );
            }

            // Level up every 500 distance
            if ((int)(m_distance / 500) > m_level - 1) {
                nextLevel();
            }

            // Check if marble fell off
            if (!m_marble.alive) {
                m_lives--;
                if (m_lives <= 0) {
                    m_state = STATE_GAMEOVER;
                    spawnParticles(m_marble.position, TFT_RED, 20);
                } else {
                    resetMarble();
                    m_shakeAmount = 5.0f;
                }
            }

            // Pause
            if (m_buttonPressed && !m_lastButtonState && millis() - m_buttonPressTime > 1000) {
                // Long press for pause (not implemented in this version)
            }
            break;

        case STATE_GAMEOVER:
            handleInput();
            updateParticles(deltaTime);
            if (m_buttonPressed && !m_lastButtonState) {
                m_state = STATE_MENU;
            }
            break;

        case STATE_PAUSED:
            handleInput();
            break;
    }

    m_lastButtonState = m_buttonPressed;
}

void MarbleGame::handleInput() {
    m_buttonPressed = (digitalRead(BOOT_BUTTON) == LOW);

    if (m_state == STATE_PLAYING) {
        // Button pressed - make marble jump
        if (m_buttonPressed && !m_lastButtonState && m_marble.grounded) {
            m_marble.verticalVel = JUMP_FORCE;
            m_marble.grounded = false;
            spawnParticles(m_marble.position, TFT_WHITE, 5);
        }

        // Hold button for speed boost
        if (m_buttonPressed) {
            m_marble.speedBoost = 1.5f;
        } else {
            m_marble.speedBoost = 1.0f;
        }
    }
}

void MarbleGame::updatePhysics(float deltaTime) {
    updateMarblePosition(deltaTime);
    applyGravity(deltaTime);

    // Update marble rotation based on speed
    m_marble.rotation += (m_trackSpeed + m_marble.velocity.length()) * deltaTime * 0.1f;

    // Update powerups
    if (m_marble.invincibleTime > 0) {
        m_marble.invincibleTime -= deltaTime;
    }

    // Update obstacle animations
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (m_obstacles[i].active && m_obstacles[i].type == OBSTACLE_SPINNER) {
            m_obstacles[i].rotationAngle += deltaTime * 2.0f;
        }
    }

    // Update collectible animations
    for (int i = 0; i < MAX_COLLECTIBLES; i++) {
        if (!m_collectibles[i].collected) {
            m_collectibles[i].bobOffset = sinf(m_gameTime * 3.0f + i) * 3.0f;
        }
    }
}

void MarbleGame::updateMarblePosition(float deltaTime) {
    // Apply friction
    m_marble.velocity *= FRICTION;

    // Update position
    m_marble.position += m_marble.velocity * deltaTime;

    // Keep marble on track (circular constraint)
    float distFromCenter = m_marble.position.length();
    if (distFromCenter > 0.1f) {
        Vec2 normal = m_marble.position.normalized();
        float deviation = distFromCenter - TRACK_RADIUS;

        // Constrain to track radius
        if (fabs(deviation) > TRACK_WIDTH / 2) {
            if (!m_marble.grounded) {
                // Fell off track
                m_marble.alive = false;
            } else {
                // Bounce back to track
                m_marble.position = normal * (TRACK_RADIUS + (deviation > 0 ? TRACK_WIDTH/2 : -TRACK_WIDTH/2) * 0.9f);
                m_marble.velocity = m_marble.velocity * -0.5f;
            }
        }
    }
}

void MarbleGame::applyGravity(float deltaTime) {
    if (!m_marble.grounded) {
        m_marble.verticalVel -= GRAVITY * deltaTime * 50.0f;
        m_marble.height += m_marble.verticalVel * deltaTime;

        // Check if landed
        if (m_marble.height <= 0) {
            m_marble.height = 0;
            m_marble.verticalVel = 0;
            m_marble.grounded = true;
            spawnParticles(m_marble.position, TFT_LIGHTGREY, 3);
            m_shakeAmount = 2.0f;
        }
    } else {
        m_marble.height = 0;
    }
}

void MarbleGame::checkCollisions() {
    Vec2 marblePos = angleToPosition(m_marble.angle, TRACK_RADIUS);

    // Check obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!m_obstacles[i].active) continue;

        float angleDiff = fabs(normalizeAngle(m_obstacles[i].angle - m_marble.angle));

        if (angleDiff < 0.3f) { // Within obstacle range
            if (m_marble.grounded || m_obstacles[i].type == OBSTACLE_BARRIER) {
                hitObstacle(m_obstacles[i]);
                m_obstacles[i].active = false;
            }
        }

        // Remove obstacles that are behind marble
        if (normalizeAngle(m_marble.angle - m_obstacles[i].angle) > M_PI) {
            m_obstacles[i].active = false;
        }
    }

    // Check collectibles
    for (int i = 0; i < MAX_COLLECTIBLES; i++) {
        if (m_collectibles[i].collected) continue;

        float angleDiff = fabs(normalizeAngle(m_collectibles[i].angle - m_marble.angle));

        if (angleDiff < 0.2f) {
            collectItem(m_collectibles[i]);
            m_collectibles[i].collected = true;
        }

        // Remove collectibles that are behind marble
        if (normalizeAngle(m_marble.angle - m_collectibles[i].angle) > M_PI) {
            m_collectibles[i].collected = true;
        }
    }
}

void MarbleGame::hitObstacle(const Obstacle& obs) {
    if (m_marble.invincibleTime > 0) {
        // Invincible - destroy obstacle instead
        spawnParticles(angleToPosition(obs.angle, TRACK_RADIUS), TFT_YELLOW, 10);
        m_score += 50;
        return;
    }

    switch (obs.type) {
        case OBSTACLE_HOLE:
            // Fall through hole
            m_marble.alive = false;
            spawnParticles(m_marble.position, TFT_ORANGE, 15);
            break;

        case OBSTACLE_BUMP:
            // Launch into air
            if (m_marble.grounded) {
                m_marble.verticalVel = JUMP_FORCE * 1.5f;
                m_marble.grounded = false;
                spawnParticles(m_marble.position, TFT_CYAN, 8);
                m_score += 10;
            }
            break;

        case OBSTACLE_BARRIER:
            // Hit wall - bounce back
            m_marble.velocity = m_marble.velocity * -1.5f;
            m_shakeAmount = 5.0f;
            spawnParticles(m_marble.position, TFT_RED, 12);
            m_score = max(0, m_score - 20);
            break;

        case OBSTACLE_SPINNER:
            // Spin marble
            Vec2 tangent = m_marble.position.rotated(M_PI / 2);
            m_marble.velocity += tangent * 50.0f;
            spawnParticles(m_marble.position, TFT_MAGENTA, 10);
            break;
    }
}

void MarbleGame::collectItem(const Collectible& col) {
    spawnParticles(angleToPosition(col.angle, TRACK_RADIUS), TFT_YELLOW, 8);

    switch (col.type) {
        case COLLECT_COIN:
            m_score += 100;
            break;

        case COLLECT_SPEEDUP:
            m_marble.speedBoost = 2.0f;
            m_score += 50;
            break;

        case COLLECT_INVINCIBLE:
            m_marble.invincibleTime = 3.0f;
            m_score += 50;
            break;

        case COLLECT_EXTRALIFE:
            m_lives++;
            m_score += 200;
            break;
    }
}

void MarbleGame::spawnObstacle() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!m_obstacles[i].active) {
            m_obstacles[i].active = true;
            m_obstacles[i].angle = normalizeAngle(m_marble.angle + M_PI * 0.8f + (random(-50, 50) / 100.0f));
            m_obstacles[i].size = 15 + random(0, 10);
            m_obstacles[i].rotationAngle = 0;

            // Random obstacle type (weighted)
            int rand = random(0, 100);
            if (rand < 40) {
                m_obstacles[i].type = OBSTACLE_HOLE;
            } else if (rand < 70) {
                m_obstacles[i].type = OBSTACLE_BUMP;
            } else if (rand < 90) {
                m_obstacles[i].type = OBSTACLE_BARRIER;
            } else {
                m_obstacles[i].type = OBSTACLE_SPINNER;
            }

            break;
        }
    }
}

void MarbleGame::spawnCollectible() {
    for (int i = 0; i < MAX_COLLECTIBLES; i++) {
        if (m_collectibles[i].collected) {
            m_collectibles[i].collected = false;
            m_collectibles[i].angle = normalizeAngle(m_marble.angle + M_PI * 0.7f + (random(-30, 30) / 100.0f));
            m_collectibles[i].bobOffset = 0;

            // Random type (weighted)
            int rand = random(0, 100);
            if (rand < 60) {
                m_collectibles[i].type = COLLECT_COIN;
            } else if (rand < 80) {
                m_collectibles[i].type = COLLECT_SPEEDUP;
            } else if (rand < 95) {
                m_collectibles[i].type = COLLECT_INVINCIBLE;
            } else {
                m_collectibles[i].type = COLLECT_EXTRALIFE;
            }

            break;
        }
    }
}

void MarbleGame::resetMarble() {
    m_marble.position = Vec2(TRACK_RADIUS, 0);
    m_marble.velocity = Vec2(0, 0);
    m_marble.height = 0;
    m_marble.verticalVel = 0;
    m_marble.angle = 0;
    m_marble.rotation = 0;
    m_marble.grounded = true;
    m_marble.alive = true;
    m_marble.speedBoost = 1.0f;
    m_marble.invincibleTime = 0;
}

void MarbleGame::nextLevel() {
    m_level++;
    m_score += 500;
    spawnParticles(m_marble.position, TFT_GREEN, 20);
    m_shakeAmount = 3.0f;

    Serial.printf("Level %d reached! Score: %d\n", m_level, m_score);
}

void MarbleGame::spawnParticles(Vec2 position, uint16_t color, int count) {
    for (int i = 0; i < count && i < MAX_PARTICLES; i++) {
        for (int j = 0; j < MAX_PARTICLES; j++) {
            if (!m_particles[j].active) {
                m_particles[j].active = true;
                m_particles[j].position = position;

                float angle = (float)random(0, 628) / 100.0f;
                float speed = random(20, 60);
                m_particles[j].velocity = Vec2(cosf(angle), sinf(angle)) * speed;

                m_particles[j].lifetime = 0;
                m_particles[j].maxLifetime = random(30, 80) / 100.0f;
                m_particles[j].color = color;
                break;
            }
        }
    }
}

void MarbleGame::updateParticles(float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (m_particles[i].active) {
            m_particles[i].lifetime += deltaTime;
            m_particles[i].position += m_particles[i].velocity * deltaTime;
            m_particles[i].velocity *= 0.95f;

            if (m_particles[i].lifetime >= m_particles[i].maxLifetime) {
                m_particles[i].active = false;
            }
        }
    }
}

void MarbleGame::render() {
    m_tft->fillScreen(TFT_BLACK);

    switch (m_state) {
        case STATE_MENU:
            renderMenu();
            break;

        case STATE_PLAYING:
            renderGame();
            renderHUD();
            break;

        case STATE_GAMEOVER:
            renderGame();
            renderGameOver();
            break;

        case STATE_PAUSED:
            renderGame();
            m_tft->setTextColor(TFT_YELLOW, TFT_BLACK);
            m_tft->setTextDatum(MC_DATUM);
            m_tft->drawString("PAUSED", SCREEN_CENTER_X, SCREEN_CENTER_Y, 4);
            break;
    }
}

void MarbleGame::renderMenu() {
    // Title
    m_tft->setTextColor(TFT_CYAN, TFT_BLACK);
    m_tft->setTextDatum(MC_DATUM);
    m_tft->drawString("MARBLE", SCREEN_CENTER_X, 50, 4);
    m_tft->drawString("RUNNER", SCREEN_CENTER_X, 75, 4);

    // Animated marble preview
    float bobAngle = m_gameTime * 2.0f;
    int marbleY = 110 + (int)(sinf(bobAngle) * 5);
    renderMarbleShadow();

    // Instructions
    m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft->setTextDatum(MC_DATUM);
    m_tft->drawString("Press Button", SCREEN_CENTER_X, 135, 2);
    m_tft->drawString("to Start", SCREEN_CENTER_X, 150, 2);
}

void MarbleGame::renderGame() {
    // Apply camera shake
    int shakeX = (int)m_shakeOffset.x;
    int shakeY = (int)m_shakeOffset.y;

    // Render track
    renderTrack();

    // Render obstacles (before marble)
    renderObstacles();

    // Render collectibles
    renderCollectibles();

    // Render marble shadow
    renderMarbleShadow();

    // Render marble
    renderMarble();

    // Render particles
    renderParticles();
}

void MarbleGame::renderHUD() {
    // Score
    m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft->setTextDatum(TL_DATUM);
    m_tft->drawString("Score:", 5, 5, 2);
    m_tft->drawNumber(m_score, 50, 5, 2);

    // Lives
    m_tft->setTextDatum(TR_DATUM);
    for (int i = 0; i < m_lives; i++) {
        m_tft->fillCircle(155 - i * 15, 10, 5, TFT_RED);
    }

    // Level
    m_tft->setTextDatum(TC_DATUM);
    m_tft->drawString("Lv" + String(m_level), SCREEN_CENTER_X, 5, 2);

    // Speed boost indicator
    if (m_buttonPressed) {
        m_tft->fillRect(5, 150, 30, 5, TFT_YELLOW);
    }

    // Invincibility indicator
    if (m_marble.invincibleTime > 0) {
        m_tft->fillCircle(SCREEN_CENTER_X, 150, 5, TFT_CYAN);
    }
}

void MarbleGame::renderGameOver() {
    // Semi-transparent overlay
    m_tft->fillRect(20, 50, 120, 60, TFT_BLACK);
    m_tft->drawRect(20, 50, 120, 60, TFT_RED);

    m_tft->setTextColor(TFT_RED, TFT_BLACK);
    m_tft->setTextDatum(MC_DATUM);
    m_tft->drawString("GAME OVER", SCREEN_CENTER_X, 65, 2);

    m_tft->setTextColor(TFT_WHITE, TFT_BLACK);
    m_tft->drawString("Score: " + String(m_score), SCREEN_CENTER_X, 85, 2);
    m_tft->drawString("Level: " + String(m_level), SCREEN_CENTER_X, 100, 2);
}

void MarbleGame::renderTrack() {
    // Draw circular track with 3D shading
    int segments = 32;
    float angleStep = (M_PI * 2) / segments;

    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep - m_marble.angle * 0.5f; // Slight parallax
        float angle2 = (i + 1) * angleStep - m_marble.angle * 0.5f;

        // Calculate brightness based on angle (simulate lighting)
        float lightAngle = angle1 + M_PI / 4; // Light from top-left
        float brightness = (sinf(lightAngle) + 1.0f) * 0.5f;
        uint8_t shade = 40 + (uint8_t)(brightness * 80);

        uint16_t trackColor = rgb(shade, shade, shade + 20);

        renderTrackSegment(angle1, angle2, trackColor);
    }

    // Inner and outer edges
    m_tft->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, (int)(TRACK_RADIUS + TRACK_WIDTH/2), TFT_DARKGREY);
    m_tft->drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, (int)(TRACK_RADIUS - TRACK_WIDTH/2), TFT_DARKGREY);
}

void MarbleGame::renderTrackSegment(float startAngle, float endAngle, uint16_t color) {
    // Calculate arc positions
    int x1_outer = SCREEN_CENTER_X + (int)(cosf(startAngle) * (TRACK_RADIUS + TRACK_WIDTH/2));
    int y1_outer = SCREEN_CENTER_Y + (int)(sinf(startAngle) * (TRACK_RADIUS + TRACK_WIDTH/2));

    int x2_outer = SCREEN_CENTER_X + (int)(cosf(endAngle) * (TRACK_RADIUS + TRACK_WIDTH/2));
    int y2_outer = SCREEN_CENTER_Y + (int)(sinf(endAngle) * (TRACK_RADIUS + TRACK_WIDTH/2));

    int x1_inner = SCREEN_CENTER_X + (int)(cosf(startAngle) * (TRACK_RADIUS - TRACK_WIDTH/2));
    int y1_inner = SCREEN_CENTER_Y + (int)(sinf(startAngle) * (TRACK_RADIUS - TRACK_WIDTH/2));

    int x2_inner = SCREEN_CENTER_X + (int)(cosf(endAngle) * (TRACK_RADIUS - TRACK_WIDTH/2));
    int y2_inner = SCREEN_CENTER_Y + (int)(sinf(endAngle) * (TRACK_RADIUS - TRACK_WIDTH/2));

    // Draw lines to create track segment
    m_tft->drawLine(x1_outer, y1_outer, x2_outer, y2_outer, color);
    m_tft->drawLine(x1_inner, y1_inner, x2_inner, y2_inner, color);
}

void MarbleGame::renderMarbleShadow() {
    if (!m_marble.alive) return;

    Vec2 screenPos = angleToPosition(m_marble.angle, TRACK_RADIUS);
    int x = SCREEN_CENTER_X + (int)screenPos.x + (int)m_shakeOffset.x;
    int y = SCREEN_CENTER_Y + (int)screenPos.y + (int)m_shakeOffset.y;

    // Shadow gets smaller when marble is higher
    float shadowScale = 1.0f - (m_marble.height / 50.0f);
    shadowScale = max(0.3f, shadowScale);

    int shadowRadius = (int)(MARBLE_RADIUS * shadowScale);

    // Draw soft shadow (multiple circles)
    for (int i = 2; i >= 0; i--) {
        uint8_t alpha = 20 - i * 5;
        m_tft->fillCircle(x + i, y + i, shadowRadius + (2 - i), rgb(alpha, alpha, alpha));
    }
}

void MarbleGame::renderMarble() {
    if (!m_marble.alive) return;

    Vec2 screenPos = angleToPosition(m_marble.angle, TRACK_RADIUS);
    int x = SCREEN_CENTER_X + (int)screenPos.x + (int)m_shakeOffset.x;
    int y = SCREEN_CENTER_Y + (int)screenPos.y - (int)m_marble.height + (int)m_shakeOffset.y;

    // Light direction (from top-left)
    Vec3 lightDir = Vec3(-0.5f, 0.8f, 0.5f).normalized();

    // Render marble with spherical shading
    int radius = (int)MARBLE_RADIUS;

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist <= radius) {
                // Calculate surface normal
                float z = sqrtf(radius * radius - dx * dx - dy * dy);
                Vec3 normal = Vec3(dx, dy, z).normalized();

                // Calculate shading
                uint16_t color = calculateMarbleShading(normal, lightDir);

                // Invincibility effect
                if (m_marble.invincibleTime > 0 && (int)(m_gameTime * 10) % 2 == 0) {
                    color = blendColors(color, TFT_CYAN, 0.5f);
                }

                m_tft->drawPixel(x + dx, y + dy, color);
            }
        }
    }

    // Specular highlight
    int highlightX = x - radius / 3;
    int highlightY = y - radius / 3;
    m_tft->fillCircle(highlightX, highlightY, radius / 4, TFT_WHITE);
}

uint16_t MarbleGame::calculateMarbleShading(Vec3 normal, Vec3 lightDir) {
    // Diffuse lighting
    float diffuse = max(0.0f, normal.dot(lightDir));

    // Marble base colors (swirled pattern)
    float patternValue = sinf(normal.x * 5 + m_marble.rotation) *
                        cosf(normal.y * 5 - m_marble.rotation);
    patternValue = (patternValue + 1.0f) * 0.5f; // 0 to 1

    // Blend between two marble colors
    Vec3 color1(0.8f, 0.3f, 0.2f); // Reddish
    Vec3 color2(0.9f, 0.9f, 0.95f); // White
    Vec3 baseColor = color1 * (1.0f - patternValue) + color2 * patternValue;

    // Apply lighting
    Vec3 finalColor = baseColor * (0.3f + diffuse * 0.7f);

    // Specular highlight
    Vec3 viewDir(0, 0, 1);
    Vec3 halfVector = (lightDir + viewDir).normalized();
    float specular = powf(max(0.0f, normal.dot(halfVector)), 32.0f);

    finalColor = finalColor + Vec3(specular, specular, specular);

    // Clamp and convert to RGB565
    uint8_t r = (uint8_t)(min(1.0f, finalColor.x) * 255);
    uint8_t g = (uint8_t)(min(1.0f, finalColor.y) * 255);
    uint8_t b = (uint8_t)(min(1.0f, finalColor.z) * 255);

    return rgb(r, g, b);
}

void MarbleGame::renderObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (m_obstacles[i].active) {
            renderObstacle(m_obstacles[i]);
        }
    }
}

void MarbleGame::renderObstacle(const Obstacle& obs) {
    Vec2 pos = angleToPosition(obs.angle, TRACK_RADIUS);
    int x = SCREEN_CENTER_X + (int)pos.x + (int)m_shakeOffset.x;
    int y = SCREEN_CENTER_Y + (int)pos.y + (int)m_shakeOffset.y;

    int size = (int)obs.size;

    switch (obs.type) {
        case OBSTACLE_HOLE:
            // Black hole with gradient
            m_tft->fillCircle(x, y, size, TFT_BLACK);
            m_tft->drawCircle(x, y, size, TFT_DARKGREY);
            m_tft->drawCircle(x, y, size - 2, rgb(40, 40, 40));
            break;

        case OBSTACLE_BUMP:
            // Yellow ramp/bump
            m_tft->fillTriangle(x - size, y + size/2, x + size, y + size/2, x, y - size, TFT_YELLOW);
            m_tft->drawTriangle(x - size, y + size/2, x + size, y + size/2, x, y - size, TFT_ORANGE);
            break;

        case OBSTACLE_BARRIER:
            // Red wall
            m_tft->fillRect(x - size/2, y - size, size, size * 2, TFT_RED);
            m_tft->drawRect(x - size/2, y - size, size, size * 2, TFT_MAROON);
            // Highlight
            m_tft->drawLine(x - size/2 + 2, y - size + 2, x - size/2 + 2, y + size - 2, rgb(255, 100, 100));
            break;

        case OBSTACLE_SPINNER:
            // Rotating purple spinner
            for (int i = 0; i < 4; i++) {
                float angle = obs.rotationAngle + (M_PI / 2) * i;
                int x2 = x + (int)(cosf(angle) * size);
                int y2 = y + (int)(sinf(angle) * size);
                m_tft->drawLine(x, y, x2, y2, TFT_MAGENTA);
                m_tft->fillCircle(x2, y2, 3, TFT_PURPLE);
            }
            m_tft->fillCircle(x, y, 4, TFT_DARKGREY);
            break;
    }
}

void MarbleGame::renderCollectibles() {
    for (int i = 0; i < MAX_COLLECTIBLES; i++) {
        if (!m_collectibles[i].collected) {
            renderCollectible(m_collectibles[i]);
        }
    }
}

void MarbleGame::renderCollectible(const Collectible& col) {
    Vec2 pos = angleToPosition(col.angle, TRACK_RADIUS);
    int x = SCREEN_CENTER_X + (int)pos.x + (int)m_shakeOffset.x;
    int y = SCREEN_CENTER_Y + (int)pos.y + (int)col.bobOffset + (int)m_shakeOffset.y;

    uint16_t color = TFT_YELLOW;
    int size = 6;

    switch (col.type) {
        case COLLECT_COIN:
            color = TFT_YELLOW;
            m_tft->fillCircle(x, y, size, color);
            m_tft->drawCircle(x, y, size, TFT_ORANGE);
            m_tft->fillCircle(x - 2, y - 2, 2, rgb(255, 255, 150));
            break;

        case COLLECT_SPEEDUP:
            color = TFT_CYAN;
            // Lightning bolt shape
            m_tft->fillTriangle(x - 4, y - 6, x + 2, y, x - 2, y + 6, color);
            m_tft->fillTriangle(x - 2, y - 2, x + 4, y + 6, x + 2, y, color);
            break;

        case COLLECT_INVINCIBLE:
            color = TFT_BLUE;
            // Star shape
            for (int i = 0; i < 5; i++) {
                float angle = (M_PI * 2 * i / 5) - M_PI / 2;
                int x2 = x + (int)(cosf(angle) * size);
                int y2 = y + (int)(sinf(angle) * size);
                m_tft->fillTriangle(x, y, x2, y2,
                    x + (int)(cosf(angle + M_PI * 2 / 5) * size),
                    y + (int)(sinf(angle + M_PI * 2 / 5) * size), color);
            }
            break;

        case COLLECT_EXTRALIFE:
            color = TFT_GREEN;
            // Heart shape
            m_tft->fillCircle(x - 3, y - 2, 3, color);
            m_tft->fillCircle(x + 3, y - 2, 3, color);
            m_tft->fillTriangle(x - 6, y - 2, x + 6, y - 2, x, y + 6, color);
            break;
    }
}

void MarbleGame::renderParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (m_particles[i].active) {
            float alpha = 1.0f - (m_particles[i].lifetime / m_particles[i].maxLifetime);
            int x = SCREEN_CENTER_X + (int)m_particles[i].position.x + (int)m_shakeOffset.x;
            int y = SCREEN_CENTER_Y + (int)m_particles[i].position.y + (int)m_shakeOffset.y;

            if (alpha > 0.5f) {
                m_tft->fillCircle(x, y, 2, m_particles[i].color);
            } else {
                m_tft->drawPixel(x, y, m_particles[i].color);
            }
        }
    }
}

Vec2 MarbleGame::angleToPosition(float angle, float radius) {
    return Vec2(cosf(angle) * radius, sinf(angle) * radius);
}

float MarbleGame::positionToAngle(Vec2 pos) {
    return atan2f(pos.y, pos.x);
}

float MarbleGame::normalizeAngle(float angle) {
    while (angle > M_PI) angle -= M_PI * 2;
    while (angle < -M_PI) angle += M_PI * 2;
    return angle;
}

uint16_t MarbleGame::rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t MarbleGame::blendColors(uint16_t c1, uint16_t c2, float t) {
    uint8_t r1 = (c1 >> 11) << 3;
    uint8_t g1 = ((c1 >> 5) & 0x3F) << 2;
    uint8_t b1 = (c1 & 0x1F) << 3;

    uint8_t r2 = (c2 >> 11) << 3;
    uint8_t g2 = ((c2 >> 5) & 0x3F) << 2;
    uint8_t b2 = (c2 & 0x1F) << 3;

    uint8_t r = r1 * (1.0f - t) + r2 * t;
    uint8_t g = g1 * (1.0f - t) + g2 * t;
    uint8_t b = b1 * (1.0f - t) + b2 * t;

    return rgb(r, g, b);
}
