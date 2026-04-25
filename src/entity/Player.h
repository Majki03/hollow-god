#pragma once

#include "entity/Entity.h"
#include "entity/PlayerStats.h"
#include "entity/weapon/Weapon.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <memory>
#include <optional>

namespace hollow {

class ActionMap;
class InputState;

class Player : public Entity {
public:
    Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions);

    // Takes ownership of a weapon; replaces any previously equipped weapon.
    void setWeapon(std::unique_ptr<Weapon> w);

    float aimAngle() const { return m_aimAngle; }

    // Hitbox queries delegate to the equipped weapon.
    bool         hitboxActive()   const { return m_weapon && m_weapon->hitboxActive(); }
    sf::Vector2f hitboxPosition() const { return m_weapon ? m_weapon->hitboxPosition() : m_position; }
    float        hitboxRadius()   const { return m_weapon ? m_weapon->hitboxRadius() : 0.f; }

    // Increments each new attack — used by GameScene to detect attack edges for SFX.
    int swingId() const { return m_weapon ? m_weapon->attackId() : 0; }

    // Damage multiplier for this frame's hitbox (> 1 during heavy / alt-fire).
    float weaponDamageMult() const { return m_weapon ? m_weapon->damageMult() : 1.f; }

    // Non-null when the weapon fired a projectile this frame. Consuming it
    // resets the weapon's internal state, so call at most once per frame.
    std::optional<Weapon::PendingShot> pendingWeaponProjectile()
        { return m_weapon ? m_weapon->pendingProjectile() : std::nullopt; }

    // Damage the enemy deals to the player on body / projectile contact.
    static constexpr int kContactDamage = 10;

    int  hp()    const { return m_hp; }
    int  maxHp() const { return m_stats.maxHp; }

    float dashCooldownRemaining() const { return m_dashCooldown; }
    float dashCooldownMax()       const { return m_stats.dashCooldown; }

    // True for exactly one frame after a dash fires; read and consumed by GameScene.
    bool consumeDashEvent() { if (!m_dashedThisFrame) return false; m_dashedThisFrame = false; return true; }
    sf::Vector2f dashOrigin() const { return m_dashOrigin; }

    const PlayerStats& stats()        const { return m_stats; }
    PlayerStats&       mutableStats()       { return m_stats; }

    // Returns false while i-frames are active (hit ignored).
    bool damage(int amount);
    void healBy(int amount);

    // Extend i-frame window to at least `duration` seconds (e.g. Soul Drain on kill).
    void extendIframes(float duration) { m_iframeTimer = std::max(m_iframeTimer, duration); }

    // Reduce current HP by amount, clamped to 1 (cannot kill, for curse/boon costs).
    void loseHpDirect(int amount) { m_hp = std::max(1, m_hp - amount); }

    void confine(sf::Vector2f min, sf::Vector2f max);

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    PlayerStats        m_stats;
    sf::CircleShape    m_body;
    sf::RectangleShape m_aimIndicator;

    sf::Vector2f m_velocity{};
    float        m_aimAngle = 0.f;

    int          m_hp             = 0;
    float        m_iframeTimer   = 0.f;
    float        m_dashCooldown  = 0.f;
    bool         m_dashedThisFrame = false;
    sf::Vector2f m_dashOrigin{};

    std::unique_ptr<Weapon> m_weapon;

    float m_echoStepTimer = 0.f; // remaining duration of Echo Step speed boost

    const InputState& m_input;
    const ActionMap&  m_actions;
};

} // namespace hollow
