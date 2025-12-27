/**
 * SYNESTHESIA - Generative Art & Music System
 * The creative project I always wanted to build
 *
 * A self-evolving audio-visual experience that generates unique
 * art and music in real-time, never repeating the same pattern twice.
 */

#ifndef SYNESTHESIA_H
#define SYNESTHESIA_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <cmath>

// Hardware configuration
#define BOOT_BUTTON 9
#define AUDIO_PIN 2

// Display configuration
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 160
#define SCREEN_CENTER_X 80
#define SCREEN_CENTER_Y 80

// Audio configuration
#define SAMPLE_RATE 22050
#define AUDIO_BUFFER_SIZE 64
#define MAX_VOICES 4
#define PWM_FREQ 22050
#define PWM_RESOLUTION 8

// Visual configuration
#define MAX_PARTICLES 80
#define FLOW_FIELD_RES 16
#define MAX_AUTOMATA_CELLS 256

// Musical scales and modes
enum Scale {
    SCALE_MAJOR,
    SCALE_MINOR,
    SCALE_PENTATONIC,
    SCALE_BLUES,
    SCALE_DORIAN,
    SCALE_PHRYGIAN,
    SCALE_MIXOLYDIAN,
    SCALE_CHROMATIC
};

// Artistic modes
enum ArtMode {
    ART_FLOW_FIELD,      // Flowing particle trails
    ART_FRACTAL,         // Animated fractals
    ART_AUTOMATA,        // Cellular automata
    ART_WAVES,           // Wave interference patterns
    ART_MANDALA,         // Rotating symmetric patterns
    ART_PLASMA,          // Plasma/fire effects
    ART_GALAXY,          // Spiral galaxy simulation
    ART_MEDITATION       // Breathing circle with aurora
};

// Moods (combine visual + audio characteristics)
enum Mood {
    MOOD_CALM,           // Slow, peaceful, blues/greens
    MOOD_ENERGETIC,      // Fast, vibrant, warm colors
    MOOD_MYSTERIOUS,     // Slow, complex, purples/deep colors
    MOOD_JOYFUL,         // Bright, major scale, yellows/pinks
    MOOD_MELANCHOLIC,    // Minor scale, cool colors
    MOOD_TRANSCENDENT,   // Ethereal, high frequency, whites/golds
    MOOD_CHAOTIC,        // Fast changes, dissonant, multi-color
    MOOD_MEDITATIVE      // Very slow, minimal, deep colors
};

// Waveform types for synthesis
enum Waveform {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAW,
    WAVE_NOISE
};

// Vector2 for particles and flow fields
struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(float f) const { return Vec2(x * f, y * f); }

    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const {
        float len = length();
        return len > 0 ? Vec2(x / len, y / len) : Vec2(0, 0);
    }

    Vec2 rotated(float angle) const {
        float c = cosf(angle);
        float s = sinf(angle);
        return Vec2(x * c - y * s, x * s + y * c);
    }
};

// Color with HSV support
struct Color {
    uint16_t rgb565;

    Color() : rgb565(0) {}
    Color(uint16_t rgb) : rgb565(rgb) {}

    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b) {
        return Color(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    }

    static Color fromHSV(float h, float s, float v);

    Color blend(const Color& other, float t) const;
};

// Particle for flow field and other effects
struct Particle {
    Vec2 pos;
    Vec2 vel;
    Color color;
    float life;
    float maxLife;
    float size;
    bool active;

    Particle() : pos(0, 0), vel(0, 0), life(0), maxLife(1.0f),
                 size(1.0f), active(false) {}
};

// Voice for audio synthesis
struct Voice {
    float frequency;
    float phase;
    float amplitude;
    float envelope;
    Waveform waveform;
    bool active;

    // ADSR envelope
    float attack;
    float decay;
    float sustain;
    float release;
    float envTime;

    Voice() : frequency(440), phase(0), amplitude(0), envelope(0),
              waveform(WAVE_SINE), active(false),
              attack(0.05f), decay(0.1f), sustain(0.7f), release(0.3f),
              envTime(0) {}
};

// Note in a sequence
struct Note {
    int pitch;          // MIDI note number
    float duration;     // In beats
    float velocity;     // 0-1

    Note() : pitch(60), duration(1.0f), velocity(0.8f) {}
    Note(int p, float d, float v = 0.8f) : pitch(p), duration(d), velocity(v) {}
};

// Audio engine class
class AudioEngine {
public:
    AudioEngine();

    void begin();
    void update(float deltaTime);
    void playNote(int midiNote, float duration, float velocity = 0.8f, Waveform wave = WAVE_SINE);
    void stopAll();

    void setScale(Scale scale);
    void setTempo(float bpm);
    void setMasterVolume(float volume);

    Scale getCurrentScale() const { return m_scale; }

private:
    Voice m_voices[MAX_VOICES];
    Scale m_scale;
    float m_tempo;
    float m_masterVolume;

    hw_timer_t* m_timer;

    float generateSample(const Voice& voice);
    float midiToFreq(int midiNote);
    void updateVoiceEnvelope(Voice& voice, float deltaTime);

    static void IRAM_ATTR onTimer();
    static AudioEngine* s_instance;
};

// Music composer class
class MusicComposer {
public:
    MusicComposer(AudioEngine* audio);

    void begin();
    void update(float deltaTime);
    void setMood(Mood mood);
    void generate();

private:
    AudioEngine* m_audio;
    Mood m_mood;

    float m_tempo;
    Scale m_scale;
    int m_rootNote;

    float m_beatTime;
    int m_currentBeat;
    int m_currentBar;

    // Generative parameters
    float m_complexity;
    float m_dissonance;
    float m_density;

    // Current sequence
    Note m_sequence[16];
    int m_sequenceLength;

    void generateMelody();
    void generateChord();
    int quantizeToScale(int note);
    int getScaleNote(int degree);
};

// Visual generator base class
class VisualGenerator {
public:
    virtual ~VisualGenerator() {}

    virtual void begin() = 0;
    virtual void update(float deltaTime, float audioLevel, float beatPhase) = 0;
    virtual void render(TFT_eSPI* tft) = 0;
    virtual void setMood(Mood mood) = 0;
};

// Flow field generator
class FlowFieldGenerator : public VisualGenerator {
public:
    FlowFieldGenerator();

    void begin() override;
    void update(float deltaTime, float audioLevel, float beatPhase) override;
    void render(TFT_eSPI* tft) override;
    void setMood(Mood mood) override;

private:
    Particle m_particles[MAX_PARTICLES];
    Vec2 m_flowField[FLOW_FIELD_RES][FLOW_FIELD_RES];

    Mood m_mood;
    float m_noiseScale;
    float m_noiseTime;
    float m_flowStrength;

    Vec2 getFlowAt(Vec2 pos);
    float noise2D(float x, float y);
    void updateFlowField(float deltaTime);
};

// Fractal generator
class FractalGenerator : public VisualGenerator {
public:
    FractalGenerator();

    void begin() override;
    void update(float deltaTime, float audioLevel, float beatPhase) override;
    void render(TFT_eSPI* tft) override;
    void setMood(Mood mood) override;

private:
    Mood m_mood;
    float m_zoom;
    float m_centerX;
    float m_centerY;
    float m_rotation;
    int m_maxIterations;

    Color getIterationColor(int iter, float smooth);
    int mandelbrot(float x0, float y0, float& smoothValue);
};

// Cellular automata generator
class AutomataGenerator : public VisualGenerator {
public:
    AutomataGenerator();

    void begin() override;
    void update(float deltaTime, float audioLevel, float beatPhase) override;
    void render(TFT_eSPI* tft) override;
    void setMood(Mood mood) override;

private:
    uint8_t m_cells[MAX_AUTOMATA_CELLS];
    uint8_t m_nextCells[MAX_AUTOMATA_CELLS];

    Mood m_mood;
    int m_rule;
    float m_updateTimer;
    float m_updateInterval;
    int m_generation;

    void randomize();
    void applyRule();
};

// Meditation mode (breathing circle with aurora)
class MeditationGenerator : public VisualGenerator {
public:
    MeditationGenerator();

    void begin() override;
    void update(float deltaTime, float audioLevel, float beatPhase) override;
    void render(TFT_eSPI* tft) override;
    void setMood(Mood mood) override;

private:
    Mood m_mood;
    float m_breathPhase;
    float m_breathRate;
    float m_auroraPhase;

    void renderBreathingCircle(TFT_eSPI* tft, float radius, float alpha);
    void renderAurora(TFT_eSPI* tft);
};

// Main Synesthesia engine
class Synesthesia {
public:
    Synesthesia(TFT_eSPI* display);
    ~Synesthesia();

    void begin();
    void update(float deltaTime);
    void render();
    void handleInput();

    void cycleMood();
    void cycleArtMode();
    void evolve();

    Mood getCurrentMood() const { return m_mood; }
    ArtMode getCurrentArtMode() const { return m_artMode; }

private:
    TFT_eSPI* m_tft;

    AudioEngine* m_audio;
    MusicComposer* m_composer;
    VisualGenerator* m_visualGenerator;

    Mood m_mood;
    ArtMode m_artMode;

    float m_time;
    float m_audioLevel;
    float m_beatPhase;

    // Evolution parameters
    float m_evolutionRate;
    float m_evolutionTimer;

    // Input
    bool m_buttonPressed;
    bool m_lastButtonState;
    unsigned long m_buttonPressTime;

    void createVisualGenerator();
    void updateEvolution(float deltaTime);
    const char* getMoodName(Mood mood);
    const char* getArtModeName(ArtMode mode);
};

#endif // SYNESTHESIA_H
