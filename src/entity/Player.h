#pragma once

#include "entity/Entity.h"
#include "entity/PlayerStats.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace hollow {

class ActionMap;
class InputState;

class Player : public Entity {
public:
    Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions);

    float aimAngle() const { return m_aimAngle; } // radians

    // Swing hitbox: a circle at an offset from the player. Only "active" for a
    // short window inside the swing — lets us animate a windup/recovery later
    // without the hitbox being alive for the whole animation.
    bool         hitboxActive() const;
    sf::Vector2f hitboxPosition() const;
    float        hitboxRadius() const;

    // Monotonically-increasing ID for each swing. Victims record which swing
    // hit them so the hitbox can't double-tap the same target in one swing.
    int swingId() const { return m_swingId; }

    // Damage the enemy deals to the player on body contact.
    static constexpr int kContactDamage = 10;

    int  hp()    const { return m_hp; }
    int  maxHp() const { return m_stats.maxHp; }

    // Dash cooldown queries (used by HUD).
    float dashCooldownRemaining() const { return m_dashCooldown; }
    float dashCooldownMax()       const { return m_stats.dashCooldown; }

    const PlayerStats& stats()        const { return m_stats; }
    PlayerStats&       mutableStats()       { return m_stats; }

    // Returns false while i-frames are active (hit ignored).
    bool damage(int amount);
    void healBy(int amount);

    void confine(sf::Vector2f min, sf::Vector2f max);

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    enum class AttackState { Idle, Swinging };

    PlayerStats        m_stats;
    sf::CircleShape    m_body;
    sf::RectangleShape m_aimIndicator;
    sf::VertexArray    m_swingArc;   // ring-sector drawn during attack

    sf::Vector2f m_velocity{};
    float        m_aimAngle    = 0.f;
    AttackState  m_attackState = AttackState::Idle;
    float        m_attackTimer = 0.f;
    int          m_swingId     = 0;

    int   m_hp           = 0;   // set to m_stats.maxHp in ctor body
    float m_iframeTimer  = 0.f;
    float m_dashCooldown = 0.f;

    const InputState& m_input;
    const ActionMap&  m_actions;
};

} // namespace hollow
