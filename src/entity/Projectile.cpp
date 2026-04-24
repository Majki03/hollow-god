#include "entity/Projectile.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Projectile::Projectile(sf::Vector2f origin, sf::Vector2f direction, float speed)
    : Entity(origin)
    , m_body(kRadius)
    , m_dir(direction)
    , m_speed(speed)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(origin);
    m_body.setFillColor(sf::Color(240, 160, 60));
    m_body.setOutlineColor(sf::Color(180, 100, 20));
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
