#pragma once

#include "data/DataStore.h"
#include "entity/EnemyBase.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

// Basic melee enemy — chases the player at a constant speed.
class Grunt : public EnemyBase {
public:
    Grunt(sf::Vector2f position, const GruntStats& stats);

    void seek(sf::Vector2f playerPos) override;
    void render(sf::RenderTarget& target) const override;

protected:
    void      syncShape()               override { m_body.setPosition(m_position); }
    void      setBodyColor(sf::Color c) override { m_body.setFillColor(c); }
    void      setBodyScale(float s)     override { m_body.setScale(s, s); }
    sf::Color normalColor()             const override { return sf::Color(150, 40, 40); }

private:
    sf::RectangleShape m_body;
    GruntStats         m_stats;
};

} // namespace hollow
