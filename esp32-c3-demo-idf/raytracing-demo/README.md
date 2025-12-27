# ESP32-C3 Real-Time Raytracing Demo 🌟

**Real-time raytracing on a microcontroller with rotating metal spheres, reflections, and procedural sky**

![Platform](https://img.shields.io/badge/platform-ESP32--C3-green) ![Framework](https://img.shields.io/badge/framework-ESP--IDF-red) ![Display](https://img.shields.io/badge/display-160x160-blue)

## Overview

This project demonstrates real-time raytracing on the ESP32-C3 microcontroller with a 160x160 round LCD display. Despite the limited processing power, the demo achieves impressive visual effects through careful optimization.

### Features

- **Real-time raytracing** at 80x80 resolution (scaled to 160x160)
- **5 rotating metal spheres** with different materials:
  - Silver (mirror-like)
  - Gold
  - Copper
  - Blue metal
  - Green metal
- **Reflective checkered floor** with alternating black and white tiles
- **Procedural sky** with:
  - Blue gradient from horizon to zenith
  - Bright sun with glow effect
  - Animated clouds using noise functions
- **Camera animation** - Smooth rotation and vertical movement
- **Sphere animation** - Orbiting and bobbing spheres
- **Ray reflections** - Up to 2 bounces for realistic metal reflections
- **Gamma correction** - Proper color mapping for display

## Visual Effects

### Materials

**Metal Spheres:**
- Physically-based reflections
- Colored metallic tints
- Fresnel effect for realistic reflectance
- Adjustable roughness

**Checkered Floor:**
- High-contrast black and white pattern
- Diffuse lighting with shadows
- Subtle reflections on white tiles

**Sky:**
- Procedural gradient (horizon to zenith)
- Sun disc with bright core and glow
- Multi-octave cloud noise
- Animated cloud movement

### Animation

- Camera orbits the scene at 6-unit radius
- Gentle vertical camera bobbing
- Center sphere bobs up and down
- Outer spheres orbit in circular patterns
- Smooth 30-degree animation cycles

## Technical Details

### Performance

- **Render Resolution**: 80x80 pixels (6,400 pixels)
- **Display Resolution**: 160x160 pixels (2x scaling)
- **Frame Rate**: ~0.1-0.5 FPS (varies by scene complexity)
- **Ray Depth**: Maximum 2 bounces
- **Memory Usage**: ~35KB RAM (framebuffer + scene data)

### Optimizations

1. **Low Internal Resolution**: Renders at 80x80 then scales to 160x160
2. **Limited Ray Depth**: Only 2 reflection bounces to reduce computation
3. **Fast Math**: Uses `sinf()`, `cosf()`, `sqrtf()` instead of double-precision
4. **Efficient Intersection Tests**: Early-out ray intersection checks
5. **Watchdog Yielding**: Periodic `yield()` calls to prevent timeout
6. **Simple Materials**: Streamlined shading models
7. **Optimized Vector Math**: Inline operations and minimal allocations

### Architecture

```
raytracer.h         - Core raytracing engine (vectors, rays, materials)
raytracer.cpp       - Implementation (intersection, shading, rendering)
main.cpp            - Application entry point and main loop
User_Setup.h        - TFT_eSPI display configuration
```

### Scene Structure

**Camera:**
- Position: Orbits at radius 6, height 3 (±0.5)
- Look-at: Center of scene (0, 0.5, 0)
- FOV: 50 degrees
- Aspect ratio: 1:1 (square)

**Spheres:**
- Center silver: radius 0.7, position (0, 0.7, 0) - bobs
- Gold: radius 0.5, orbits at radius 1.5
- Copper: radius 0.5, orbits at radius 2.0
- Blue: radius 0.4, orbits at radius 1.5
- Green: radius 0.4, orbits at radius 2.0

**Floor:**
- Infinite plane at y = 0
- Normal: (0, 1, 0)
- 0.5-unit checkerboard pattern

**Sky:**
- Sun direction: (0.3, 0.5, -0.5) normalized
- Cloud animation speed: 0.1 units/second

## Hardware Requirements

### Required

- **ESP32-C3** microcontroller
- **0.71" Round LCD** (GC9D01 driver, 160x160 pixels, SPI interface)
- **GPIO Connections**:
  - GPIO7: TFT_MOSI
  - GPIO6: TFT_SCLK
  - GPIO5: TFT_CS
  - GPIO4: TFT_DC
  - GPIO8: TFT_RST

### Optional

- USB cable for serial monitoring
- Power supply (3.3V, >300mA recommended)

## Quick Start

### 1. Prerequisites

- ESP-IDF v5.0+ with VS Code extension
- TFT_eSPI library (cloned manually)

### 2. Clone TFT_eSPI Library

The project uses TFT_eSPI in a shared components directory:

```bash
cd esp32-c3-demo-idf/components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../raytracing-demo/main/User_Setup.h tft_espi/User_Setup.h
```

### 3. Build with VS Code

**Set Target:**
- Press F1 → "ESP-IDF: Set Espressif device target" → `esp32c3`

**Build:**
- Click 🔨 Build button

**Flash:**
- Click ⚡ Flash button

**Monitor:**
- Click 🖥️ Monitor button

### 4. Build with Command Line

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Navigate to project
cd esp32-c3-demo-idf/raytracing-demo

# Configure
idf.py set-target esp32c3

# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor
```

## Understanding the Code

### Ray-Sphere Intersection

The raytracer uses the quadratic formula to find ray-sphere intersections:

```cpp
Vec3 oc = ray.origin - sphere.center;
float a = ray.direction.dot(ray.direction);
float b = 2.0f * oc.dot(ray.direction);
float c = oc.dot(oc) - sphere.radius * sphere.radius;
float discriminant = b * b - 4 * a * c;
```

If discriminant ≥ 0, the ray hits the sphere.

### Metal Reflection

Reflections use the standard reflection formula:

```cpp
Vec3 reflected = incident.reflect(normal);
// where: reflect = incident - normal * (2 * dot(incident, normal))
```

### Sky with Clouds

Clouds use multi-octave noise:

```cpp
for (int i = 0; i < 3; i++) {
    float fx = x * (1 << i);  // Frequency doubling
    float fy = y * (1 << i);
    value += hash(fx, fy) * amplitude;
    amplitude *= 0.5f;        // Amplitude halving
}
```

### Color Conversion

RGB float values are converted to RGB565 format:

```cpp
uint8_t r5 = (uint8_t)(clamp(r) * 31);
uint8_t g6 = (uint8_t)(clamp(g) * 63);
uint8_t b5 = (uint8_t)(clamp(b) * 31);
uint16_t rgb565 = (r5 << 11) | (g6 << 5) | b5;
```

## Customization

### Change Scene Layout

Edit `raytracer.cpp`, function `setupScene()`:

```cpp
// Add more spheres
m_scene.addSphere(Sphere(
    Vec3(x, y, z),           // Position
    radius,                  // Size
    Material(MATERIAL_METAL, Vec3(r, g, b), roughness, metallic)
));
```

### Adjust Camera

Edit `raytracer.cpp`, function `update()`:

```cpp
// Change orbit radius
float radius = 8.0f;  // Further away

// Change rotation speed
float angle = m_time * 0.5f;  // Faster rotation

// Change height
m_camera.position.y = 5.0f;  // Higher viewpoint
```

### Modify Sky

Edit `raytracer.cpp`, function `skyColor()`:

```cpp
// Change sky color
Vec3 skyGradient = Vec3(0.3f, 0.5f, 0.8f);  // Darker blue

// Move sun
Vec3 sunDir = Vec3(0.5f, 0.3f, -0.5f).normalized();

// Adjust sun size
if (sunDot > 0.995f) {  // Larger sun
```

### Change Resolution

Edit `raytracer.h`:

```cpp
// Trade quality for speed
#define RENDER_WIDTH 60
#define RENDER_HEIGHT 60

// Or trade speed for quality
#define RENDER_WIDTH 100
#define RENDER_HEIGHT 100
```

**Note**: Higher resolutions will significantly reduce frame rate.

### Adjust Ray Depth

Edit `raytracer.h`:

```cpp
// Fewer bounces = faster but less realistic
#define MAX_RAY_DEPTH 1

// More bounces = slower but more realistic
#define MAX_RAY_DEPTH 3
```

## Troubleshooting

### Display Issues

**Black screen:**
- Check GPIO connections match User_Setup.h
- Press RESET button on ESP32-C3
- Verify 3.3V power supply

**Corrupted graphics:**
- Lower SPI frequency in User_Setup.h
- Check wiring for loose connections
- Ensure stable power supply

### Performance Issues

**Very slow rendering:**
- This is expected! Raytracing is computationally expensive
- Each frame takes 2-10 seconds on ESP32-C3
- Try reducing resolution (RENDER_WIDTH/HEIGHT)
- Reduce MAX_RAY_DEPTH to 1

**Watchdog timeout:**
- The code includes `yield()` calls to prevent this
- If it still occurs, add more `yield()` in render loop

**Out of memory:**
- Reduce MAX_SPHERES
- Lower RENDER_WIDTH and RENDER_HEIGHT
- Check free heap: `Serial.println(ESP.getFreeHeap())`

### Build Errors

**TFT_eSPI not found:**
```bash
cd components
git clone https://github.com/Bodmer/TFT_eSPI.git tft_espi
cp ../raytracing-demo/main/User_Setup.h tft_espi/
```

**Math errors (sqrt, sin, cos undefined):**
- Make sure `#include <cmath>` is in raytracer.h
- Check compiler flags include `-lm`

## Algorithm Details

### Raytracing Pipeline

1. **For each pixel:**
   - Calculate screen UV coordinates
   - Generate camera ray through pixel
   - Trace ray into scene

2. **Ray tracing:**
   - Test intersection with floor plane
   - Test intersection with all spheres
   - Find closest intersection

3. **Shading:**
   - **If metal**: Calculate reflection, trace reflected ray
   - **If floor**: Apply checkerboard pattern, add lighting
   - **If sky**: Calculate sky gradient, add sun, add clouds

4. **Color output:**
   - Apply gamma correction
   - Convert to RGB565
   - Store in framebuffer

5. **Display:**
   - Scale 80x80 framebuffer to 160x160
   - Transfer to display via SPI

### Vector Math Optimization

- All vectors use 32-bit floats (not doubles)
- Dot products avoid intermediate objects
- Normalized vectors cached where possible
- Length calculations use squared length when possible

## Performance Characteristics

### Timing Breakdown (Approximate)

- Ray generation: ~5%
- Intersection tests: ~40%
- Shading calculations: ~35%
- Sky/cloud rendering: ~10%
- Display transfer: ~10%

### Bottlenecks

1. **Ray-sphere intersection** (quadratic solve with sqrt)
2. **Sky gradient calculation** (trigonometric functions)
3. **Cloud noise** (multiple sin/hash calculations)
4. **SPI display transfer** (6,400 pixels × 2 bytes)

## Future Enhancements

Possible improvements:

- [ ] Use integer/fixed-point math for faster calculations
- [ ] Implement bounding volume hierarchy (BVH) for faster intersection
- [ ] Add temporal reprojection (render alternate pixels per frame)
- [ ] Implement jittered sampling for anti-aliasing
- [ ] Add directional light with shadows
- [ ] Support for additional primitive types (planes, boxes)
- [ ] Interactive camera control via serial commands
- [ ] Depth of field effect
- [ ] Motion blur
- [ ] ESP32-S3 version with dual-core rendering

## Credits

- **Raytracing Algorithm**: Based on Peter Shirley's "Ray Tracing in One Weekend"
- **Display Library**: TFT_eSPI by Bodmer
- **Framework**: ESP-IDF by Espressif
- **Hardware**: ESP32-C3 with GC9D01 round display
- **Created with**: Claude Code

## License

This project is provided as-is for educational purposes.

## References

- [Ray Tracing in One Weekend](https://raytracing.github.io/)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)

---

**Experience real-time raytracing on a tiny microcontroller!** ✨🔮

