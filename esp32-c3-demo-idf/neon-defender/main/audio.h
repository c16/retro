/**
 * NEON DEFENDER - Audio System
 * PWM-based audio output with procedural sound generation
 * Connect GPIO2 to speaker/amplifier for audio output
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <cstdint>

#define AUDIO_PIN 2           // GPIO2 for PWM audio output
#define SAMPLE_RATE 22050     // 22.05kHz audio
#define AUDIO_BUFFER_SIZE 256

// Sound effects
enum SoundEffect {
    SFX_SHOOT,
    SFX_HIT,
    SFX_EXPLOSION,
    SFX_POWERUP,
    SFX_COMBO,
    SFX_GAMEOVER,
    SFX_WAVE_COMPLETE,
    SFX_BOSS_HIT,
    SFX_MENU_MOVE,
    SFX_MENU_SELECT
};

class AudioSystem {
public:
    AudioSystem();

    // Initialize PWM audio system
    bool begin();

    // Play sound effects
    void playSFX(SoundEffect effect);

    // Background music control
    void startMusic();
    void stopMusic();
    void setMusicTempo(uint16_t bpm);
    bool isMusicPlaying() { return m_musicPlaying; }

    // Volume control (0-100)
    void setVolume(uint8_t volume);

    // Update audio (call from main loop)
    void update();

private:
    // Sound synthesis
    int16_t generateSquareWave(float phase, float amplitude);
    int16_t generateSineWave(float phase, float amplitude);
    int16_t generateTriangleWave(float phase, float amplitude);
    int16_t generateSawWave(float phase, float amplitude);
    int16_t generateNoise(float amplitude);

    // Music generation (procedural chiptune)
    void generateMusicSample();

    // SFX generation
    void generateSFXSample(SoundEffect sfx, uint32_t position, int16_t* output);

    // Get note frequency
    float getNoteFreq(int note);

    bool m_musicPlaying;
    uint16_t m_tempo;
    uint8_t m_volume;

    // Music state
    uint32_t m_musicSampleCount;
    uint32_t m_beatSamples;
    uint8_t m_currentNote;
    uint8_t m_currentPattern;
    float m_musicPhase;

    // PWM state
    uint8_t m_pwmChannel;

    // Active sound effects
    struct ActiveSFX {
        SoundEffect type;
        uint32_t position;
        uint32_t duration;
        bool active;
    };
    static const int MAX_SFX = 4;
    ActiveSFX m_activeSFX[MAX_SFX];

    // Audio mixing buffer
    int16_t m_mixBuffer;
};

// Global audio system instance
extern AudioSystem audio;

#endif // AUDIO_H
