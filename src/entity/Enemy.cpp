#include "entity/Enemy.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace hollow {

Enemy::Enemy(sf::Vector2f position)
    : Entity(position)
    , m_body({ 2.f * kRadius, 2.f * kRadius })
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(sf::Color(150, 40, 40));
    m_body.setOutlineColor(sf::Color(50, 10, 10));
    m_body.setOutlineThickness(2.f);
}

void Enemy::damage(int amount)
{
    m_hp -= amount;
    if (m_hp <= 0) {
        kill();
    }
}

void Enemy::update(float /*dt*/)
{
    // Stationary dummy for now — real AI lands in Phase 5.
}

void Enemy::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
}

} // namespace hollow
