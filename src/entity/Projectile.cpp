#include "entity/Projectile.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Projectile::Projectile(sf::Vector2f origin, sf::Vector2f direction,
                       float speed, int damage, bool pierces,
                       sf::Color color, float radius)
    : Entity(origin)
    , m_body(radius)
    , m_dir(direction)
    , m_speed(speed)
    , m_radius(radius)
    , m_damage(damage)
    , m_pierces(pierces)
{
    m_body.setOrigin(radius, radius);
    m_body.setPosition(origin);
    m_body.setFillColor(color);
    m_body.setOutlineColor(sf::Color(
        static_cast<sf::Uint8>(color.r * 0.7f),
        static_cast<sf::Uint8>(color.g * 0.7f),
        static_cast<sf::Uint8>(color.b * 0.7f)));
    m_body.setOutlineThickness(1.5f);
}

void Projectile::update(float dt)
{
    const float step = m_speed * dt;
    m_position += m_dir * step;
    m_traveled += step;
    m_body.setPosition(m_position);

    if (m_traveled >= kMaxRange) kill();
}

void Projectile::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
}

} // namespace hollow
