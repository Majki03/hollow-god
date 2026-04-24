#pragma once

#include "data/DataStore.h"
#include "entity/EnemyBase.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>

namespace hollow {

// Ranged enemy. Tries to maintain distance from the player and fires a
// projectile at them every stats.fireInterval seconds.
//
// GameScene polls pendingShot() each frame. If it returns a direction
// vector, the scene spawns a Projectile and clears the pending shot.
class Archer : public EnemyBase {
public:
    Archer(sf::Vector2f position, const ArcherStats& stats);

    void seek(sf::Vector2f playerPos) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

    // Returns the normalised fire direction if a shot is ready, then clears it.
    std::optional<sf::Vector2f> pendingShot();

protected:
    void      syncShape()               override;
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    void      setBodyScale(float s)     override { m_body.setScale(s, s); }
    sf::Color normalColor()             const override { return sf::Color(60, 130, 180); }
    sf::Color hitColor()                const override { return sf::Color(180, 230, 255); }

private:
    ArcherStats  m_stats;
    sf::CircleShape m_body;
    sf::Vector2f    m_lastPlayerDir{};  // updated by seek() each frame
    float           m_fireTimer   = 0.f;
    bool            m_wantsToFire = false;
};

} // namespace hollow
