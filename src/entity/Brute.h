#pragma once

#include "data/DataStore.h"
#include "entity/EnemyBase.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

// Slow, tanky enemy. Takes many hits to kill and deals heavy contact damage.
// Its size makes it hard to dodge in tight spaces.
class Brute : public EnemyBase {
public:
    Brute(sf::Vector2f position, const BruteStats& stats);

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
    BruteStats         m_stats;
};

} // namespace hollow
