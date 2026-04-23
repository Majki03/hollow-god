#include "entity/Enemy.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

namespace {
    const sf::Color kBaseFill  = sf::Color(150, 40, 40);
    const sf::Color kFlashFill = sf::Color(255, 230, 230);
}

Enemy::Enemy(sf::Vector2f position)
    : Entity(position)
    , m_body({ 2.f * kRadius, 2.f * kRadius })
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(kBaseFill);
    m_body.setOutlineColor(sf::Color(50, 10, 10));
    m_body.setOutlineThickness(2.f);
}

void Enemy::damage(int amount)
{
    m_hp -= amount;
    m_flashTimer = kFlashDuration;
    if (m_hp <= 0) {
        kill();
    }
}

void Enemy::seek(sf::Vector2f target)
{
    sf::Vector2f dir = target - m_position;
    const float d2 = dir.x * dir.x + dir.y * dir.y;
    m_moveVel = (d2 > 0.f) ? dir * (kMoveSpeed / std::sqrt(d2)) : sf::Vector2f{};
}

void Enemy::applyImpulse(sf::Vector2f impulse)
{
    m_knockback += impulse;
}

void Enemy::confine(sf::Vector2f mn, sf::Vector2f mx)
{
    if (m_position.x < mn.x) { m_position.x = mn.x; m_knockback.x = std::max(m_knockback.x, 0.f); }
    if (m_position.x > mx.x) { m_position.x = mx.x; m_knockback.x = std::min(m_knockback.x, 0.f); }
    if (m_position.y < mn.y) { m_position.y = mn.y; m_knockback.y = std::max(m_knockback.y, 0.f); }
    if (m_position.y > mx.y) { m_position.y = mx.y; m_knockback.y = std::min(m_knockback.y, 0.f); }
    m_body.setPosition(m_position);
}

void Enemy::update(float dt)
{
    if (m_flashTimer > 0.f) {
        m_flashTimer -= dt;
        m_body.setFillColor(m_flashTimer > 0.f ? kFlashFill : kBaseFill);
    }

    m_position += (m_knockback + m_moveVel) * dt;
    m_knockback *= std::exp2(-dt / kKnockbackHalfLife);

    m_body.setPosition(m_position);
}

void Enemy::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
}

} // namespace hollow
