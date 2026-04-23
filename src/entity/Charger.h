#pragma once

#include "entity/EnemyBase.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// Fast enemy that stalks at moderate speed then sprints at the player in a
// short burst. The brief windup pause telegraphs the charge so the player
// has a window to dodge.
//
//  Stalk → Windup (0.35 s) → Charge (sprint to locked target) → Rest (0.55 s) → Stalk
class Charger : public EnemyBase {
public:
    explicit Charger(sf::Vector2f position);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

protected:
    void      syncShape()               override { m_body.setPosition(m_position); }
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    sf::Color normalColor()             const override;

private:
    enum class State { Stalk, Windup, Charge, Rest };

    State        m_state      = State::Stalk;
    float        m_stateTimer = 0.f;
    sf::Vector2f m_chargeTarget{};  // locked when windup begins

    sf::CircleShape m_body;

    static constexpr float kRadius      = 12.f;
    static constexpr int   kMaxHp       = 10;
    static constexpr float kStalkSpeed  = 75.f;
    static constexpr float kChargeSpeed = 310.f;
    static constexpr float kWindupDur   = 0.35f;
    static constexpr float kRestDur     = 0.55f;
    static constexpr float kChargeDist  = 280.f; // start charge within this range
};

} // namespace hollow
