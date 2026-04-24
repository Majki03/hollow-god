#include "entity/Brute.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Brute::Brute(sf::Vector2f position, const BruteStats& stats)
    : EnemyBase(position, stats.radius, stats.maxHp)
    , m_body({ 2.f * stats.radius, 2.f * stats.radius })
    , m_stats(stats)
{
    m_body.setOrigin(stats.radius, stats.radius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(30, 5, 20));
    m_body.setOutlineThickness(3.f);
}

void Brute::seek(sf::Vector2f playerPos)
{
    sf::Vector2f dir = playerPos - m_position;
    const float d2 = dir.x * dir.x + dir.y * dir.y;
    m_moveVel = (d2 > 0.f) ? dir * (m_stats.moveSpeed / std::sqrt(d2)) : sf::Vector2f{};
}

void Brute::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
