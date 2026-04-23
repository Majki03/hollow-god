#include "entity/Grunt.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Grunt::Grunt(sf::Vector2f position)
    : EnemyBase(position, kRadius, kMaxHp)
    , m_body({ 2.f * kRadius, 2.f * kRadius })
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(50, 10, 10));
    m_body.setOutlineThickness(2.f);
}

void Grunt::seek(sf::Vector2f playerPos)
{
    sf::Vector2f dir = playerPos - m_position;
    const float d2 = dir.x * dir.x + dir.y * dir.y;
    m_moveVel = (d2 > 0.f) ? dir * (kMoveSpeed / std::sqrt(d2)) : sf::Vector2f{};
}

void Grunt::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
