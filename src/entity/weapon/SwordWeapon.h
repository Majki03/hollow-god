#pragma once

#include "data/DataStore.h"
#include "entity/weapon/Weapon.h"

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// Melee arc weapon extracted from the original Player swing code.
//
// Primary: tap Attack → 0.30s swing arc, hitbox active 0.05–0.20s into the swing.
// Alt-fire: hold Attack ≥ 0.4s → heavy slam auto-fires (wider arc, 2× damage).
//
// Both attacks use PlayerStats::swingDamage; damageMult() returns 2.0 during
// the heavy slam so GameScene can scale correctly without knowing weapon internals.
class SwordWeapon : public Weapon {
public:
    explicit SwordWeapon(const SwordStats& stats);

    void update(float dt, sf::Vector2f ownerPos, float aimAngle,
                const ActionMap& actions) override;

    bool         hitboxActive()   const override;
    sf::Vector2f hitboxPosition() const override;
    float        hitboxRadius()   const override;
    float        damageMult()     const override;

    std::optional<PendingShot> pendingProjectile() override { return std::nullopt; }

    void render(sf::RenderTarget& target) const override;

private:
    enum class State { Idle, Charging, Swinging };

    SwordStats   m_stats;
    State        m_state       = State::Idle;
    float        m_attackTimer = 0.f;
    float        m_holdTimer   = 0.f;
    bool         m_isHeavy     = false;
    sf::Vector2f m_ownerPos{};
    float        m_aimAngle = 0.f;

    sf::VertexArray m_swingArc;

    static constexpr float kArcInner       = 20.f;
    static constexpr float kArcOuter       = 66.f;
    static constexpr float kHeavyArcOuter  = 90.f;
    static constexpr float kArcHalfDeg     = 52.f;
    static constexpr float kHeavyArcHalfDeg = 65.f;
    static constexpr int   kArcSegments    = 18;
    // Hitbox window as fractions of total swing duration.
    static constexpr float kHitboxStart    = 0.05f;
    static constexpr float kHitboxEnd      = 0.20f;
};

} // namespace hollow
