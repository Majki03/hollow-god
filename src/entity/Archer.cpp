#include "entity/Archer.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Archer::Archer(sf::Vector2f position, const ArcherStats& stats)
    : EnemyBase(position, stats.radius, stats.maxHp)
    , m_stats(stats)
    , m_body(stats.radius)
{
    m_body.setOrigin(stats.radius, stats.radius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(20, 60, 100));
    m_body.setOutlineThickness(2.f);
    // Stagger first shot so multiple archers don't volley simultaneously.
    m_fireTimer = m_stats.fireInterval * 0.5f;
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
    if (dist > m_stats.preferredDist + 40.f) {
        m_moveVel = dir * m_stats.moveSpeed;
    } else if (dist < m_stats.preferredDist - 40.f) {
        m_moveVel = dir * (-m_stats.moveSpeed * 0.65f);
    } else {
        m_moveVel = {};
    }
}

void Archer::update(float dt)
{
    EnemyBase::update(dt);

    m_fireTimer -= dt;
    if (m_fireTimer <= 0.f) {
        m_fireTimer   = m_stats.fireInterval;
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
