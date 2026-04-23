#include "entity/Brute.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Brute::Brute(sf::Vector2f position)
    : EnemyBase(position, kRadius, kMaxHp)
    , m_body({ 2.f * kRadius, 2.f * kRadius })
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(30, 5, 20));
    m_body.setOutlineThickness(3.f);
}

void Brute::seek(sf::Vector2f playerPos)
{
    sf::Vector2f dir = playerPos - m_position;
    const float d2 = dir.x * dir.x + dir.y * dir.y;
    m_moveVel = (d2 > 0.f) ? dir * (kMoveSpeed / std::sqrt(d2)) : sf::Vector2f{};
}

void Brute::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
