#pragma once

#include <SFML/System/Vector2.hpp>

#include <optional>

namespace sf { class RenderTarget; }

namespace hollow {

class ActionMap;

enum class WeaponType { Sword, Bow, Spear };

// Abstract weapon. Player owns one via unique_ptr; GameScene queries hitbox
// and projectile state from Player, which delegates here.
//
// Design: the weapon owns all attack animation and input logic so Player stays
// focused on movement, dash, and health. Damage values live in PlayerStats;
// damageMult() modifies them for charged / alt-fire attacks.
class Weapon {
public:
    virtual ~Weapon() = default;

    virtual void update(float dt, sf::Vector2f ownerPos, float aimAngle,
                        const ActionMap& actions) = 0;

    virtual bool         hitboxActive()   const = 0;
    virtual sf::Vector2f hitboxPosition() const = 0;
    virtual float        hitboxRadius()   const = 0;

    // Multiplier applied to PlayerStats::swingDamage when resolving hits.
    // Returns > 1 for charged / alt-fire melee attacks.
    virtual float damageMult() const { return 1.f; }

    // Non-null when this weapon just fired a projectile. The weapon resets to
    // nullopt internally after returning the value so each shot is consumed once.
    struct PendingShot {
        sf::Vector2f direction;
        float        speed;
        int          damage;
        bool         pierces = false;
    };
    virtual std::optional<PendingShot> pendingProjectile() = 0;

    virtual void render(sf::RenderTarget& target) const = 0;

    // Monotonically increments on each new attack. Mirrors Player::swingId()
    // so GameScene can detect attack edges for SFX without touching internals.
    int attackId() const { return m_attackId; }

protected:
    int m_attackId = 0;
};

} // namespace hollow
