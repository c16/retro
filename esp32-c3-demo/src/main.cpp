/**
 * ESP32-C3 Round Display Demo Showcase
 * Over-the-top demo application for 160x160 GC9D01 round LCD
 *
 * Hardware: ESP32-C3 with 0.71" Round LCD (GC9D01)
 * Display: 160x160 IPS, 4-wire SPI
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <vector>
#include <math.h>

// Demo function declarations
void demoAnalogClock();
void demo3DCube();
void demoWiFiScanner();
void demoRadarSweep();
void demoSnakeGame();
void demoPlasma();
void demoParticles();
void demoMatrix();
void demoSpectrum();

// Display instance
TFT_eSPI tft = TFT_eSPI();

// Screen constants
const int16_t CENTER_X = 80;
const int16_t CENTER_Y = 80;
const int16_t RADIUS = 80;

// Demo menu structure
struct Demo {
    const char* name;
    void (*function)();
    uint16_t color;
};

Demo demos[] = {
    {"Analog Clock", demoAnalogClock, TFT_CYAN},
    {"3D Cube", demo3DCube, TFT_MAGENTA},
    {"WiFi Scanner", demoWiFiScanner, TFT_GREEN},
    {"Radar Sweep", demoRadarSweep, TFT_YELLOW},
    {"Snake Game", demoSnakeGame, TFT_RED},
    {"Plasma Effect", demoPlasma, TFT_PURPLE},
    {"Particles", demoParticles, TFT_ORANGE},
    {"Matrix Rain", demoMatrix, TFT_GREENYELLOW},
    {"Spectrum", demoSpectrum, TFT_SKYBLUE}
};

const int numDemos = sizeof(demos) / sizeof(Demo);
int currentDemo = 0;
unsigned long lastInteraction = 0;
const unsigned long AUTO_START_DELAY = 3000; // Auto-start demo after 3 seconds

// Utility functions
void drawCircularText(const char* text, int16_t radius, float angleOffset, uint16_t color) {
    int len = strlen(text);
    float angleStep = 360.0 / len;

    for (int i = 0; i < len; i++) {
        float angle = (angleOffset + i * angleStep) * PI / 180.0;
        int16_t x = CENTER_X + radius * cos(angle);
        int16_t y = CENTER_Y + radius * sin(angle);

        tft.setCursor(x - 3, y - 4);
        tft.setTextColor(color);
        tft.print(text[i]);
    }
}

void drawCircularMenu() {
    tft.fillScreen(TFT_BLACK);

    // Draw outer circle border
    for (int r = RADIUS - 2; r < RADIUS; r++) {
        tft.drawCircle(CENTER_X, CENTER_Y, r, TFT_DARKGREY);
    }

    // Draw demo icons in circular arrangement
    float angleStep = 360.0 / numDemos;

    for (int i = 0; i < numDemos; i++) {
        float angle = (i * angleStep - 90) * PI / 180.0; // -90 to start at top
        int16_t x = CENTER_X + 50 * cos(angle);
        int16_t y = CENTER_Y + 50 * sin(angle);

        uint16_t color = (i == currentDemo) ? demos[i].color : TFT_DARKGREY;
        int size = (i == currentDemo) ? 8 : 5;

        // Draw icon circle
        tft.fillCircle(x, y, size, color);

        if (i == currentDemo) {
            tft.drawCircle(x, y, size + 2, TFT_WHITE);
        }
    }

    // Draw selected demo name in center
    tft.setTextColor(demos[currentDemo].color, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.drawString(demos[currentDemo].name, CENTER_X, CENTER_Y);

    // Draw selection indicator
    float angle = (currentDemo * angleStep - 90) * PI / 180.0;
    int16_t x1 = CENTER_X + 35 * cos(angle);
    int16_t y1 = CENTER_Y + 35 * sin(angle);
    int16_t x2 = CENTER_X + 50 * cos(angle);
    int16_t y2 = CENTER_Y + 50 * sin(angle);
    tft.drawLine(CENTER_X, CENTER_Y, x2, y2, demos[currentDemo].color);

    // Draw rotating border animation
    static float rotAngle = 0;
    rotAngle += 2;
    if (rotAngle >= 360) rotAngle = 0;

    for (int i = 0; i < 12; i++) {
        float a = (rotAngle + i * 30) * PI / 180.0;
        int16_t x = CENTER_X + (RADIUS - 5) * cos(a);
        int16_t y = CENTER_Y + (RADIUS - 5) * sin(a);
        tft.fillCircle(x, y, 2, TFT_WHITE);
    }

    // Instructions
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(BC_DATUM);
    tft.drawString("Touch to cycle", CENTER_X, 150);
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nESP32-C3 Round Display Demo Starting...");

    // Initialize display
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    // Boot animation
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    for (int r = 0; r < RADIUS; r += 5) {
        tft.drawCircle(CENTER_X, CENTER_Y, r, TFT_CYAN);
        delay(30);
    }

    tft.drawString("ESP32-C3", CENTER_X, CENTER_Y - 15);
    tft.setTextSize(1);
    tft.drawString("DEMO SHOWCASE", CENTER_X, CENTER_Y + 10);
    delay(2000);

    // Initialize WiFi for scanning (don't connect)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.printf("Total demos: %d\n", numDemos);
    lastInteraction = millis();
}

void loop() {
    // Draw menu
    drawCircularMenu();

    // Auto-start demo after delay
    if (millis() - lastInteraction > AUTO_START_DELAY) {
        Serial.printf("Starting demo: %s\n", demos[currentDemo].name);
        demos[currentDemo].function();
        lastInteraction = millis();
    }

    // Simple touch detection: could use touchRead() on GPIO pins
    // For now, cycle through demos automatically every 15 seconds
    static unsigned long lastCycle = 0;
    if (millis() - lastCycle > 15000) {
        currentDemo = (currentDemo + 1) % numDemos;
        lastCycle = millis();
        lastInteraction = millis();
    }

    delay(50);
}

// ============================================================================
// DEMO IMPLEMENTATIONS
// ============================================================================

// -----------------------------------------------------------------------------
// Analog Clock Demo
// -----------------------------------------------------------------------------
void demoAnalogClock() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw clock face
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_WHITE);
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 6, TFT_WHITE);

        // Draw hour markers
        for (int i = 0; i < 12; i++) {
            float angle = (i * 30 - 90) * PI / 180.0;
            int16_t x1 = CENTER_X + (RADIUS - 15) * cos(angle);
            int16_t y1 = CENTER_Y + (RADIUS - 15) * sin(angle);
            int16_t x2 = CENTER_X + (RADIUS - 10) * cos(angle);
            int16_t y2 = CENTER_Y + (RADIUS - 10) * sin(angle);
            tft.drawLine(x1, y1, x2, y2, TFT_WHITE);
        }

        // Get current time (simulated smooth animation)
        float seconds = ((millis() - startTime) % 60000) / 1000.0;
        float minutes = ((millis() - startTime) % 3600000) / 60000.0;
        float hours = ((millis() - startTime) % 43200000) / 3600000.0;

        // Draw second hand (red, thin)
        float secondAngle = (seconds * 6 - 90) * PI / 180.0;
        int16_t sx = CENTER_X + (RADIUS - 20) * cos(secondAngle);
        int16_t sy = CENTER_Y + (RADIUS - 20) * sin(secondAngle);
        tft.drawLine(CENTER_X, CENTER_Y, sx, sy, TFT_RED);

        // Draw minute hand (green, medium)
        float minuteAngle = (minutes * 6 - 90) * PI / 180.0;
        int16_t mx = CENTER_X + (RADIUS - 30) * cos(minuteAngle);
        int16_t my = CENTER_Y + (RADIUS - 30) * sin(minuteAngle);
        tft.drawWideLine(CENTER_X, CENTER_Y, mx, my, 2, TFT_GREEN, TFT_BLACK);

        // Draw hour hand (blue, thick)
        float hourAngle = (hours * 30 - 90) * PI / 180.0;
        int16_t hx = CENTER_X + (RADIUS - 45) * cos(hourAngle);
        int16_t hy = CENTER_Y + (RADIUS - 45) * sin(hourAngle);
        tft.drawWideLine(CENTER_X, CENTER_Y, hx, hy, 3, TFT_CYAN, TFT_BLACK);

        // Center dot
        tft.fillCircle(CENTER_X, CENTER_Y, 4, TFT_WHITE);

        delay(50);
    }
}

// -----------------------------------------------------------------------------
// 3D Rotating Cube Demo
// -----------------------------------------------------------------------------
void demo3DCube() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();

    // Cube vertices
    float vertices[8][3] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
        {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
    };

    // Cube edges
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
    };

    float angleX = 0, angleY = 0, angleZ = 0;

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw border circle
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_DARKGREY);

        // Rotate cube
        angleX += 0.03;
        angleY += 0.02;
        angleZ += 0.04;

        // Project and draw edges
        int16_t projected[8][2];

        for (int i = 0; i < 8; i++) {
            // Rotation matrix application
            float x = vertices[i][0];
            float y = vertices[i][1];
            float z = vertices[i][2];

            // Rotate X
            float y1 = y * cos(angleX) - z * sin(angleX);
            float z1 = y * sin(angleX) + z * cos(angleX);

            // Rotate Y
            float x2 = x * cos(angleY) + z1 * sin(angleY);
            float z2 = -x * sin(angleY) + z1 * cos(angleY);

            // Rotate Z
            float x3 = x2 * cos(angleZ) - y1 * sin(angleZ);
            float y3 = x2 * sin(angleZ) + y1 * cos(angleZ);

            // Perspective projection
            float scale = 40 / (z2 + 4);
            projected[i][0] = CENTER_X + x3 * scale;
            projected[i][1] = CENTER_Y + y3 * scale;
        }

        // Draw edges with rainbow colors
        for (int i = 0; i < 12; i++) {
            uint16_t color = tft.color565(
                127 + 127 * sin(angleX + i * 0.5),
                127 + 127 * sin(angleY + i * 0.5 + 2),
                127 + 127 * sin(angleZ + i * 0.5 + 4)
            );

            tft.drawLine(
                projected[edges[i][0]][0], projected[edges[i][0]][1],
                projected[edges[i][1]][0], projected[edges[i][1]][1],
                color
            );
        }

        delay(30);
    }
}

// -----------------------------------------------------------------------------
// WiFi Scanner Demo
// -----------------------------------------------------------------------------
void demoWiFiScanner() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Scanning WiFi...", CENTER_X, CENTER_Y);

    int n = WiFi.scanNetworks();
    Serial.printf("Found %d networks\n", n);

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw circular visualization
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_DARKGREY);

        if (n > 0) {
            float angleStep = 360.0 / n;

            for (int i = 0; i < n && i < 20; i++) { // Limit to 20 networks
                float angle = (i * angleStep - 90 + millis() / 50.0) * PI / 180.0;
                int32_t rssi = WiFi.RSSI(i);

                // Map RSSI to radius (-100 to -30 dBm)
                int radius = map(constrain(rssi, -100, -30), -100, -30, 20, 70);

                // Color based on signal strength
                uint16_t color;
                if (rssi > -50) color = TFT_GREEN;
                else if (rssi > -70) color = TFT_YELLOW;
                else color = TFT_RED;

                int16_t x = CENTER_X + radius * cos(angle);
                int16_t y = CENTER_Y + radius * sin(angle);

                tft.fillCircle(x, y, 4, color);
                tft.drawLine(CENTER_X, CENTER_Y, x, y, color);
            }
        }

        // Center info
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(String(n) + " Networks", CENTER_X, CENTER_Y);

        delay(50);
    }
}

// -----------------------------------------------------------------------------
// Radar Sweep Demo
// -----------------------------------------------------------------------------
void demoRadarSweep() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();
    float angle = 0;

    // Random "targets"
    struct Target {
        float angle;
        int radius;
        int brightness;
    };

    Target targets[10];
    for (int i = 0; i < 10; i++) {
        targets[i].angle = random(0, 360);
        targets[i].radius = random(20, 70);
        targets[i].brightness = 255;
    }

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw radar rings
        for (int r = 20; r < RADIUS; r += 15) {
            tft.drawCircle(CENTER_X, CENTER_Y, r, TFT_DARKGREEN);
        }

        // Draw crosshairs
        tft.drawLine(CENTER_X - RADIUS + 10, CENTER_Y, CENTER_X + RADIUS - 10, CENTER_Y, TFT_DARKGREEN);
        tft.drawLine(CENTER_X, CENTER_Y - RADIUS + 10, CENTER_X, CENTER_Y + RADIUS - 10, TFT_DARKGREEN);

        // Draw targets with fade
        for (int i = 0; i < 10; i++) {
            float ta = targets[i].angle * PI / 180.0;
            int16_t tx = CENTER_X + targets[i].radius * cos(ta);
            int16_t ty = CENTER_Y + targets[i].radius * sin(ta);

            uint8_t bright = targets[i].brightness;
            uint16_t color = tft.color565(0, bright, 0);
            tft.fillCircle(tx, ty, 3, color);

            // Fade targets
            if (targets[i].brightness > 0) {
                targets[i].brightness -= 2;
            }

            // Refresh target if sweep passes over it
            float angleDiff = abs(angle - targets[i].angle);
            if (angleDiff < 5 || angleDiff > 355) {
                targets[i].brightness = 255;
            }
        }

        // Draw sweep line
        float rad = angle * PI / 180.0;
        int16_t x1 = CENTER_X;
        int16_t y1 = CENTER_Y;
        int16_t x2 = CENTER_X + (RADIUS - 10) * cos(rad);
        int16_t y2 = CENTER_Y + (RADIUS - 10) * sin(rad);

        // Sweep with gradient (draw multiple lines with decreasing brightness)
        for (int i = 0; i < 30; i++) {
            float sweepAngle = (angle - i * 2) * PI / 180.0;
            int16_t sx = CENTER_X + (RADIUS - 10) * cos(sweepAngle);
            int16_t sy = CENTER_Y + (RADIUS - 10) * sin(sweepAngle);
            uint8_t bright = 255 - i * 8;
            uint16_t color = tft.color565(0, bright, 0);
            tft.drawLine(CENTER_X, CENTER_Y, sx, sy, color);
        }

        angle += 3;
        if (angle >= 360) angle = 0;

        delay(30);
    }
}

// -----------------------------------------------------------------------------
// Circular Snake Game Demo
// -----------------------------------------------------------------------------
void demoSnakeGame() {
    const int DEMO_DURATION = 15000;
    unsigned long startTime = millis();

    // Snake position in polar coordinates
    struct SnakeSegment {
        float angle;
        int radius;
    };

    std::vector<SnakeSegment> snake;
    snake.push_back({0, 40});

    SnakeSegment food = {random(0, 360), random(25, 70)};
    float direction = 1; // Angular velocity
    int radiusDirection = 0;

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw border
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_DARKGREY);
        tft.drawCircle(CENTER_X, CENTER_Y, 20, TFT_DARKGREY);

        // Simple AI: move towards food
        float angleToFood = food.angle - snake[0].angle;
        if (angleToFood > 180) angleToFood -= 360;
        if (angleToFood < -180) angleToFood += 360;

        direction = (angleToFood > 0) ? 2 : -2;
        radiusDirection = (food.radius > snake[0].radius) ? 1 : -1;

        // Move snake
        if (millis() % 100 < 50) {
            SnakeSegment newHead = snake[0];
            newHead.angle += direction;
            newHead.radius += radiusDirection;

            // Wrap angle
            if (newHead.angle >= 360) newHead.angle -= 360;
            if (newHead.angle < 0) newHead.angle += 360;

            // Constrain radius
            newHead.radius = constrain(newHead.radius, 25, 70);

            snake.insert(snake.begin(), newHead);

            // Check if food eaten
            float dist = sqrt(pow(newHead.angle - food.angle, 2) + pow(newHead.radius - food.radius, 2));
            if (dist < 10) {
                // Grow snake and spawn new food
                food.angle = random(0, 360);
                food.radius = random(25, 70);
            } else {
                // Remove tail if not eating
                if (snake.size() > 15) {
                    snake.pop_back();
                }
            }
        }

        // Draw food
        float fa = food.angle * PI / 180.0;
        int16_t fx = CENTER_X + food.radius * cos(fa);
        int16_t fy = CENTER_Y + food.radius * sin(fa);
        tft.fillCircle(fx, fy, 4, TFT_RED);

        // Draw snake
        for (size_t i = 0; i < snake.size(); i++) {
            float sa = snake[i].angle * PI / 180.0;
            int16_t sx = CENTER_X + snake[i].radius * cos(sa);
            int16_t sy = CENTER_Y + snake[i].radius * sin(sa);

            uint16_t color = (i == 0) ? TFT_YELLOW : TFT_GREEN;
            tft.fillCircle(sx, sy, (i == 0) ? 4 : 3, color);
        }

        // Score
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(String(snake.size()), CENTER_X, CENTER_Y);

        delay(50);
    }
}

// -----------------------------------------------------------------------------
// Plasma Effect Demo
// -----------------------------------------------------------------------------
void demoPlasma() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();
    float phase = 0;

    while (millis() - startTime < DEMO_DURATION) {
        // Draw plasma effect only in circular area
        for (int y = 0; y < 160; y++) {
            for (int x = 0; x < 160; x++) {
                int dx = x - CENTER_X;
                int dy = y - CENTER_Y;
                float dist = sqrt(dx * dx + dy * dy);

                // Only draw within circle
                if (dist < RADIUS) {
                    float angle = atan2(dy, dx);

                    float plasma = sin(dist / 8.0 + phase);
                    plasma += sin(x / 16.0 + phase);
                    plasma += sin(y / 8.0 + phase / 3.0);
                    plasma += sin(angle * 4 + phase);

                    uint8_t r = 127 + 127 * sin(plasma);
                    uint8_t g = 127 + 127 * sin(plasma + PI / 3);
                    uint8_t b = 127 + 127 * sin(plasma + 2 * PI / 3);

                    tft.drawPixel(x, y, tft.color565(r, g, b));
                }
            }
        }

        phase += 0.1;
        delay(30);
    }
}

// -----------------------------------------------------------------------------
// Particle System Demo
// -----------------------------------------------------------------------------
void demoParticles() {
    const int DEMO_DURATION = 10000;
    const int NUM_PARTICLES = 50;
    unsigned long startTime = millis();

    struct Particle {
        float x, y;
        float vx, vy;
        uint16_t color;
        int life;
    };

    Particle particles[NUM_PARTICLES];

    // Initialize particles
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x = CENTER_X;
        particles[i].y = CENTER_Y;
        particles[i].vx = (random(-100, 100)) / 50.0;
        particles[i].vy = (random(-100, 100)) / 50.0;
        particles[i].color = tft.color565(random(100, 255), random(100, 255), random(100, 255));
        particles[i].life = random(50, 150);
    }

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw border
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_DARKGREY);

        // Update and draw particles
        for (int i = 0; i < NUM_PARTICLES; i++) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].life--;

            // Check boundaries (circular)
            float dx = particles[i].x - CENTER_X;
            float dy = particles[i].y - CENTER_Y;
            float dist = sqrt(dx * dx + dy * dy);

            if (dist > RADIUS - 10 || particles[i].life <= 0) {
                // Respawn from center
                particles[i].x = CENTER_X;
                particles[i].y = CENTER_Y;
                particles[i].vx = (random(-100, 100)) / 50.0;
                particles[i].vy = (random(-100, 100)) / 50.0;
                particles[i].color = tft.color565(random(100, 255), random(100, 255), random(100, 255));
                particles[i].life = random(50, 150);
            }

            // Draw particle with trail
            tft.fillCircle(particles[i].x, particles[i].y, 2, particles[i].color);

            // Draw trail
            float tx = particles[i].x - particles[i].vx * 2;
            float ty = particles[i].y - particles[i].vy * 2;
            tft.drawLine(particles[i].x, particles[i].y, tx, ty, particles[i].color);
        }

        delay(30);
    }
}

// -----------------------------------------------------------------------------
// Matrix Rain Demo
// -----------------------------------------------------------------------------
void demoMatrix() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();
    const int NUM_STREAMS = 12;

    struct Stream {
        int x;
        int y;
        int speed;
        int length;
    };

    Stream streams[NUM_STREAMS];
    for (int i = 0; i < NUM_STREAMS; i++) {
        streams[i].x = random(10, 150);
        streams[i].y = random(-50, 0);
        streams[i].speed = random(2, 5);
        streams[i].length = random(10, 30);
    }

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw circular mask
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_GREEN);

        for (int i = 0; i < NUM_STREAMS; i++) {
            streams[i].y += streams[i].speed;

            // Draw stream
            for (int j = 0; j < streams[i].length; j++) {
                int y = streams[i].y - j * 6;
                if (y >= 0 && y < 160) {
                    int dx = streams[i].x - CENTER_X;
                    int dy = y - CENTER_Y;

                    // Only draw within circle
                    if (dx * dx + dy * dy < RADIUS * RADIUS) {
                        uint8_t brightness = 255 - (j * 255 / streams[i].length);
                        uint16_t color = tft.color565(0, brightness, 0);

                        char c = random(33, 126);
                        tft.setCursor(streams[i].x, y);
                        tft.setTextColor(color, TFT_BLACK);
                        tft.setTextSize(1);
                        tft.print(c);
                    }
                }
            }

            // Reset stream if off screen
            if (streams[i].y > 180) {
                streams[i].y = random(-50, -10);
                streams[i].x = random(10, 150);
                streams[i].speed = random(2, 5);
            }
        }

        delay(50);
    }
}

// -----------------------------------------------------------------------------
// Audio Spectrum Analyzer Demo (simulated)
// -----------------------------------------------------------------------------
void demoSpectrum() {
    const int DEMO_DURATION = 10000;
    unsigned long startTime = millis();
    const int NUM_BANDS = 16;
    float bands[NUM_BANDS];

    // Initialize bands
    for (int i = 0; i < NUM_BANDS; i++) {
        bands[i] = random(10, 60);
    }

    while (millis() - startTime < DEMO_DURATION) {
        tft.fillScreen(TFT_BLACK);

        // Draw circular spectrum
        float angleStep = 360.0 / NUM_BANDS;

        for (int i = 0; i < NUM_BANDS; i++) {
            float angle1 = (i * angleStep - 90) * PI / 180.0;
            float angle2 = ((i + 1) * angleStep - 90) * PI / 180.0;

            // Animate band height
            float target = random(10, 60);
            bands[i] += (target - bands[i]) * 0.3;

            int innerRadius = 20;
            int outerRadius = innerRadius + bands[i];

            // Draw band as arc
            for (int r = innerRadius; r < outerRadius; r++) {
                // Color based on height
                uint8_t hue = (bands[i] / 60.0) * 255;
                uint16_t color = tft.color565(255 - hue, hue, 128);

                for (float a = angle1; a < angle2; a += 0.05) {
                    int16_t x = CENTER_X + r * cos(a);
                    int16_t y = CENTER_Y + r * sin(a);
                    tft.drawPixel(x, y, color);
                }
            }
        }

        // Center circle
        tft.fillCircle(CENTER_X, CENTER_Y, 18, TFT_BLACK);
        tft.drawCircle(CENTER_X, CENTER_Y, 20, TFT_WHITE);

        // Outer circle
        tft.drawCircle(CENTER_X, CENTER_Y, RADIUS - 5, TFT_WHITE);

        delay(50);
    }
}
