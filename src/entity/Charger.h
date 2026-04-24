#pragma once

#include "data/DataStore.h"
#include "entity/EnemyBase.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// Fast enemy that stalks at moderate speed then sprints at the player in a
// short burst. The brief windup pause telegraphs the charge so the player
// has a window to dodge.
//
//  Stalk → Windup → Charge (sprint to locked target) → Rest → Stalk
class Charger : public EnemyBase {
public:
    Charger(sf::Vector2f position, const ChargerStats& stats);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

protected:
    void      syncShape()               override { m_body.setPosition(m_position); }
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    void      setBodyScale(float s)     override { m_body.setScale(s, s); }
    sf::Color normalColor()             const override;

private:
    enum class State { Stalk, Windup, Charge, Rest };

    ChargerStats m_stats;
    State        m_state      = State::Stalk;
    float        m_stateTimer = 0.f;
    sf::Vector2f m_chargeTarget{};  // locked when windup begins

    sf::CircleShape m_body;
};

} // namespace hollow
