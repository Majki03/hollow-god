#include "entity/weapon/BowWeapon.h"

#include "input/ActionMap.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

BowWeapon::BowWeapon(const BowStats& stats)
    : m_stats(stats)
    , m_chargeRing(8.f)
{
    m_chargeRing.setOrigin(8.f, 8.f);
    m_chargeRing.setFillColor(sf::Color::Transparent);
    m_chargeRing.setOutlineThickness(2.f);
}

void BowWeapon::fireShot(bool charged)
{
    PendingShot shot;
    shot.direction = { std::cos(m_aimAngle), std::sin(m_aimAngle) };
    shot.speed     = m_stats.projectileSpeed;
    shot.damage    = static_cast<int>(
        m_stats.baseDamage * (charged ? m_stats.chargeDamageMult : 1.f));
    shot.pierces   = charged;
    m_pending      = shot;
    ++m_attackId;
}

std::optional<Weapon::PendingShot> BowWeapon::pendingProjectile()
{
    auto shot = m_pending;
    m_pending.reset();
    return shot;
}

void BowWeapon::update(float dt, sf::Vector2f ownerPos, float aimAngle,
                       const ActionMap& actions)
{
    m_ownerPos  = ownerPos;
    m_aimAngle  = aimAngle;
    m_showCharge = false;

    const bool attackDown = actions.isDown(Action::Attack);

    switch (m_state) {
    case State::Idle:
        if (actions.justPressed(Action::Attack)) {
            m_state     = State::Charging;
            m_holdTimer = 0.f;
        }
        break;

    case State::Charging:
        if (attackDown) {
            m_holdTimer += dt;
            m_showCharge = true;

            if (m_holdTimer >= m_stats.chargeTime) {
                // Full charge reached — auto-fire charged shot.
                fireShot(true);
                m_state     = State::Cooldown;
                m_coolTimer = m_stats.cooldown;
            }
        } else {
            // Released early — fire normal arrow.
            fireShot(false);
            m_state     = State::Cooldown;
            m_coolTimer = m_stats.cooldown;
        }
        break;

    case State::Cooldown:
        m_coolTimer -= dt;
        if (m_coolTimer <= 0.f)
            m_state = State::Idle;
        break;
    }

    // Scale charge ring so it grows from player radius to full over charge time.
    if (m_showCharge) {
        const float t = std::min(m_holdTimer / m_stats.chargeTime, 1.f);
        const float r = 14.f + t * 18.f;  // 14px (body radius) → 32px at full charge
        m_chargeRing.setRadius(r);
        m_chargeRing.setOrigin(r, r);
        m_chargeRing.setPosition(ownerPos);
        // Colour shifts cyan → white as charge builds.
        const sf::Uint8 g = static_cast<sf::Uint8>(150 + t * 100);
        m_chargeRing.setOutlineColor(sf::Color(100, g, 255, 200));
    }
}

void BowWeapon::render(sf::RenderTarget& target) const
{
    if (m_showCharge)
        target.draw(m_chargeRing);
}

} // namespace hollow
