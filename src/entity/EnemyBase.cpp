#include "entity/EnemyBase.h"

#include <algorithm>
#include <cmath>

namespace hollow {

EnemyBase::EnemyBase(sf::Vector2f position, float radius, int maxHp)
    : Entity(position)
    , m_radius(radius)
    , m_maxHp(maxHp)
    , m_hp(maxHp)
{}

void EnemyBase::damage(int amount)
{
    m_hp -= amount;
    m_flashTimer = kFlashDuration;
    if (m_hp <= 0) {
        kill();
    }
}

void EnemyBase::applyImpulse(sf::Vector2f impulse)
{
    m_knockback += impulse;
}

void EnemyBase::confine(sf::Vector2f mn, sf::Vector2f mx)
{
    if (m_position.x < mn.x) { m_position.x = mn.x; m_knockback.x = std::max(m_knockback.x, 0.f); }
    if (m_position.x > mx.x) { m_position.x = mx.x; m_knockback.x = std::min(m_knockback.x, 0.f); }
    if (m_position.y < mn.y) { m_position.y = mn.y; m_knockback.y = std::max(m_knockback.y, 0.f); }
    if (m_position.y > mx.y) { m_position.y = mx.y; m_knockback.y = std::min(m_knockback.y, 0.f); }
    syncShape();
}

void EnemyBase::update(float dt)
{
    if (m_flashTimer > 0.f) {
        m_flashTimer -= dt;
        setBodyColor(m_flashTimer > 0.f ? hitColor() : normalColor());
    }

    m_position += (m_knockback + m_moveVel) * dt;
    m_knockback *= std::exp2(-dt / kKnockbackHalfLife);

    syncShape();
}

} // namespace hollow
