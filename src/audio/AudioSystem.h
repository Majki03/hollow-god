#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <array>
#include <cstddef>
#include <string>

namespace hollow {

enum class Sfx {
    Swing,
    Hit,
    EnemyDeath,
    PlayerHit,
    PlayerDeath,
    Dash,
    BoonPick,
    WaveStart,
    Count_
};

// Central audio manager. All SFX are generated procedurally at startup so the
// game needs no audio asset files. Music is streamed from disk if present.
class AudioSystem {
public:
    AudioSystem();

    void play(Sfx sfx);
    void playMusic(const std::string& path);
    void stopMusic();

private:
    static constexpr std::size_t kPoolSize = 8;
    static constexpr std::size_t kSfxCount = static_cast<std::size_t>(Sfx::Count_);

    void       buildBuffers();
    sf::Sound& idleSlot();

    std::array<sf::SoundBuffer, kSfxCount> m_buffers;
    std::array<sf::Sound,       kPoolSize>  m_pool;
    std::size_t                             m_nextSlot = 0;
    sf::Music                               m_music;
};

} // namespace hollow
