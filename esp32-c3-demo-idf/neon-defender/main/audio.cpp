/**
 * NEON DEFENDER - Audio System Implementation
 */

#include "audio.h"
#include <driver/ledc.h>

// Global instance
AudioSystem audio;

// Music patterns (note indices: 0=C, 2=D, 4=E, 5=F, 7=G, 9=A, 11=B)
const int8_t MUSIC_PATTERN[] = {
    0, 7, 12, 7, 0, 7, 12, 7,   // Bass pattern
    4, 7, 11, 7, 4, 7, 11, 7,
    5, 7, 12, 7, 5, 7, 12, 7,
    2, 7, 11, 7, 2, 7, 11, 7
};
const int PATTERN_LENGTH = sizeof(MUSIC_PATTERN) / sizeof(MUSIC_PATTERN[0]);

AudioSystem::AudioSystem()
    : m_musicPlaying(false)
    , m_tempo(140)
    , m_volume(70)
    , m_musicSampleCount(0)
    , m_beatSamples(0)
    , m_currentNote(0)
    , m_currentPattern(0)
    , m_musicPhase(0)
    , m_pwmChannel(0)
    , m_mixBuffer(0)
{
    for (int i = 0; i < MAX_SFX; i++) {
        m_activeSFX[i].active = false;
    }
}

bool AudioSystem::begin() {
    // Configure LEDC for PWM audio on GPIO2
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_conf.duty_resolution = LEDC_TIMER_8_BIT;
    timer_conf.timer_num = LEDC_TIMER_0;
    timer_conf.freq_hz = SAMPLE_RATE;
    timer_conf.clk_cfg = LEDC_AUTO_CLK;

    esp_err_t err = ledc_timer_config(&timer_conf);
    if (err != ESP_OK) {
        Serial.printf("LEDC timer config failed: %d\n", err);
        return false;
    }

    ledc_channel_config_t channel_conf = {};
    channel_conf.gpio_num = AUDIO_PIN;
    channel_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    channel_conf.channel = LEDC_CHANNEL_0;
    channel_conf.timer_sel = LEDC_TIMER_0;
    channel_conf.duty = 128;
    channel_conf.hpoint = 0;

    err = ledc_channel_config(&channel_conf);
    if (err != ESP_OK) {
        Serial.printf("LEDC channel config failed: %d\n", err);
        return false;
    }

    m_pwmChannel = LEDC_CHANNEL_0;
    m_beatSamples = (SAMPLE_RATE * 60) / m_tempo / 4; // 16th note

    Serial.println("Audio system initialized (PWM on GPIO2)");
    return true;
}

void AudioSystem::playSFX(SoundEffect effect) {
    // Find free SFX slot
    for (int i = 0; i < MAX_SFX; i++) {
        if (!m_activeSFX[i].active) {
            m_activeSFX[i].type = effect;
            m_activeSFX[i].position = 0;
            m_activeSFX[i].active = true;

            // Set duration based on effect type
            switch (effect) {
                case SFX_SHOOT:       m_activeSFX[i].duration = SAMPLE_RATE / 20; break;  // 50ms
                case SFX_HIT:         m_activeSFX[i].duration = SAMPLE_RATE / 10; break;  // 100ms
                case SFX_EXPLOSION:   m_activeSFX[i].duration = SAMPLE_RATE / 2; break;   // 500ms
                case SFX_POWERUP:     m_activeSFX[i].duration = SAMPLE_RATE / 4; break;   // 250ms
                case SFX_COMBO:       m_activeSFX[i].duration = SAMPLE_RATE / 8; break;   // 125ms
                case SFX_GAMEOVER:    m_activeSFX[i].duration = SAMPLE_RATE * 2; break;   // 2s
                case SFX_WAVE_COMPLETE: m_activeSFX[i].duration = SAMPLE_RATE; break;    // 1s
                case SFX_BOSS_HIT:    m_activeSFX[i].duration = SAMPLE_RATE / 5; break;   // 200ms
                case SFX_MENU_MOVE:   m_activeSFX[i].duration = SAMPLE_RATE / 40; break;  // 25ms
                case SFX_MENU_SELECT: m_activeSFX[i].duration = SAMPLE_RATE / 15; break;  // 66ms
            }
            break;
        }
    }
}

void AudioSystem::startMusic() {
    m_musicPlaying = true;
    m_musicSampleCount = 0;
    m_currentNote = 0;
    m_currentPattern = 0;
    Serial.println("Music started");
}

void AudioSystem::stopMusic() {
    m_musicPlaying = false;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)m_pwmChannel, 128);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)m_pwmChannel);
}

void AudioSystem::setMusicTempo(uint16_t bpm) {
    m_tempo = bpm;
    m_beatSamples = (SAMPLE_RATE * 60) / m_tempo / 4;
}

void AudioSystem::setVolume(uint8_t volume) {
    m_volume = constrain(volume, 0, 100);
}

float AudioSystem::getNoteFreq(int note) {
    // A4 = 440Hz, note 57
    return 440.0 * pow(2.0, (note - 57) / 12.0);
}

int16_t AudioSystem::generateSquareWave(float phase, float amplitude) {
    return (fmod(phase, 1.0) < 0.5) ? amplitude : -amplitude;
}

int16_t AudioSystem::generateSineWave(float phase, float amplitude) {
    return amplitude * sin(phase * 2 * PI);
}

int16_t AudioSystem::generateTriangleWave(float phase, float amplitude) {
    float p = fmod(phase, 1.0);
    return amplitude * (p < 0.5 ? (4 * p - 1) : (3 - 4 * p));
}

int16_t AudioSystem::generateSawWave(float phase, float amplitude) {
    return amplitude * (2 * fmod(phase, 1.0) - 1);
}

int16_t AudioSystem::generateNoise(float amplitude) {
    return amplitude * ((float)random(-1000, 1000) / 1000.0);
}

void AudioSystem::generateSFXSample(SoundEffect sfx, uint32_t position, int16_t* output) {
    float t = (float)position / SAMPLE_RATE;
    float progress = (float)position / m_activeSFX[0].duration;

    switch (sfx) {
        case SFX_SHOOT: {
            // Quick chirp upward
            float freq = 200 + position * 10;
            *output = generateSquareWave(freq * t, 5000 * (1.0 - progress));
            break;
        }

        case SFX_HIT: {
            // Sharp blip
            float freq = 600 - position * 3;
            *output = generateSquareWave(freq * t, 8000 * (1.0 - progress));
            break;
        }

        case SFX_EXPLOSION: {
            // Noise burst with lowpass
            float envelope = pow(1.0 - progress, 2);
            *output = generateNoise(15000 * envelope);
            break;
        }

        case SFX_POWERUP: {
            // Arpeggio up
            int step = (position * 8) / m_activeSFX[0].duration;
            float freq = getNoteFreq(60 + step * 7);  // C major arpeggio
            *output = generateSineWave(freq * t, 10000 * (1.0 - progress));
            break;
        }

        case SFX_COMBO: {
            // Quick beep
            *output = generateSquareWave(1000 * t, 12000 * (1.0 - progress));
            break;
        }

        case SFX_WAVE_COMPLETE: {
            // Victory fanfare
            int step = (position * 4) / m_activeSFX[0].duration;
            float freq = getNoteFreq(64 + step * 4);  // E major arpeggio
            *output = generateSineWave(freq * t, 15000 * (1.0 - progress));
            break;
        }

        case SFX_BOSS_HIT: {
            // Deep hit
            float freq = 80 - position * 0.2;
            *output = generateSquareWave(freq * t, 20000 * (1.0 - progress));
            break;
        }

        case SFX_GAMEOVER: {
            // Descending tone
            float freq = 400 * pow(0.5, progress * 2);
            *output = generateSineWave(freq * t, 18000 * (1.0 - progress));
            break;
        }

        case SFX_MENU_MOVE: {
            // Blip
            *output = generateSquareWave(800 * t, 5000);
            break;
        }

        case SFX_MENU_SELECT: {
            // Confirm beep
            *output = generateSquareWave(1200 * t, 8000 * (1.0 - progress));
            break;
        }
    }
}

void AudioSystem::generateMusicSample() {
    if (!m_musicPlaying) return;

    // Get current note from pattern
    int noteIndex = MUSIC_PATTERN[m_currentNote];
    float baseNote = 36; // C2
    float freq = getNoteFreq(baseNote + noteIndex);

    // Generate bass + lead
    float t = (float)m_musicSampleCount / SAMPLE_RATE;
    int16_t bass = generateSquareWave(freq * t, 8000);

    // Add lead melody (octave up, different pattern)
    int leadNote = MUSIC_PATTERN[(m_currentNote + 8) % PATTERN_LENGTH];
    float leadFreq = getNoteFreq(baseNote + 24 + leadNote);
    int16_t lead = generateSquareWave(leadFreq * t, 4000);

    m_mixBuffer = bass + lead;

    m_musicSampleCount++;

    // Advance to next note
    if (m_musicSampleCount >= m_beatSamples) {
        m_musicSampleCount = 0;
        m_currentNote = (m_currentNote + 1) % PATTERN_LENGTH;
    }
}

void AudioSystem::update() {
    int32_t mixedSample = 0;

    // Generate music
    if (m_musicPlaying) {
        generateMusicSample();
        mixedSample = m_mixBuffer;
    }

    // Mix in active sound effects
    for (int i = 0; i < MAX_SFX; i++) {
        if (m_activeSFX[i].active) {
            int16_t sfxSample = 0;
            generateSFXSample(m_activeSFX[i].type, m_activeSFX[i].position, &sfxSample);
            mixedSample += sfxSample;

            m_activeSFX[i].position++;
            if (m_activeSFX[i].position >= m_activeSFX[i].duration) {
                m_activeSFX[i].active = false;
            }
        }
    }

    // Apply volume and clamp
    mixedSample = (mixedSample * m_volume) / 100;
    mixedSample = constrain(mixedSample, -32768, 32767);

    // Convert to 8-bit PWM duty cycle
    uint32_t duty = ((mixedSample + 32768) >> 8); // 0-255
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)m_pwmChannel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)m_pwmChannel);
}
