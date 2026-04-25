#include "entity/weapon/SwordWeapon.h"

#include "data/DataStore.h"
#include "input/ActionMap.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <numbers>

namespace hollow {

SwordWeapon::SwordWeapon(const SwordStats& stats)
    : m_stats(stats)
    , m_swingArc(sf::Triangles, static_cast<std::size_t>(kArcSegments * 6))
{
}

bool SwordWeapon::hitboxActive() const
{
    if (m_state != State::Swinging) return false;
    const float dur = m_isHeavy ? m_stats.swingDuration * 1.5f : m_stats.swingDuration;
    const float t = m_attackTimer / dur;
    return t >= kHitboxStart && t <= kHitboxEnd;
}

sf::Vector2f SwordWeapon::hitboxPosition() const
{
    return m_ownerPos + sf::Vector2f{
        std::cos(m_aimAngle) * m_stats.hitboxReach,
        std::sin(m_aimAngle) * m_stats.hitboxReach,
    };
}

float SwordWeapon::hitboxRadius() const
{
    float r = m_stats.hitboxRadius;
    if (m_isHeavy && m_state == State::Swinging) r *= m_stats.heavyRadiusMult;
    return r;
}

float SwordWeapon::damageMult() const
{
    return (m_isHeavy && m_state == State::Swinging) ? m_stats.heavyDamageMult : 1.f;
}

void SwordWeapon::update(float dt, sf::Vector2f ownerPos, float aimAngle,
                         const ActionMap& actions)
{
    m_ownerPos = ownerPos;
    m_aimAngle = aimAngle;

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
            if (m_holdTimer >= m_stats.heavyChargeTime) {
                // Held long enough — auto-fire the heavy slam.
                m_isHeavy     = true;
                m_attackTimer = 0.f;
                m_state       = State::Swinging;
                ++m_attackId;
            }
        } else {
            // Released before threshold — normal swing.
            m_isHeavy     = false;
            m_attackTimer = 0.f;
            m_state       = State::Swinging;
            ++m_attackId;
        }
        break;

    case State::Swinging: {
        const float dur = m_isHeavy ? m_stats.swingDuration * 1.5f : m_stats.swingDuration;
        m_attackTimer += dt;

        // Rebuild arc geometry. Heavy slam is wider and more saturated.
        {
            const float progress  = m_attackTimer / dur;
            const float alpha     = std::max(0.f, 1.f - progress) * 200.f;
            const float halfDeg   = m_isHeavy ? kHeavyArcHalfDeg : kArcHalfDeg;
            const float halfRad   = halfDeg * (std::numbers::pi_v<float> / 180.f);
            const float arcStep   = (2.f * halfRad) / kArcSegments;
            const float arcOuter  = m_isHeavy ? kHeavyArcOuter : kArcOuter;
            const sf::Color arcColor(
                m_isHeavy ? 255 : 240,
                m_isHeavy ? 100 : 200,
                m_isHeavy ? 40  : 120,
                static_cast<sf::Uint8>(alpha));

            std::size_t vi = 0;
            for (int s = 0; s < kArcSegments; ++s) {
                const float a0  = m_aimAngle - halfRad + s * arcStep;
                const float a1  = a0 + arcStep;
                const sf::Vector2f in0  = ownerPos + sf::Vector2f(std::cos(a0), std::sin(a0)) * kArcInner;
                const sf::Vector2f out0 = ownerPos + sf::Vector2f(std::cos(a0), std::sin(a0)) * arcOuter;
                const sf::Vector2f in1  = ownerPos + sf::Vector2f(std::cos(a1), std::sin(a1)) * kArcInner;
                const sf::Vector2f out1 = ownerPos + sf::Vector2f(std::cos(a1), std::sin(a1)) * arcOuter;

                m_swingArc[vi++] = { in0,  arcColor };
                m_swingArc[vi++] = { out0, arcColor };
                m_swingArc[vi++] = { out1, arcColor };
                m_swingArc[vi++] = { in0,  arcColor };
                m_swingArc[vi++] = { out1, arcColor };
                m_swingArc[vi++] = { in1,  arcColor };
            }
        }

        if (m_attackTimer >= dur) {
            m_attackTimer = 0.f;
            m_state       = State::Idle;
        }
        break;
    }
    }
}

void SwordWeapon::render(sf::RenderTarget& target) const
{
    if (m_state == State::Swinging)
        target.draw(m_swingArc);
}

} // namespace hollow
