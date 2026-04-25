#pragma once

#include "data/DataStore.h"
#include "entity/weapon/Weapon.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// Ranged weapon. No melee hitbox — all damage goes through projectiles.
//
// Primary (tap): fires a single arrow on release (0.6s cooldown).
// Alt-fire (hold ≥ 1.2s): charged shot auto-fires — pierces all enemies,
// 1.5× damage, visibly larger. Cooldown applies to both modes.
//
// The bow taps Action::Attack for primary and holds it for charge.
// Projectile damage = BowStats::baseDamage (×1.5 when charged).
// No interaction with PlayerStats::swingDamage — boon flavour text says
// "swing" for a reason.
class BowWeapon : public Weapon {
public:
    explicit BowWeapon(const BowStats& stats);

    void update(float dt, sf::Vector2f ownerPos, float aimAngle,
                const ActionMap& actions) override;

    bool         hitboxActive()   const override { return false; }
    sf::Vector2f hitboxPosition() const override { return m_ownerPos; }
    float        hitboxRadius()   const override { return 0.f; }

    std::optional<PendingShot> pendingProjectile() override;

    void render(sf::RenderTarget& target) const override;

private:
    enum class State { Idle, Charging, Cooldown };

    void fireShot(bool charged);

    BowStats     m_stats;
    State        m_state      = State::Idle;
    float        m_holdTimer  = 0.f;
    float        m_coolTimer  = 0.f;
    sf::Vector2f m_ownerPos{};
    float        m_aimAngle   = 0.f;

    std::optional<PendingShot> m_pending;

    // Charge progress indicator: ring that grows while holding.
    sf::CircleShape m_chargeRing;
    bool            m_showCharge = false;
};

} // namespace hollow
