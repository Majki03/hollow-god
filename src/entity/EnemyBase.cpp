#include "entity/EnemyBase.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>

namespace hollow {

EnemyBase::EnemyBase(sf::Vector2f position, float radius, int maxHp)
    : Entity(position)
    , m_radius(radius)
    , m_maxHp(maxHp)
    , m_hp(maxHp)
{}

void EnemyBase::scaleHp(float factor)
{
    m_maxHp = std::max(1, static_cast<int>(m_maxHp * factor));
    m_hp    = m_maxHp;
}

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

void EnemyBase::renderHpBar(sf::RenderTarget& target) const
{
    // Skip if at full health — no need to clutter the screen.
    if (m_hp >= m_maxHp) return;

    constexpr float kBarW  = 36.f;
    constexpr float kBarH  =  4.f;
    constexpr float kYOff  =  6.f; // above the entity's top edge

    const float ratio = static_cast<float>(m_hp) / static_cast<float>(m_maxHp);
    const float y     = m_position.y - m_radius - kYOff - kBarH;
    const float x     = m_position.x - kBarW * 0.5f;

    sf::RectangleShape bg({ kBarW, kBarH });
    bg.setPosition({ x, y });
    bg.setFillColor(sf::Color(20, 10, 10, 200));
    target.draw(bg);

    sf::RectangleShape fill({ kBarW * ratio, kBarH });
    fill.setPosition({ x, y });
    fill.setFillColor(sf::Color(200, 50, 50));
    target.draw(fill);
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

    // Spawn pop: scale from 0 to 1 with a brief overshoot.
    if (m_spawnTimer < kSpawnDuration) {
        m_spawnTimer += dt;
        const float t = std::min(m_spawnTimer / kSpawnDuration, 1.f);
        float scale;
        constexpr float kPeak = 0.65f;
        if (t < kPeak) scale = (t / kPeak) * 1.25f;
        else           scale = 1.25f - 0.25f * ((t - kPeak) / (1.f - kPeak));
        setBodyScale(scale);
    }
}

} // namespace hollow
