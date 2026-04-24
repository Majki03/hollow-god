#pragma once

#include "entity/EnemyBase.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <optional>

namespace hollow {

// Ranged enemy. Tries to maintain distance from the player and fires a
// projectile at them every kFireInterval seconds.
//
// GameScene polls pendingShot() each frame. If it returns a direction
// vector, the scene spawns a Projectile and clears the pending shot.
class Archer : public EnemyBase {
public:
    explicit Archer(sf::Vector2f position);

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
    sf::CircleShape m_body;
    sf::Vector2f    m_lastPlayerDir{};  // updated by seek() each frame
    float           m_fireTimer   = 0.f;
    bool            m_wantsToFire = false;

    static constexpr float kRadius       = 13.f;
    static constexpr int   kMaxHp        = 18;
    static constexpr float kMoveSpeed    = 65.f;
    static constexpr float kPreferredDist = 290.f;  // target distance from player
    static constexpr float kFireInterval  = 2.2f;
};

} // namespace hollow
