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

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::CircleShape    m_body;
    sf::RectangleShape m_aimIndicator;
    sf::Vector2f       m_velocity{};
    float              m_aimAngle = 0.f;

    const InputState& m_input;
    const ActionMap&  m_actions;
};

} // namespace hollow
