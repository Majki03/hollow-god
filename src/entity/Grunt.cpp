#include "entity/Grunt.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Grunt::Grunt(sf::Vector2f position, const GruntStats& stats)
    : EnemyBase(position, stats.radius, stats.maxHp)
    , m_body({ 2.f * stats.radius, 2.f * stats.radius })
    , m_stats(stats)
{
    m_body.setOrigin(stats.radius, stats.radius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(50, 10, 10));
    m_body.setOutlineThickness(2.f);
}

void Grunt::seek(sf::Vector2f playerPos)
{
    sf::Vector2f dir = playerPos - m_position;
    const float d2 = dir.x * dir.x + dir.y * dir.y;
    m_moveVel = (d2 > 0.f) ? dir * (m_stats.moveSpeed / std::sqrt(d2)) : sf::Vector2f{};
}

void Grunt::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
