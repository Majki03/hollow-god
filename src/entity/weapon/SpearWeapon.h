#pragma once

#include "data/DataStore.h"
#include "entity/weapon/Weapon.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace hollow {

// Long-reach melee weapon with a thrown alt-fire.
//
// Primary (tap Attack): short forward thrust, rectangle hitbox extending
// hitboxLength px in the aim direction. Pierces all enemies in the line
// (all overlapping get hit in one thrust — same as the existing melee logic).
//
// Alt-fire (Interact / E): throws the spear as a piercing projectile.
// Player is unarmed for unarmedDuration seconds; hitboxActive() returns false
// during this time. The spear "returns" automatically when the timer expires.
//
// Hitbox is approximated as a circle (center at midpoint of the spear,
// radius = diagonal/2) so GameScene's existing circle-overlap code works.
class SpearWeapon : public Weapon {
public:
    explicit SpearWeapon(const SpearStats& stats);

    void update(float dt, sf::Vector2f ownerPos, float aimAngle,
                const ActionMap& actions) override;

    bool         hitboxActive()   const override;
    sf::Vector2f hitboxPosition() const override;
    float        hitboxRadius()   const override;

    std::optional<PendingShot> pendingProjectile() override;

    void render(sf::RenderTarget& target) const override;

private:
    enum class State { Idle, Thrusting, Thrown };

    SpearStats   m_stats;
    State        m_state       = State::Idle;
    float        m_stateTimer  = 0.f;
    sf::Vector2f m_ownerPos{};
    float        m_aimAngle    = 0.f;

    std::optional<PendingShot> m_pending;

    sf::RectangleShape m_spearShape;  // visual during thrust/idle

    // Hitbox radius: circle bounding the rectangle.
    float hitboxR() const;
};

} // namespace hollow
