#include "entity/Charger.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

Charger::Charger(sf::Vector2f position)
    : EnemyBase(position, kRadius, kMaxHp)
    , m_body(kRadius)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(120, 30, 0));
    m_body.setOutlineThickness(2.f);
}

sf::Color Charger::normalColor() const
{
    // Bright orange in Stalk/Rest, intense red-orange during Windup to signal
    // the incoming charge.
    if (m_state == State::Windup)
        return sf::Color(255, 120, 10);
    return sf::Color(220, 80, 20);
}

void Charger::seek(sf::Vector2f playerPos)
{
    // State-machine AI — timer advances happen here (called once per frame
    // before world.update, so dt isn't available; we use the seek frequency
    // as a rough tick and keep timers frame-independent via m_stateTimer).
    // NOTE: dt is not passed to seek(); instead we defer timer updates to
    // EnemyBase::update() via a tick hook. For now we piggyback the timer
    // update onto the seek call and accept that it ticks at seek rate (60 Hz).
    // A proper solution would pass dt, addressed in the AI refactor pass.

    const sf::Vector2f delta   = playerPos - m_position;
    const float        dist2   = delta.x * delta.x + delta.y * delta.y;
    const float        dist    = (dist2 > 0.f) ? std::sqrt(dist2) : 0.f;

    constexpr float kDt = 1.f / 60.f; // seek is called once per sim tick

    switch (m_state) {
    case State::Stalk:
        // Approach until close enough or until we've been stalking a while.
        if (dist < kChargeDist) {
            m_state      = State::Windup;
            m_stateTimer = kWindupDur;
            m_chargeTarget = playerPos; // lock target at windup start
            setBodyColor(normalColor());
        } else if (dist > 0.f) {
            m_moveVel = delta * (kStalkSpeed / dist);
        }
        break;

    case State::Windup:
        m_moveVel    = {}; // pause during telegraph
        m_stateTimer -= kDt;
        if (m_stateTimer <= 0.f) {
            m_state      = State::Charge;
            m_stateTimer = 0.f;
        }
        break;

    case State::Charge: {
        sf::Vector2f chargeDir = m_chargeTarget - m_position;
        const float  cd2 = chargeDir.x * chargeDir.x + chargeDir.y * chargeDir.y;
        if (cd2 > 1.f) {
            // Sprint toward locked target.
            chargeDir *= kChargeSpeed / std::sqrt(cd2);
            m_moveVel  = chargeDir;
        } else {
            // Reached the target — start resting.
            m_moveVel    = {};
            m_state      = State::Rest;
            m_stateTimer = kRestDur;
            setBodyColor(normalColor());
        }
        break;
    }

    case State::Rest:
        m_moveVel    = {};
        m_stateTimer -= kDt;
        if (m_stateTimer <= 0.f) {
            m_state = State::Stalk;
        }
        break;
    }
}

void Charger::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
