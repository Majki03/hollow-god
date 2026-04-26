#pragma once

#include "data/DataStore.h"
#include "entity/EnemyBase.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

namespace hollow {

// The Hollow Herald — a large boss enemy that appears alone on milestone waves.
//
// Attack cycle (timers run in parallel while Idle):
//   Slam:   1.6 s windup (expanding red ring telegraph) → AoE burst → brief rest
//   Volley: instant ring of volleyCount projectiles → brief pause
//
// GameScene polls slamActive(), slamRadius(), slamDamage() each frame to apply
// AoE damage, and calls takeVolley() when hasVolley() is true to spawn projectiles.
class BossEnemy : public EnemyBase {
public:
    BossEnemy(sf::Vector2f position, const BossStats& stats);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

    // ── Attack query interface (polled by GameScene) ──────────────────────────
    bool  slamActive()  const { return m_state == State::SlamActive; }
    float slamRadius()  const { return m_stats.slamRadius; }
    int   slamDamage()  const { return m_stats.slamDamage; }

    bool hasVolley() const { return m_pendingVolley; }
    // Returns unit vectors for each projectile direction; clears the flag.
    std::vector<sf::Vector2f> takeVolley();

protected:
    void      syncShape()               override;
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    void      setBodyScale(float s)     override { m_body.setScale(s, s); }
    sf::Color normalColor()             const override { return sf::Color(55, 20, 80); }
    sf::Color hitColor()                const override { return sf::Color(200, 160, 255); }

private:
    enum class State { Idle, SlamWindup, SlamActive, SlamRest, VolleyPause };

    BossStats    m_stats;
    State        m_state      = State::Idle;
    float        m_stateTimer = 0.f;
    float        m_slamCd     = 2.00f;   // countdown to next slam (initial delay)
    float        m_volleyCd   = 3.00f;   // countdown to next volley (initial delay)
    bool         m_pendingVolley = false;

    // Cached player position — stored in seek() for use in takeVolley().
    sf::Vector2f m_playerPos{};

    // ── Drawables ─────────────────────────────────────────────────────────────
    sf::CircleShape m_body;
    sf::CircleShape m_outerRing;     // ambient pulsing aura
    sf::CircleShape m_slamRing;      // telegraph ring (grows during SlamWindup)
    float           m_pulseTimer = 0.f;
    float           m_slamProgress = 0.f; // 0→1 over slamWindup
};

} // namespace hollow
