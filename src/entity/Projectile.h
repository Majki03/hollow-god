#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// A slow-moving projectile fired by the Archer enemy. Travels in a fixed
// direction until it hits the player, reaches max range, or leaves the room.
class Projectile : public Entity {
public:
    Projectile(sf::Vector2f origin, sf::Vector2f direction, float speed = 200.f);

    float        radius() const { return kRadius; }
    sf::Vector2f travelDir() const { return m_dir; }

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::CircleShape m_body;
    sf::Vector2f    m_dir;
    float           m_speed;
    float           m_traveled = 0.f;

    static constexpr float kRadius   = 5.f;
    static constexpr float kMaxRange = 680.f;
};

} // namespace hollow
