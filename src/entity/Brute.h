#pragma once

#include "entity/EnemyBase.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

// Slow, tanky enemy. Takes many hits to kill and deals heavy contact damage.
// Its size makes it hard to dodge in tight spaces.
class Brute : public EnemyBase {
public:
    explicit Brute(sf::Vector2f position);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

protected:
    void      syncShape()               override { m_body.setPosition(m_position); }
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    void      setBodyScale(float s)     override { m_body.setScale(s, s); }
    sf::Color normalColor()             const override { return sf::Color(90, 20, 60); }
    sf::Color hitColor()                const override { return sf::Color(255, 200, 240); }

private:
    sf::RectangleShape m_body;

    static constexpr float kRadius    = 28.f;
    static constexpr int   kMaxHp     = 50;
    static constexpr float kMoveSpeed = 60.f;
};

} // namespace hollow
