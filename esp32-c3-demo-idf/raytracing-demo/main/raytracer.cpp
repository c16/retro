/**
 * ESP32-C3 Raytracer Implementation
 */

#include "raytracer.h"

Raytracer::Raytracer(TFT_eSPI* display)
    : m_tft(display)
    , m_framebuffer(nullptr)
    , m_time(0)
    , m_frameCount(0)
{
}

void Raytracer::begin() {
    // Allocate framebuffer
    m_framebuffer = (uint16_t*)malloc(RENDER_WIDTH * RENDER_HEIGHT * sizeof(uint16_t));
    if (!m_framebuffer) {
        Serial.println("ERROR: Failed to allocate framebuffer!");
        return;
    }

    Serial.println("Raytracer initialized");
    Serial.printf("Render resolution: %dx%d\n", RENDER_WIDTH, RENDER_HEIGHT);
    Serial.printf("Display resolution: %dx%d\n", m_tft->width(), m_tft->height());

    setupScene();
}

void Raytracer::setupScene() {
    m_scene.clear();

    // Add multiple metal spheres with different colors
    // Center sphere - silver
    m_scene.addSphere(Sphere(
        Vec3(0, 0.7f, 0),
        0.7f,
        Material(MATERIAL_METAL, Vec3(0.95f, 0.95f, 0.95f), 0.05f, 1.0f)
    ));

    // Left sphere - gold
    m_scene.addSphere(Sphere(
        Vec3(-2.0f, 0.5f, -1.0f),
        0.5f,
        Material(MATERIAL_METAL, Vec3(1.0f, 0.84f, 0.0f), 0.1f, 1.0f)
    ));

    // Right sphere - copper
    m_scene.addSphere(Sphere(
        Vec3(2.0f, 0.5f, 1.0f),
        0.5f,
        Material(MATERIAL_METAL, Vec3(0.95f, 0.64f, 0.54f), 0.1f, 1.0f)
    ));

    // Back left - blue metal
    m_scene.addSphere(Sphere(
        Vec3(-1.5f, 0.4f, -3.0f),
        0.4f,
        Material(MATERIAL_METAL, Vec3(0.3f, 0.5f, 1.0f), 0.15f, 1.0f)
    ));

    // Back right - green metal
    m_scene.addSphere(Sphere(
        Vec3(1.5f, 0.4f, -2.5f),
        0.4f,
        Material(MATERIAL_METAL, Vec3(0.3f, 0.95f, 0.3f), 0.15f, 1.0f)
    ));

    // Setup camera
    m_camera.position = Vec3(4, 3, 6);
    m_camera.lookAt = Vec3(0, 0.5f, 0);
    m_camera.up = Vec3(0, 1, 0);
    m_camera.fov = 50.0f;
    m_camera.aspectRatio = 1.0f;
    m_camera.update();
}

void Raytracer::update(float deltaTime) {
    m_time += deltaTime;

    // Rotate camera around scene
    float angle = m_time * 0.2f;  // Slow rotation
    float radius = 6.0f;
    m_camera.position = Vec3(
        FAST_COS(angle) * radius,
        3.0f + FAST_SIN(m_time * 0.5f) * 0.5f,  // Gentle up/down movement
        FAST_SIN(angle) * radius
    );
    m_camera.update();

    // Rotate spheres
    for (int i = 0; i < m_scene.sphereCount; i++) {
        // Store initial positions and rotate them
        float baseAngle = m_time * 0.5f + i * (M_PI * 2.0f / m_scene.sphereCount);

        // Keep vertical position, rotate horizontally
        float y = m_scene.spheres[i].center.y;

        if (i == 0) {
            // Center sphere stays but bobs up and down
            m_scene.spheres[i].center.y = 0.7f + FAST_SIN(m_time * 2.0f) * 0.2f;
        } else {
            // Other spheres orbit
            float orbitRadius = 1.5f + (i % 2) * 0.5f;
            m_scene.spheres[i].center.x = FAST_COS(baseAngle) * orbitRadius;
            m_scene.spheres[i].center.z = FAST_SIN(baseAngle) * orbitRadius;
        }
    }

    m_scene.time = m_time;
}

void Raytracer::render() {
    unsigned long startTime = millis();

    // Render to framebuffer
    for (int y = 0; y < RENDER_HEIGHT; y++) {
        for (int x = 0; x < RENDER_WIDTH; x++) {
            // Calculate UV coordinates
            float u = (float)x / (RENDER_WIDTH - 1);
            float v = 1.0f - ((float)y / (RENDER_HEIGHT - 1));  // Flip Y

            // Generate ray
            Ray ray = m_camera.getRay(u, v);

            // Trace ray
            Vec3 color = traceRay(ray, 0);

            // Store in framebuffer
            m_framebuffer[y * RENDER_WIDTH + x] = vec3ToRgb565(color);
        }

        // Yield to prevent watchdog timeout
        if (y % 10 == 0) {
            yield();
        }
    }

    // Scale and draw framebuffer to display
    int scale = m_tft->width() / RENDER_WIDTH;
    for (int y = 0; y < RENDER_HEIGHT; y++) {
        for (int x = 0; x < RENDER_WIDTH; x++) {
            uint16_t color = m_framebuffer[y * RENDER_WIDTH + x];

            // Draw scaled pixel block
            if (scale > 1) {
                m_tft->fillRect(x * scale, y * scale, scale, scale, color);
            } else {
                m_tft->drawPixel(x, y, color);
            }
        }

        // Yield periodically
        if (y % 5 == 0) {
            yield();
        }
    }

    unsigned long renderTime = millis() - startTime;
    m_frameCount++;

    // Print stats
    if (m_frameCount % 10 == 0) {
        Serial.printf("Frame %d - Render time: %lums (%.1f FPS)\n",
                     m_frameCount, renderTime,
                     renderTime > 0 ? 1000.0f / renderTime : 0.0f);
    }
}

Vec3 Raytracer::traceRay(const Ray& ray, int depth) {
    if (depth >= MAX_RAY_DEPTH) {
        return Vec3(0, 0, 0);
    }

    // Check intersection with scene
    HitRecord rec = m_scene.intersect(ray, 0.001f, INFINITY);

    if (rec.hit) {
        return shade(ray, rec, depth);
    }

    // No hit - return sky color
    return skyColor(ray);
}

Vec3 Raytracer::skyColor(const Ray& ray) {
    Vec3 unitDirection = ray.direction.normalized();

    // Sky gradient (blue at horizon, lighter at top)
    float t = 0.5f * (unitDirection.y + 1.0f);
    Vec3 skyGradient = Vec3(0.5f, 0.7f, 1.0f) * (1.0f - t * 0.3f);

    // Sun
    Vec3 sunDir = Vec3(0.3f, 0.5f, -0.5f).normalized();
    float sunDot = unitDirection.dot(sunDir);

    if (sunDot > 0.998f) {
        // Sun core (very bright)
        return Vec3(3.0f, 2.5f, 2.0f);
    } else if (sunDot > 0.99f) {
        // Sun glow
        float glow = (sunDot - 0.99f) / 0.008f;
        return skyGradient + Vec3(2.0f, 1.5f, 1.0f) * glow;
    }

    // Clouds
    if (unitDirection.y > 0.0f) {
        float cloudFactor = cloudNoise(unitDirection.x * 5.0f + m_time * 0.1f,
                                       unitDirection.z * 5.0f);
        cloudFactor = cloudFactor * cloudFactor;  // Square for more contrast
        cloudFactor *= unitDirection.y;  // Fade near horizon

        Vec3 cloudColor = Vec3(1.0f, 1.0f, 1.0f);
        skyGradient = skyGradient * (1.0f - cloudFactor * 0.7f) + cloudColor * (cloudFactor * 0.7f);
    }

    return skyGradient;
}

float Raytracer::cloudNoise(float x, float y) {
    // Simple multi-octave noise
    float value = 0;
    float amp = 0.5f;

    for (int i = 0; i < SKY_SAMPLES; i++) {
        float fx = x * (1 << i);
        float fy = y * (1 << i);

        // Simple hash-based noise
        float s = FAST_SIN(fx * 12.9898f + fy * 78.233f) * 43758.5453f;
        value += (s - floor(s)) * amp;

        amp *= 0.5f;
    }

    return value;
}

Vec3 Raytracer::shade(const Ray& ray, const HitRecord& hit, int depth) {
    Vec3 color(0, 0, 0);

    if (hit.material.type == MATERIAL_METAL) {
        // Metal shading with reflection
        Vec3 reflected = ray.direction.reflect(hit.normal);

        // Add slight roughness perturbation (simplified)
        Vec3 roughVec = hit.normal * hit.material.roughness * 0.1f;
        reflected = (reflected + roughVec).normalized();

        // Trace reflection ray
        Ray reflectRay(hit.point, reflected);
        Vec3 reflectColor = traceRay(reflectRay, depth + 1);

        // Mix with albedo (metal tint)
        color = reflectColor.mult(hit.material.albedo);

        // Add Fresnel effect (simplified)
        float fresnel = 1.0f - max(0.0f, -ray.direction.dot(hit.normal));
        fresnel = fresnel * fresnel;
        color = color * (0.5f + fresnel * 0.5f);

    } else if (hit.material.type == MATERIAL_CHECKERBOARD) {
        // Checkerboard pattern for floor
        float scale = 0.5f;
        int xi = (int)floor(hit.point.x / scale);
        int zi = (int)floor(hit.point.z / scale);
        bool isWhite = ((xi + zi) % 2) == 0;

        Vec3 baseColor = isWhite ? Vec3(0.9f, 0.9f, 0.9f) : Vec3(0.2f, 0.2f, 0.2f);

        // Simple diffuse lighting
        Vec3 lightDir = Vec3(0.3f, 0.7f, -0.5f).normalized();
        float diffuse = max(0.0f, hit.normal.dot(lightDir));

        // Ambient + diffuse
        color = baseColor * (0.3f + diffuse * 0.7f);

        // Add slight reflection for white tiles
        if (isWhite && depth < MAX_RAY_DEPTH - 1) {
            Vec3 reflected = ray.direction.reflect(hit.normal);
            Ray reflectRay(hit.point, reflected);
            Vec3 reflectColor = traceRay(reflectRay, depth + 1);
            color = color * 0.7f + reflectColor * 0.3f;
        }
    } else {
        // Diffuse material
        Vec3 lightDir = Vec3(0.3f, 0.7f, -0.5f).normalized();
        float diffuse = max(0.0f, hit.normal.dot(lightDir));
        color = hit.material.albedo * (0.2f + diffuse * 0.8f);
    }

    return color;
}

HitRecord Scene::intersect(const Ray& ray, float tMin, float tMax) const {
    HitRecord rec;
    rec.hit = false;
    rec.t = tMax;

    // Check floor
    HitRecord floorRec;
    if (intersectFloor(ray, floorRec, tMin, rec.t)) {
        rec = floorRec;
    }

    // Check all spheres
    for (int i = 0; i < sphereCount; i++) {
        HitRecord sphereRec;
        if (intersectSphere(ray, spheres[i], sphereRec, tMin, rec.t)) {
            rec = sphereRec;
        }
    }

    return rec;
}

bool Scene::intersectFloor(const Ray& ray, HitRecord& rec, float tMin, float tMax) const {
    // Floor plane at y = 0, normal = (0, 1, 0)
    float denom = ray.direction.y;

    if (fabs(denom) < 0.0001f) {
        return false;  // Ray parallel to floor
    }

    float t = -ray.origin.y / denom;

    if (t < tMin || t > tMax) {
        return false;
    }

    rec.hit = true;
    rec.t = t;
    rec.point = ray.at(t);
    rec.normal = Vec3(0, 1, 0);
    rec.material = Material(MATERIAL_CHECKERBOARD, Vec3(1, 1, 1), 0, 0);

    return true;
}

bool Scene::intersectSphere(const Ray& ray, const Sphere& sphere, HitRecord& rec, float tMin, float tMax) const {
    Vec3 oc = ray.origin - sphere.center;
    float a = ray.direction.dot(ray.direction);
    float b = 2.0f * oc.dot(ray.direction);
    float c = oc.dot(oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtD = FAST_SQRT(discriminant);
    float t = (-b - sqrtD) / (2.0f * a);

    if (t < tMin || t > tMax) {
        t = (-b + sqrtD) / (2.0f * a);
        if (t < tMin || t > tMax) {
            return false;
        }
    }

    rec.hit = true;
    rec.t = t;
    rec.point = ray.at(t);
    rec.normal = (rec.point - sphere.center) / sphere.radius;
    rec.material = sphere.material;

    return true;
}

uint16_t Raytracer::vec3ToRgb565(const Vec3& color) {
    // Gamma correction
    float r = gammaCorrect(color.x);
    float g = gammaCorrect(color.y);
    float b = gammaCorrect(color.z);

    // Clamp and convert to RGB565
    uint8_t r8 = (uint8_t)(min(1.0f, r) * 31);
    uint8_t g8 = (uint8_t)(min(1.0f, g) * 63);
    uint8_t b8 = (uint8_t)(min(1.0f, b) * 31);

    return (r8 << 11) | (g8 << 5) | b8;
}

float Raytracer::gammaCorrect(float value) {
    if (value <= 0.0f) return 0.0f;
    // Simple gamma correction (gamma = 2.2)
    return pow(value, 1.0f / 2.2f);
}
