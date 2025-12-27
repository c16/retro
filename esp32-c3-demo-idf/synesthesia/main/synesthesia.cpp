/**
 * SYNESTHESIA - Implementation
 * Where art meets code, and music meets mathematics
 */

#include "synesthesia.h"

// Static instance for ISR
AudioEngine* AudioEngine::s_instance = nullptr;

// ============================================================================
// COLOR IMPLEMENTATION
// ============================================================================

Color Color::fromHSV(float h, float s, float v) {
    h = fmodf(h, 360.0f);
    if (h < 0) h += 360.0f;

    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return fromRGB(
        (uint8_t)((r + m) * 255),
        (uint8_t)((g + m) * 255),
        (uint8_t)((b + m) * 255)
    );
}

Color Color::blend(const Color& other, float t) const {
    uint8_t r1 = (rgb565 >> 11) << 3;
    uint8_t g1 = ((rgb565 >> 5) & 0x3F) << 2;
    uint8_t b1 = (rgb565 & 0x1F) << 3;

    uint8_t r2 = (other.rgb565 >> 11) << 3;
    uint8_t g2 = ((other.rgb565 >> 5) & 0x3F) << 2;
    uint8_t b2 = (other.rgb565 & 0x1F) << 3;

    uint8_t r = r1 * (1.0f - t) + r2 * t;
    uint8_t g = g1 * (1.0f - t) + g2 * t;
    uint8_t b = b1 * (1.0f - t) + b2 * t;

    return fromRGB(r, g, b);
}

// ============================================================================
// AUDIO ENGINE IMPLEMENTATION
// ============================================================================

AudioEngine::AudioEngine()
    : m_scale(SCALE_PENTATONIC)
    , m_tempo(120.0f)
    , m_masterVolume(0.3f)
    , m_timer(nullptr)
{
    s_instance = this;
}

void AudioEngine::begin() {
    pinMode(AUDIO_PIN, OUTPUT);

    // Setup PWM
    ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(AUDIO_PIN, 0);

    Serial.println("Audio engine initialized on GPIO2");
}

float AudioEngine::midiToFreq(int midiNote) {
    return 440.0f * powf(2.0f, (midiNote - 69) / 12.0f);
}

void AudioEngine::playNote(int midiNote, float duration, float velocity, Waveform wave) {
    // Find free voice
    for (int i = 0; i < MAX_VOICES; i++) {
        if (!m_voices[i].active) {
            m_voices[i].frequency = midiToFreq(midiNote);
            m_voices[i].amplitude = velocity;
            m_voices[i].waveform = wave;
            m_voices[i].active = true;
            m_voices[i].envelope = 0;
            m_voices[i].envTime = 0;
            m_voices[i].phase = 0;
            return;
        }
    }
}

void AudioEngine::stopAll() {
    for (int i = 0; i < MAX_VOICES; i++) {
        m_voices[i].active = false;
    }
}

void AudioEngine::setScale(Scale scale) {
    m_scale = scale;
}

void AudioEngine::setTempo(float bpm) {
    m_tempo = bpm;
}

void AudioEngine::setMasterVolume(float volume) {
    m_masterVolume = constrain(volume, 0.0f, 1.0f);
}

void AudioEngine::update(float deltaTime) {
    // Mix all active voices
    float mixedSample = 0;

    for (int i = 0; i < MAX_VOICES; i++) {
        if (m_voices[i].active) {
            updateVoiceEnvelope(m_voices[i], deltaTime);
            mixedSample += generateSample(m_voices[i]);
        }
    }

    // Apply master volume and convert to PWM value
    mixedSample *= m_masterVolume;
    int pwmValue = (int)((mixedSample + 1.0f) * 127.5f);
    pwmValue = constrain(pwmValue, 0, 255);

    ledcWrite(0, pwmValue);
}

float AudioEngine::generateSample(const Voice& voice) {
    if (!voice.active) return 0;

    float sample = 0;

    switch (voice.waveform) {
        case WAVE_SINE:
            sample = sinf(voice.phase * 2.0f * M_PI);
            break;

        case WAVE_SQUARE:
            sample = voice.phase < 0.5f ? 1.0f : -1.0f;
            break;

        case WAVE_TRIANGLE:
            sample = fabsf(voice.phase * 4.0f - 2.0f) - 1.0f;
            break;

        case WAVE_SAW:
            sample = voice.phase * 2.0f - 1.0f;
            break;

        case WAVE_NOISE:
            sample = (random(0, 1000) / 500.0f) - 1.0f;
            break;
    }

    return sample * voice.amplitude * voice.envelope;
}

void AudioEngine::updateVoiceEnvelope(Voice& voice, float deltaTime) {
    voice.phase += voice.frequency / SAMPLE_RATE;
    if (voice.phase >= 1.0f) voice.phase -= 1.0f;

    voice.envTime += deltaTime;

    // Simple envelope
    if (voice.envTime < voice.attack) {
        voice.envelope = voice.envTime / voice.attack;
    } else if (voice.envTime < voice.attack + voice.decay) {
        float t = (voice.envTime - voice.attack) / voice.decay;
        voice.envelope = 1.0f - (1.0f - voice.sustain) * t;
    } else if (voice.envTime < voice.attack + voice.decay + 1.0f) {
        voice.envelope = voice.sustain;
    } else {
        float t = (voice.envTime - voice.attack - voice.decay - 1.0f) / voice.release;
        voice.envelope = voice.sustain * (1.0f - t);
        if (t >= 1.0f) {
            voice.active = false;
        }
    }
}

// ============================================================================
// MUSIC COMPOSER IMPLEMENTATION
// ============================================================================

MusicComposer::MusicComposer(AudioEngine* audio)
    : m_audio(audio)
    , m_mood(MOOD_CALM)
    , m_tempo(90.0f)
    , m_scale(SCALE_PENTATONIC)
    , m_rootNote(60)
    , m_beatTime(0)
    , m_currentBeat(0)
    , m_currentBar(0)
    , m_complexity(0.5f)
    , m_dissonance(0.1f)
    , m_density(0.5f)
    , m_sequenceLength(8)
{
}

void MusicComposer::begin() {
    generateMelody();
}

void MusicComposer::setMood(Mood mood) {
    m_mood = mood;

    // Adjust parameters based on mood
    switch (mood) {
        case MOOD_CALM:
            m_tempo = 80; m_scale = SCALE_PENTATONIC;
            m_complexity = 0.3f; m_density = 0.4f;
            break;
        case MOOD_ENERGETIC:
            m_tempo = 140; m_scale = SCALE_MAJOR;
            m_complexity = 0.7f; m_density = 0.8f;
            break;
        case MOOD_MYSTERIOUS:
            m_tempo = 70; m_scale = SCALE_PHRYGIAN;
            m_complexity = 0.8f; m_density = 0.5f;
            break;
        case MOOD_JOYFUL:
            m_tempo = 120; m_scale = SCALE_MAJOR;
            m_complexity = 0.5f; m_density = 0.7f;
            break;
        case MOOD_MELANCHOLIC:
            m_tempo = 75; m_scale = SCALE_MINOR;
            m_complexity = 0.6f; m_density = 0.5f;
            break;
        case MOOD_TRANSCENDENT:
            m_tempo = 60; m_scale = SCALE_MIXOLYDIAN;
            m_complexity = 0.9f; m_density = 0.3f;
            break;
        case MOOD_CHAOTIC:
            m_tempo = 160; m_scale = SCALE_CHROMATIC;
            m_complexity = 1.0f; m_density = 0.9f;
            break;
        case MOOD_MEDITATIVE:
            m_tempo = 50; m_scale = SCALE_PENTATONIC;
            m_complexity = 0.2f; m_density = 0.2f;
            break;
    }

    m_audio->setScale(m_scale);
    m_audio->setTempo(m_tempo);
}

void MusicComposer::update(float deltaTime) {
    m_beatTime += deltaTime * (m_tempo / 60.0f);

    if (m_beatTime >= m_sequence[m_currentBeat].duration) {
        m_beatTime -= m_sequence[m_currentBeat].duration;
        m_currentBeat++;

        if (m_currentBeat >= m_sequenceLength) {
            m_currentBeat = 0;
            m_currentBar++;

            // Regenerate every 4 bars
            if (m_currentBar % 4 == 0) {
                generate();
            }
        }

        // Play note
        if (m_currentBeat < m_sequenceLength) {
            Note& note = m_sequence[m_currentBeat];
            if (note.pitch > 0) {
                Waveform wave = (random(0, 100) < 70) ? WAVE_SINE : WAVE_TRIANGLE;
                m_audio->playNote(note.pitch, note.duration, note.velocity, wave);
            }
        }
    }
}

void MusicComposer::generate() {
    generateMelody();
}

void MusicComposer::generateMelody() {
    m_sequenceLength = 8;

    for (int i = 0; i < m_sequenceLength; i++) {
        // Random melody based on complexity
        if (random(0, 100) < m_density * 100) {
            int degree = random(0, 8);
            m_sequence[i].pitch = m_rootNote + getScaleNote(degree);
            m_sequence[i].duration = (random(0, 100) < 50) ? 0.5f : 1.0f;
            m_sequence[i].velocity = 0.5f + random(0, 50) / 100.0f;
        } else {
            m_sequence[i].pitch = 0; // Rest
            m_sequence[i].duration = 0.5f;
        }
    }
}

int MusicComposer::getScaleNote(int degree) {
    int scales[][8] = {
        {0, 2, 4, 5, 7, 9, 11, 12},  // Major
        {0, 2, 3, 5, 7, 8, 10, 12},  // Minor
        {0, 2, 4, 7, 9, 12, 14, 16}, // Pentatonic
        {0, 3, 5, 6, 7, 10, 12, 15}, // Blues
        {0, 2, 3, 5, 7, 9, 10, 12},  // Dorian
        {0, 1, 3, 5, 7, 8, 10, 12},  // Phrygian
        {0, 2, 4, 5, 7, 9, 10, 12},  // Mixolydian
        {0, 1, 2, 3, 4, 5, 6, 7}     // Chromatic
    };

    return scales[m_scale][degree % 8];
}

// ============================================================================
// FLOW FIELD GENERATOR IMPLEMENTATION
// ============================================================================

FlowFieldGenerator::FlowFieldGenerator()
    : m_mood(MOOD_CALM)
    , m_noiseScale(0.1f)
    , m_noiseTime(0)
    , m_flowStrength(2.0f)
{
}

void FlowFieldGenerator::begin() {
    // Initialize particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        m_particles[i].pos = Vec2(random(0, SCREEN_WIDTH), random(0, SCREEN_HEIGHT));
        m_particles[i].vel = Vec2(0, 0);
        m_particles[i].life = 0;
        m_particles[i].maxLife = random(50, 150) / 100.0f;
        m_particles[i].size = random(1, 3);
        m_particles[i].active = true;
        m_particles[i].color = Color::fromHSV(random(0, 360), 0.7f, 0.8f);
    }
}

void FlowFieldGenerator::setMood(Mood mood) {
    m_mood = mood;
}

void FlowFieldGenerator::update(float deltaTime, float audioLevel, float beatPhase) {
    m_noiseTime += deltaTime * 0.3f;
    updateFlowField(deltaTime);

    // Update particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (m_particles[i].active) {
            Vec2 flow = getFlowAt(m_particles[i].pos);
            m_particles[i].vel = m_particles[i].vel * 0.95f + flow * m_flowStrength;

            m_particles[i].pos = m_particles[i].pos + m_particles[i].vel * deltaTime * 20.0f;

            // Wrap around
            if (m_particles[i].pos.x < 0) m_particles[i].pos.x += SCREEN_WIDTH;
            if (m_particles[i].pos.x >= SCREEN_WIDTH) m_particles[i].pos.x -= SCREEN_WIDTH;
            if (m_particles[i].pos.y < 0) m_particles[i].pos.y += SCREEN_HEIGHT;
            if (m_particles[i].pos.y >= SCREEN_HEIGHT) m_particles[i].pos.y -= SCREEN_HEIGHT;

            m_particles[i].life += deltaTime;
            if (m_particles[i].life >= m_particles[i].maxLife) {
                m_particles[i].life = 0;
                m_particles[i].color = Color::fromHSV(
                    fmodf(m_noiseTime * 50 + i * 30, 360),
                    0.7f + audioLevel * 0.3f,
                    0.6f + audioLevel * 0.4f
                );
            }
        }
    }
}

void FlowFieldGenerator::updateFlowField(float deltaTime) {
    for (int y = 0; y < FLOW_FIELD_RES; y++) {
        for (int x = 0; x < FLOW_FIELD_RES; x++) {
            float nx = x * m_noiseScale;
            float ny = y * m_noiseScale;
            float angle = noise2D(nx, ny + m_noiseTime) * M_PI * 2;

            m_flowField[x][y] = Vec2(cosf(angle), sinf(angle));
        }
    }
}

Vec2 FlowFieldGenerator::getFlowAt(Vec2 pos) {
    int x = (int)(pos.x / SCREEN_WIDTH * FLOW_FIELD_RES);
    int y = (int)(pos.y / SCREEN_HEIGHT * FLOW_FIELD_RES);

    x = constrain(x, 0, FLOW_FIELD_RES - 1);
    y = constrain(y, 0, FLOW_FIELD_RES - 1);

    return m_flowField[x][y];
}

float FlowFieldGenerator::noise2D(float x, float y) {
    // Simple hash-based noise
    float value = sinf(x * 12.9898f + y * 78.233f) * 43758.5453f;
    value = value - floorf(value);
    return value;
}

void FlowFieldGenerator::render(TFT_eSPI* tft) {
    // Fade effect
    tft->fillScreen(Color::fromRGB(5, 5, 10).rgb565);

    // Draw particles
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (m_particles[i].active) {
            float alpha = 1.0f - (m_particles[i].life / m_particles[i].maxLife);
            if (alpha > 0.2f) {
                int x = (int)m_particles[i].pos.x;
                int y = (int)m_particles[i].pos.y;

                if (m_particles[i].size > 1) {
                    tft->fillCircle(x, y, (int)m_particles[i].size, m_particles[i].color.rgb565);
                } else {
                    tft->drawPixel(x, y, m_particles[i].color.rgb565);
                }
            }
        }
    }
}

// ============================================================================
// FRACTAL GENERATOR IMPLEMENTATION
// ============================================================================

FractalGenerator::FractalGenerator()
    : m_mood(MOOD_MYSTERIOUS)
    , m_zoom(1.0f)
    , m_centerX(-0.5f)
    , m_centerY(0.0f)
    , m_rotation(0)
    , m_maxIterations(32)
{
}

void FractalGenerator::begin() {
}

void FractalGenerator::setMood(Mood mood) {
    m_mood = mood;
}

void FractalGenerator::update(float deltaTime, float audioLevel, float beatPhase) {
    m_rotation += deltaTime * 0.3f;
    m_zoom = 1.0f + sinf(m_rotation * 0.5f) * 0.5f;

    // Slowly pan
    m_centerX = -0.5f + cosf(m_rotation * 0.2f) * 0.3f;
    m_centerY = sinf(m_rotation * 0.15f) * 0.3f;
}

void FractalGenerator::render(TFT_eSPI* tft) {
    for (int py = 0; py < SCREEN_HEIGHT; py += 2) {
        for (int px = 0; px < SCREEN_WIDTH; px += 2) {
            float x = (px - SCREEN_CENTER_X) / (SCREEN_WIDTH * 0.25f * m_zoom) + m_centerX;
            float y = (py - SCREEN_CENTER_Y) / (SCREEN_HEIGHT * 0.25f * m_zoom) + m_centerY;

            float smoothValue;
            int iter = mandelbrot(x, y, smoothValue);

            Color c = getIterationColor(iter, smoothValue);
            tft->fillRect(px, py, 2, 2, c.rgb565);
        }
        if (py % 10 == 0) yield();
    }
}

int FractalGenerator::mandelbrot(float x0, float y0, float& smoothValue) {
    float x = 0, y = 0;
    int iter = 0;

    while (x * x + y * y <= 4.0f && iter < m_maxIterations) {
        float xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iter++;
    }

    if (iter < m_maxIterations) {
        smoothValue = iter + 1 - logf(logf(sqrtf(x * x + y * y))) / logf(2.0f);
    } else {
        smoothValue = iter;
    }

    return iter;
}

Color FractalGenerator::getIterationColor(int iter, float smooth) {
    if (iter >= m_maxIterations) {
        return Color::fromRGB(0, 0, 0);
    }

    float hue = fmodf(smooth * 360.0f / m_maxIterations + m_rotation * 50, 360);
    float sat = 0.8f;
    float val = (iter < m_maxIterations) ? 0.9f : 0.0f;

    return Color::fromHSV(hue, sat, val);
}

// ============================================================================
// CELLULAR AUTOMATA GENERATOR
// ============================================================================

AutomataGenerator::AutomataGenerator()
    : m_mood(MOOD_CHAOTIC)
    , m_rule(30)
    , m_updateTimer(0)
    , m_updateInterval(0.1f)
    , m_generation(0)
{
}

void AutomataGenerator::begin() {
    randomize();
}

void AutomataGenerator::setMood(Mood mood) {
    m_mood = mood;
}

void AutomataGenerator::randomize() {
    for (int i = 0; i < MAX_AUTOMATA_CELLS; i++) {
        m_cells[i] = random(0, 2);
    }
    m_generation = 0;
}

void AutomataGenerator::update(float deltaTime, float audioLevel, float beatPhase) {
    m_updateTimer += deltaTime;

    if (m_updateTimer >= m_updateInterval) {
        m_updateTimer = 0;
        applyRule();
        m_generation++;

        if (m_generation >= SCREEN_HEIGHT) {
            randomize();
        }
    }
}

void AutomataGenerator::applyRule() {
    for (int i = 0; i < MAX_AUTOMATA_CELLS; i++) {
        int left = (i > 0) ? m_cells[i - 1] : 0;
        int center = m_cells[i];
        int right = (i < MAX_AUTOMATA_CELLS - 1) ? m_cells[i + 1] : 0;

        int neighborhood = (left << 2) | (center << 1) | right;
        m_nextCells[i] = (m_rule >> neighborhood) & 1;
    }

    memcpy(m_cells, m_nextCells, MAX_AUTOMATA_CELLS);
}

void AutomataGenerator::render(TFT_eSPI* tft) {
    int cellWidth = SCREEN_WIDTH / MAX_AUTOMATA_CELLS;

    for (int i = 0; i < MAX_AUTOMATA_CELLS; i++) {
        if (m_cells[i]) {
            uint16_t color = Color::fromHSV(i * 1.4f, 0.8f, 0.9f).rgb565;
            tft->fillRect(i * cellWidth, m_generation % SCREEN_HEIGHT, cellWidth, 2, color);
        }
    }
}

// ============================================================================
// MEDITATION GENERATOR IMPLEMENTATION
// ============================================================================

MeditationGenerator::MeditationGenerator()
    : m_mood(MOOD_MEDITATIVE)
    , m_breathPhase(0)
    , m_breathRate(0.15f)
    , m_auroraPhase(0)
{
}

void MeditationGenerator::begin() {
}

void MeditationGenerator::setMood(Mood mood) {
    m_mood = mood;
}

void MeditationGenerator::update(float deltaTime, float audioLevel, float beatPhase) {
    m_breathPhase += deltaTime * m_breathRate;
    m_auroraPhase += deltaTime * 0.2f;
}

void MeditationGenerator::render(TFT_eSPI* tft) {
    tft->fillScreen(Color::fromRGB(5, 0, 15).rgb565);

    renderAurora(tft);
    renderBreathingCircle(tft, 40 + sinf(m_breathPhase * M_PI * 2) * 20, 0.8f);

    // Center dot
    tft->fillCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, 3, TFT_WHITE);
}

void MeditationGenerator::renderBreathingCircle(TFT_eSPI* tft, float radius, float alpha) {
    int segments = 36;
    for (int i = 0; i < segments; i++) {
        float angle1 = i * 2 * M_PI / segments;
        float angle2 = (i + 1) * 2 * M_PI / segments;

        int x1 = SCREEN_CENTER_X + cosf(angle1) * radius;
        int y1 = SCREEN_CENTER_Y + sinf(angle1) * radius;
        int x2 = SCREEN_CENTER_X + cosf(angle2) * radius;
        int y2 = SCREEN_CENTER_Y + sinf(angle2) * radius;

        float hue = fmodf(i * 10 + m_auroraPhase * 50, 360);
        uint16_t color = Color::fromHSV(hue, 0.6f, alpha).rgb565;

        tft->drawLine(x1, y1, x2, y2, color);
    }
}

void MeditationGenerator::renderAurora(TFT_eSPI* tft) {
    for (int y = 0; y < SCREEN_HEIGHT; y += 8) {
        for (int x = 0; x < SCREEN_WIDTH; x += 8) {
            float wave = sinf(x * 0.05f + m_auroraPhase) * 0.5f + 0.5f;
            float hue = fmodf(wave * 120 + 180, 360);
            uint16_t color = Color::fromHSV(hue, 0.7f, 0.2f).rgb565;
            tft->fillRect(x, y, 8, 8, color);
        }
    }
}

// ============================================================================
// MAIN SYNESTHESIA ENGINE IMPLEMENTATION
// ============================================================================

Synesthesia::Synesthesia(TFT_eSPI* display)
    : m_tft(display)
    , m_audio(nullptr)
    , m_composer(nullptr)
    , m_visualGenerator(nullptr)
    , m_mood(MOOD_CALM)
    , m_artMode(ART_FLOW_FIELD)
    , m_time(0)
    , m_audioLevel(0)
    , m_beatPhase(0)
    , m_evolutionRate(0.01f)
    , m_evolutionTimer(0)
    , m_buttonPressed(false)
    , m_lastButtonState(false)
    , m_buttonPressTime(0)
{
}

Synesthesia::~Synesthesia() {
    if (m_audio) delete m_audio;
    if (m_composer) delete m_composer;
    if (m_visualGenerator) delete m_visualGenerator;
}

void Synesthesia::begin() {
    pinMode(BOOT_BUTTON, INPUT_PULLUP);

    // Initialize audio
    m_audio = new AudioEngine();
    m_audio->begin();

    // Initialize composer
    m_composer = new MusicComposer(m_audio);
    m_composer->begin();
    m_composer->setMood(m_mood);

    // Initialize visual generator
    createVisualGenerator();

    Serial.println("Synesthesia initialized");
    Serial.printf("Mood: %s\n", getMoodName(m_mood));
    Serial.printf("Art Mode: %s\n", getArtModeName(m_artMode));
}

void Synesthesia::createVisualGenerator() {
    if (m_visualGenerator) delete m_visualGenerator;

    switch (m_artMode) {
        case ART_FLOW_FIELD:
            m_visualGenerator = new FlowFieldGenerator();
            break;
        case ART_FRACTAL:
            m_visualGenerator = new FractalGenerator();
            break;
        case ART_AUTOMATA:
            m_visualGenerator = new AutomataGenerator();
            break;
        case ART_MEDITATION:
            m_visualGenerator = new MeditationGenerator();
            break;
        default:
            m_visualGenerator = new FlowFieldGenerator();
            break;
    }

    m_visualGenerator->begin();
    m_visualGenerator->setMood(m_mood);
}

void Synesthesia::update(float deltaTime) {
    m_time += deltaTime;

    // Update audio and music
    m_audio->update(deltaTime);
    m_composer->update(deltaTime);

    // Update visuals
    m_visualGenerator->update(deltaTime, m_audioLevel, m_beatPhase);

    // Evolution
    updateEvolution(deltaTime);

    // Handle input
    handleInput();
}

void Synesthesia::updateEvolution(float deltaTime) {
    m_evolutionTimer += deltaTime;

    // Subtle evolution every 30 seconds
    if (m_evolutionTimer >= 30.0f) {
        m_evolutionTimer = 0;
        evolve();
    }
}

void Synesthesia::evolve() {
    // Regenerate music
    m_composer->generate();

    Serial.println("Evolution: Music regenerated");
}

void Synesthesia::handleInput() {
    m_buttonPressed = (digitalRead(BOOT_BUTTON) == LOW);

    // Button press - cycle through options
    if (m_buttonPressed && !m_lastButtonState) {
        unsigned long pressTime = millis();

        if (pressTime - m_buttonPressTime > 500) {
            cycleMood();
            m_buttonPressTime = pressTime;
        }
    }

    m_lastButtonState = m_buttonPressed;
}

void Synesthesia::cycleMood() {
    int nextMood = ((int)m_mood + 1) % 8;
    m_mood = (Mood)nextMood;

    m_composer->setMood(m_mood);
    m_visualGenerator->setMood(m_mood);

    Serial.printf("Mood changed to: %s\n", getMoodName(m_mood));
}

void Synesthesia::cycleArtMode() {
    int nextMode = ((int)m_artMode + 1) % 8;
    m_artMode = (ArtMode)nextMode;

    createVisualGenerator();

    Serial.printf("Art mode changed to: %s\n", getArtModeName(m_artMode));
}

void Synesthesia::render() {
    m_visualGenerator->render(m_tft);

    // Show current mood
    m_tft->setTextColor(TFT_WHITE, Color::fromRGB(0, 0, 0).rgb565);
    m_tft->setTextDatum(TL_DATUM);
    m_tft->drawString(getMoodName(m_mood), 5, 5, 2);
}

const char* Synesthesia::getMoodName(Mood mood) {
    const char* names[] = {
        "Calm", "Energetic", "Mysterious", "Joyful",
        "Melancholic", "Transcendent", "Chaotic", "Meditative"
    };
    return names[(int)mood];
}

const char* Synesthesia::getArtModeName(ArtMode mode) {
    const char* names[] = {
        "Flow Field", "Fractal", "Automata", "Waves",
        "Mandala", "Plasma", "Galaxy", "Meditation"
    };
    return names[(int)mode];
}
