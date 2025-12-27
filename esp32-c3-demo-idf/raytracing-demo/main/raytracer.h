/**
 * ESP32-C3 Raytracer
 * Optimized real-time raytracing for round LCD display
 */

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <cmath>

// Rendering configuration
#define RENDER_WIDTH 80      // Internal render resolution (scaled to 160x160)
#define RENDER_HEIGHT 80
#define MAX_SPHERES 5        // Maximum number of spheres in scene
#define MAX_RAY_DEPTH 2      // Maximum reflection bounces
#define SKY_SAMPLES 3        // Samples for cloud noise

// Fast math helpers
#define FAST_SQRT(x) sqrtf(x)
#define FAST_SIN(x) sinf(x)
#define FAST_COS(x) cosf(x)

// Vector3 class for 3D math
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector operations
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float f) const { return Vec3(x * f, y * f, z * f); }
    Vec3 operator/(float f) const { float inv = 1.0f / f; return Vec3(x * inv, y * inv, z * inv); }

    // Compound operations
    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }

    // Vector math
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    float lengthSquared() const { return x * x + y * y + z * z; }
    float length() const { return FAST_SQRT(lengthSquared()); }

    Vec3 normalized() const {
        float len = length();
        if (len < 0.0001f) return Vec3(0, 1, 0);
        return *this / len;
    }

    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Reflection
    Vec3 reflect(const Vec3& normal) const {
        return *this - normal * (2.0f * this->dot(normal));
    }

    // Component-wise multiply
    Vec3 mult(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
};

// Ray structure
struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() {}
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d.normalized()) {}

    Vec3 at(float t) const {
        return origin + direction * t;
    }
};

// Material types
enum MaterialType {
    MATERIAL_METAL,
    MATERIAL_DIFFUSE,
    MATERIAL_CHECKERBOARD
};

// Material structure
struct Material {
    MaterialType type;
    Vec3 albedo;      // Base color
    float roughness;  // Roughness for metals (0 = mirror, 1 = rough)
    float metallic;   // Metallic factor

    Material() : type(MATERIAL_DIFFUSE), albedo(0.8f, 0.8f, 0.8f), roughness(0.5f), metallic(0.0f) {}
    Material(MaterialType t, Vec3 color, float rough = 0.1f, float metal = 1.0f)
        : type(t), albedo(color), roughness(rough), metallic(metal) {}
};

// Sphere object
struct Sphere {
    Vec3 center;
    float radius;
    Material material;

    Sphere() : radius(0) {}
    Sphere(const Vec3& c, float r, const Material& m)
        : center(c), radius(r), material(m) {}
};

// Hit record for ray intersection
struct HitRecord {
    bool hit;
    float t;           // Distance along ray
    Vec3 point;        // Hit point
    Vec3 normal;       // Surface normal
    Material material; // Material at hit point

    HitRecord() : hit(false), t(INFINITY) {}
};

// Scene class containing all objects
class Scene {
public:
    Sphere spheres[MAX_SPHERES];
    int sphereCount;
    float time;  // Animation time

    Scene() : sphereCount(0), time(0) {}

    void addSphere(const Sphere& sphere) {
        if (sphereCount < MAX_SPHERES) {
            spheres[sphereCount++] = sphere;
        }
    }

    void clear() {
        sphereCount = 0;
    }

    // Check ray intersection with all objects
    HitRecord intersect(const Ray& ray, float tMin, float tMax) const;

    // Check intersection with floor plane
    bool intersectFloor(const Ray& ray, HitRecord& rec, float tMin, float tMax) const;

    // Check intersection with sphere
    bool intersectSphere(const Ray& ray, const Sphere& sphere, HitRecord& rec, float tMin, float tMax) const;
};

// Camera class
class Camera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 up;
    float fov;
    float aspectRatio;

    // Cached values
    Vec3 u, v, w;           // Camera basis vectors
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;

    Camera() : position(0, 2, 5), lookAt(0, 0, 0), up(0, 1, 0),
               fov(60.0f), aspectRatio(1.0f) {
        update();
    }

    void update() {
        float theta = fov * M_PI / 180.0f;
        float halfHeight = tan(theta / 2.0f);
        float halfWidth = aspectRatio * halfHeight;

        w = (position - lookAt).normalized();
        u = up.cross(w).normalized();
        v = w.cross(u);

        lowerLeftCorner = position - u * halfWidth - v * halfHeight - w;
        horizontal = u * (2.0f * halfWidth);
        vertical = v * (2.0f * halfHeight);
    }

    Ray getRay(float s, float t) const {
        Vec3 direction = lowerLeftCorner + horizontal * s + vertical * t - position;
        return Ray(position, direction);
    }
};

// Raytracer class
class Raytracer {
public:
    Raytracer(TFT_eSPI* display);

    void begin();
    void render();
    void update(float deltaTime);

private:
    TFT_eSPI* m_tft;
    Scene m_scene;
    Camera m_camera;

    uint16_t* m_framebuffer;  // Framebuffer for rendering
    float m_time;
    int m_frameCount;

    // Render a single pixel
    Vec3 traceRay(const Ray& ray, int depth);

    // Sky color with sun and clouds
    Vec3 skyColor(const Ray& ray);

    // Simple cloud noise
    float cloudNoise(float x, float y);

    // Shading
    Vec3 shade(const Ray& ray, const HitRecord& hit, int depth);

    // Color conversion
    uint16_t vec3ToRgb565(const Vec3& color);

    // Gamma correction
    float gammaCorrect(float value);

    // Setup scene
    void setupScene();
};

#endif // RAYTRACER_H
