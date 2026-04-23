#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

class ActionMap;
class InputState;

class Player : public Entity {
public:
    Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions);

    float aimAngle() const { return m_aimAngle; } // radians

    // Swing hitbox: a circle at an offset from the player. Only "active" for a
    // short window inside the swing — lets us animate a windup/recovery later
    // without the hitbox being alive for the whole animation.
    bool         hitboxActive() const;
    sf::Vector2f hitboxPosition() const;
    float        hitboxRadius() const;

    // Monotonically-increasing ID for each swing. Victims record which swing
    // hit them so the hitbox can't double-tap the same target in one swing.
    int swingId() const { return m_swingId; }

    static constexpr int kContactDamage = 10;

    int  hp()    const { return m_hp; }
    int  maxHp() const { return kMaxHp; }

    // Returns false while i-frames are active so contact damage can be gated.
    bool damage(int amount);

    // Clamp the player inside [min, max] and zero any velocity component that
    // pushed against a wall so movement doesn't accumulate into the boundary.
    void confine(sf::Vector2f min, sf::Vector2f max);

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    enum class AttackState { Idle, Swinging };

    sf::CircleShape    m_body;
    sf::RectangleShape m_aimIndicator;
    sf::RectangleShape m_swingVisual;

    sf::Vector2f m_velocity{};
    float        m_aimAngle    = 0.f;
    AttackState  m_attackState = AttackState::Idle;
    float        m_attackTimer = 0.f;
    int          m_swingId     = 0;

    int   m_hp          = kMaxHp;
    float m_iframeTimer = 0.f;

    static constexpr int   kMaxHp     = 100;
    static constexpr float kIframeDur = 0.75f;

    const InputState& m_input;
    const ActionMap&  m_actions;
};

} // namespace hollow
