#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

// Abstract base for all enemy types. Owns the shared physics (knockback + seek
// velocity integration), HP, flash timer, and wall-confinement logic. Subclasses
// supply visuals and AI via the four pure-virtual hooks below.
class EnemyBase : public Entity {
public:
    EnemyBase(sf::Vector2f position, float radius, int maxHp);

    void damage(int amount);
    void applyImpulse(sf::Vector2f impulse);
    void confine(sf::Vector2f min, sf::Vector2f max);

    int   hp()    const { return m_hp; }
    int   maxHp() const { return m_maxHp; }
    float radius() const { return m_radius; }

    int  lastHitSwing() const        { return m_lastHitSwing; }
    void setLastHitSwing(int swingId) { m_lastHitSwing = swingId; }

    // Returns the enemy's base body colour — used by the particle system on death.
    virtual sf::Color normalColor() const = 0;

    // Called by GameScene before world.update() — subclass sets m_moveVel.
    virtual void seek(sf::Vector2f playerPos) = 0;

    void update(float dt) override; // integrates physics, then calls the hooks

    // Draws a small HP bar centred above the enemy body.
    void renderHpBar(sf::RenderTarget& target) const;

protected:
    // Sync the subclass's drawable to the current m_position.
    virtual void syncShape() = 0;
    // Apply `c` to the subclass's drawable (used by the flash system).
    virtual void setBodyColor(sf::Color c) = 0;

    virtual sf::Color hitColor() const { return sf::Color(255, 230, 230); }

    float        m_radius;
    int          m_maxHp;
    int          m_hp;
    sf::Vector2f m_knockback{};
    sf::Vector2f m_moveVel{};   // set fresh by seek() each frame
    int          m_lastHitSwing = -1;
    float        m_flashTimer   = 0.f;

    static constexpr float kKnockbackHalfLife = 0.10f;
    static constexpr float kFlashDuration     = 0.08f;
};

} // namespace hollow
