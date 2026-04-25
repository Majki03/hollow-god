#include "entity/weapon/SpearWeapon.h"

#include "input/ActionMap.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <numbers>

namespace hollow {

SpearWeapon::SpearWeapon(const SpearStats& stats)
    : m_stats(stats)
    , m_spearShape({ stats.hitboxLength, stats.hitboxWidth })
{
    // Origin at the left edge, centred vertically — matches aim-direction offset.
    m_spearShape.setOrigin(0.f, stats.hitboxWidth * 0.5f);
    m_spearShape.setFillColor(sf::Color(180, 190, 200, 200));
    m_spearShape.setOutlineColor(sf::Color(100, 120, 150));
    m_spearShape.setOutlineThickness(1.5f);
}

float SpearWeapon::hitboxR() const
{
    // Circle bounding the hitbox rectangle: half-diagonal.
    const float hw = m_stats.hitboxLength * 0.5f;
    const float hh = m_stats.hitboxWidth  * 0.5f;
    return std::sqrt(hw * hw + hh * hh);
}

bool SpearWeapon::hitboxActive() const
{
    return m_state == State::Thrusting;
}

sf::Vector2f SpearWeapon::hitboxPosition() const
{
    // Centre of the spear rectangle, in front of the player.
    return m_ownerPos + sf::Vector2f{
        std::cos(m_aimAngle) * m_stats.hitboxLength * 0.5f,
        std::sin(m_aimAngle) * m_stats.hitboxLength * 0.5f,
    };
}

float SpearWeapon::hitboxRadius() const
{
    return hitboxR();
}

std::optional<Weapon::PendingShot> SpearWeapon::pendingProjectile()
{
    auto shot = m_pending;
    m_pending.reset();
    return shot;
}

void SpearWeapon::update(float dt, sf::Vector2f ownerPos, float aimAngle,
                         const ActionMap& actions)
{
    m_ownerPos = ownerPos;
    m_aimAngle = aimAngle;

    switch (m_state) {
    case State::Idle:
        if (actions.justPressed(Action::Attack)) {
            m_state      = State::Thrusting;
            m_stateTimer = 0.f;
            ++m_attackId;
        }
        // Interact (E) throws the spear.
        if (actions.justPressed(Action::Interact)) {
            PendingShot shot;
            shot.direction = { std::cos(aimAngle), std::sin(aimAngle) };
            shot.speed     = m_stats.throwSpeed;
            shot.damage    = m_stats.baseDamage;
            shot.pierces   = true;
            m_pending      = shot;
            m_state        = State::Thrown;
            m_stateTimer   = 0.f;
            ++m_attackId;
        }
        break;

    case State::Thrusting:
        m_stateTimer += dt;
        if (m_stateTimer >= m_stats.thrustDuration) {
            m_state      = State::Idle;
            m_stateTimer = 0.f;
        }
        break;

    case State::Thrown:
        // Player is unarmed until the spear returns.
        m_stateTimer += dt;
        if (m_stateTimer >= m_stats.unarmedDuration) {
            m_state      = State::Idle;
            m_stateTimer = 0.f;
        }
        break;
    }

    // Update visual position and rotation.
    if (m_state != State::Thrown) {
        constexpr float kRad2Deg = 180.f / std::numbers::pi_v<float>;
        m_spearShape.setPosition(ownerPos);
        m_spearShape.setRotation(aimAngle * kRad2Deg);

        const float alpha = (m_state == State::Thrusting) ? 220.f : 140.f;
        sf::Color c = m_spearShape.getFillColor();
        c.a = static_cast<sf::Uint8>(alpha);
        m_spearShape.setFillColor(c);
    }
}

void SpearWeapon::render(sf::RenderTarget& target) const
{
    if (m_state != State::Thrown)
        target.draw(m_spearShape);
}

} // namespace hollow
