#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/CircleShape.hpp>

namespace hollow {

class ActionMap;

class Player : public Entity {
public:
    Player(sf::Vector2f startPosition, const ActionMap& actions);

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::CircleShape  m_body;
    sf::Vector2f     m_velocity{};
    const ActionMap& m_actions;
};

} // namespace hollow
