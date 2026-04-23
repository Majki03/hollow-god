#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

class Enemy : public Entity {
public:
    explicit Enemy(sf::Vector2f position);

    void  damage(int amount);
    void  applyImpulse(sf::Vector2f impulse);
    void  confine(sf::Vector2f min, sf::Vector2f max);
    int   hp() const     { return m_hp; }
    float radius() const { return kRadius; }

    // Swing-id book-keeping so the same swing can't hit the same enemy twice.
    // -1 means "never hit". Owned by the enemy, not by the swinger, so dead
    // enemies don't leave dangling references in the player's state.
    int  lastHitSwing() const           { return m_lastHitSwing; }
    void setLastHitSwing(int swingId)   { m_lastHitSwing = swingId; }

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::RectangleShape m_body;
    sf::Vector2f       m_velocity{};
    int                m_hp            = kMaxHp;
    int                m_lastHitSwing  = -1;
    float              m_flashTimer    = 0.f;

    static constexpr int   kMaxHp             = 20;
    static constexpr float kRadius            = 18.f;
    static constexpr float kFlashDuration     = 0.08f;
    static constexpr float kKnockbackHalfLife = 0.10f; // velocity halves every 100ms
};

} // namespace hollow
