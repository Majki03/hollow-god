#pragma once

#include "entity/EnemyBase.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

// Basic melee enemy — chases the player at a constant speed.
class Grunt : public EnemyBase {
public:
    explicit Grunt(sf::Vector2f position);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

protected:
    void      syncShape()            override { m_body.setPosition(m_position); }
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    sf::Color normalColor()          const override { return sf::Color(150, 40, 40); }

private:
    sf::RectangleShape m_body;

    static constexpr float kMoveSpeed = 110.f;
    static constexpr float kRadius    = 18.f;
    static constexpr int   kMaxHp     = 20;
};

} // namespace hollow
