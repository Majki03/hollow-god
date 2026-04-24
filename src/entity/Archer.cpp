#include "entity/Archer.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Archer::Archer(sf::Vector2f position)
    : EnemyBase(position, kRadius, kMaxHp)
    , m_body(kRadius)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(20, 60, 100));
    m_body.setOutlineThickness(2.f);
    // Stagger first shot so multiple archers don't volley simultaneously.
    m_fireTimer = kFireInterval * 0.5f;
}

void Archer::seek(sf::Vector2f playerPos)
{
    const sf::Vector2f delta = playerPos - m_position;
    const float dist2 = delta.x * delta.x + delta.y * delta.y;
    if (dist2 < 1.f) return;

    const float dist       = std::sqrt(dist2);
    const sf::Vector2f dir = delta / dist;
    m_lastPlayerDir        = dir;

    // Maintain preferred range — advance if too far, retreat if too close.
    if (dist > kPreferredDist + 40.f) {
        m_moveVel = dir * kMoveSpeed;
    } else if (dist < kPreferredDist - 40.f) {
        m_moveVel = dir * (-kMoveSpeed * 0.65f);
    } else {
        m_moveVel = {};
    }
}

void Archer::update(float dt)
{
    EnemyBase::update(dt);

    m_fireTimer -= dt;
    if (m_fireTimer <= 0.f) {
        m_fireTimer   = kFireInterval;
        m_wantsToFire = true;
    }
}

std::optional<sf::Vector2f> Archer::pendingShot()
{
    if (!m_wantsToFire || !alive()) return std::nullopt;
    m_wantsToFire = false;
    return m_lastPlayerDir;
}

void Archer::syncShape()
{
    m_body.setPosition(m_position);
}

void Archer::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
