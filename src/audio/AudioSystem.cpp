#include "audio/AudioSystem.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace hollow {

namespace {

constexpr unsigned kSampleRate = 44100;
constexpr float    kTau        = 6.28318530718f;

// Fixed-seed LCG — same noise pattern every run, so generated buffers are
// bitwise identical across launches. Fast enough to generate thousands of
// samples without measurable startup cost.
struct Lcg {
    uint32_t s = 0xDEAD1337u;
    float next() {
        s = s * 1664525u + 1013904223u;
        return static_cast<float>(s >> 16) / 32767.5f - 1.f;
    }
};

// Fills a SoundBuffer from a sample generator: fn(float t) -> float in [-1,1].
// The lambda is called in sample order so stateful generators (e.g. Lcg) work.
template<typename F>
sf::SoundBuffer makeBuf(float dur, F fn)
{
    const int n = static_cast<int>(kSampleRate * dur);
    std::vector<sf::Int16> samples(n);
    for (int i = 0; i < n; ++i) {
        float t = static_cast<float>(i) / kSampleRate;
        float v = std::clamp(fn(t), -1.f, 1.f);
        samples[i] = static_cast<sf::Int16>(v * 32767.f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(samples.data(), static_cast<sf::Uint64>(samples.size()), 1, kSampleRate);
    return buf;
}

// For chirps and exponential sweeps the phase must be the integral of 2*pi*f(t),
// not sin(2*pi*f*t). Otherwise instantaneous frequency is wrong at every sample.

sf::SoundBuffer makeSwing()
{
    Lcg rng;
    // Descending whoosh 700→200 Hz mixed with wind noise.
    return makeBuf(0.15f, [&](float t) {
        constexpr float D = 0.15f, f0 = 700.f, f1 = 200.f;
        const float env   = std::exp(-7.f * t) * (1.f - std::exp(-80.f * t));
        const float phase = kTau * (f0 * t + 0.5f * (f1 - f0) * t * t / D);
        return (std::sin(phase) * 0.4f + rng.next() * 0.6f) * env;
    });
}

sf::SoundBuffer makeHit()
{
    Lcg rng;
    return makeBuf(0.09f, [&](float t) {
        // Pure noise burst — no pitch, just a sharp crack.
        const float env = std::exp(-38.f * t) * (1.f - std::exp(-300.f * t));
        return rng.next() * env;
    });
}

sf::SoundBuffer makeEnemyDeath()
{
    Lcg rng;
    return makeBuf(0.28f, [&](float t) {
        const float env = std::exp(-9.f * t) * (1.f - std::exp(-25.f * t));
        // Exponential pitch decay: f(t) = 220*exp(-4t).
        // Integral: 2pi * 55 * (1 - exp(-4t)) — starts near 220 Hz, falls to ~60.
        const float phase = kTau * 55.f * (1.f - std::exp(-4.f * t));
        return (std::sin(phase) * 0.5f + rng.next() * 0.5f) * env;
    });
}

sf::SoundBuffer makePlayerHit()
{
    Lcg rng;
    return makeBuf(0.18f, [&](float t) {
        // Low dull thud: two bass sines for body + a little grit.
        const float env  = std::exp(-15.f * t) * (1.f - std::exp(-90.f * t));
        const float tone = std::sin(kTau * 90.f * t) * 0.55f
                         + std::sin(kTau * 140.f * t) * 0.3f;
        return (tone + rng.next() * 0.3f) * env;
    });
}

sf::SoundBuffer makePlayerDeath()
{
    Lcg rng;
    return makeBuf(0.55f, [&](float t) {
        const float env = std::exp(-4.f * t) * (1.f - std::exp(-18.f * t));
        // Two descending sweeps layered for a dramatic rumble.
        // Linear: f(t)=160-100*(t/0.55), phase=2pi*(160t - 50t^2/0.55)
        const float p1 = kTau * (160.f * t - 50.f * t * t / 0.55f);
        // Exponential: f(t)=90*exp(-3t), phase=2pi*30*(1-exp(-3t))
        const float p2 = kTau * 30.f * (1.f - std::exp(-3.f * t));
        return (std::sin(p1) * 0.6f + std::sin(p2) * 0.35f + rng.next() * 0.15f) * env;
    });
}

sf::SoundBuffer makeDash()
{
    Lcg rng;
    // Rising chirp 320→1800 Hz — bright and snappy.
    return makeBuf(0.12f, [&](float t) {
        constexpr float D = 0.12f, f0 = 320.f, f1 = 1800.f;
        const float env   = std::exp(-10.f * t) * (1.f - std::exp(-120.f * t));
        const float phase = kTau * (f0 * t + 0.5f * (f1 - f0) * t * t / D);
        return (std::sin(phase) * 0.65f + rng.next() * 0.35f) * env;
    });
}

sf::SoundBuffer makeBoonPick()
{
    // Major 7th arpeggio: C4, E4, G4, B4 — ascending, rewarding.
    constexpr std::array<float, 4> kNotes = { 261.63f, 329.63f, 392.00f, 493.88f };
    constexpr float kNoteLen = 0.10f;
    return makeBuf(0.42f, [&](float t) {
        const int idx = static_cast<int>(t / kNoteLen);
        if (idx >= static_cast<int>(kNotes.size())) return 0.f;
        const float nt  = t - idx * kNoteLen;
        const float env = std::exp(-9.f * nt) * (1.f - std::exp(-200.f * nt));
        return std::sin(kTau * kNotes[idx] * nt) * env;
    });
}

sf::SoundBuffer makeWaveStart()
{
    Lcg rng;
    return makeBuf(0.50f, [&](float t) {
        // Power chord drone: A1 (55 Hz) + E2 (82.5 Hz, perfect 5th).
        // Swell envelope — builds tension without musical key commitment.
        const float rise = std::min(t / 0.20f, 1.f);
        const float fall = t > 0.35f ? std::max(0.f, 1.f - (t - 0.35f) / 0.15f) : 1.f;
        const float env  = rise * fall;
        const float tone = std::sin(kTau * 55.f * t)
                         + std::sin(kTau * 82.5f * t) * 0.7f;
        return (tone * 0.75f + rng.next() * 0.25f) * env * 0.5f;
    });
}

} // namespace

AudioSystem::AudioSystem()
{
    buildBuffers();
}

void AudioSystem::buildBuffers()
{
    m_buffers[static_cast<std::size_t>(Sfx::Swing)]       = makeSwing();
    m_buffers[static_cast<std::size_t>(Sfx::Hit)]         = makeHit();
    m_buffers[static_cast<std::size_t>(Sfx::EnemyDeath)]  = makeEnemyDeath();
    m_buffers[static_cast<std::size_t>(Sfx::PlayerHit)]   = makePlayerHit();
    m_buffers[static_cast<std::size_t>(Sfx::PlayerDeath)] = makePlayerDeath();
    m_buffers[static_cast<std::size_t>(Sfx::Dash)]        = makeDash();
    m_buffers[static_cast<std::size_t>(Sfx::BoonPick)]    = makeBoonPick();
    m_buffers[static_cast<std::size_t>(Sfx::WaveStart)]   = makeWaveStart();
}

void AudioSystem::play(Sfx sfx)
{
    sf::Sound& slot = idleSlot();
    slot.setBuffer(m_buffers[static_cast<std::size_t>(sfx)]);
    slot.play();
}

sf::Sound& AudioSystem::idleSlot()
{
    for (sf::Sound& s : m_pool) {
        if (s.getStatus() != sf::Sound::Playing) return s;
    }
    // All slots busy: round-robin eviction to avoid starving any sound type.
    sf::Sound& s = m_pool[m_nextSlot];
    m_nextSlot = (m_nextSlot + 1) % kPoolSize;
    return s;
}

void AudioSystem::playMusic(const std::string& path)
{
    // Graceful no-op if the file doesn't exist — music is optional.
    if (!m_music.openFromFile(path)) return;
    m_music.setVolume(35.f);
    m_music.setLoop(true);
    m_music.play();
}

void AudioSystem::stopMusic()
{
    m_music.stop();
}

} // namespace hollow
