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

    const InputState& m_input;
    const ActionMap&  m_actions;
};

} // namespace hollow
