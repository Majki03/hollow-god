#include "world/Room.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace hollow {

namespace {
    constexpr float kWallThickness = 24.f;
    const sf::Color kFloorColor    = sf::Color(28, 22, 36);
    const sf::Color kWallColor     = sf::Color(55, 42, 70);
    const sf::Color kWallEdge      = sf::Color(90, 68, 110);
}

Room::Room(sf::Vector2f topLeft, sf::Vector2f size)
    : m_topLeft(topLeft)
    , m_size(size)
{
    buildGeometry();
}

sf::Vector2f Room::clamp(sf::Vector2f point, float margin) const
{
    const float minX = m_topLeft.x + kWallThickness + margin;
    const float maxX = m_topLeft.x + m_size.x - kWallThickness - margin;
    const float minY = m_topLeft.y + kWallThickness + margin;
    const float maxY = m_topLeft.y + m_size.y - kWallThickness - margin;

    return {
        std::clamp(point.x, minX, maxX),
        std::clamp(point.y, minY, maxY),
    };
}

void Room::buildGeometry()
{
    const float x = m_topLeft.x;
    const float y = m_topLeft.y;
    const float w = m_size.x;
    const float h = m_size.y;
    const float t = kWallThickness;

    m_floor.setPosition({ x + t, y + t });
    m_floor.setSize({ w - 2.f * t, h - 2.f * t });
    m_floor.setFillColor(kFloorColor);

    auto makeWall = [&](sf::RectangleShape& wall,
                        sf::Vector2f pos, sf::Vector2f sz) {
        wall.setPosition(pos);
        wall.setSize(sz);
        wall.setFillColor(kWallColor);
        wall.setOutlineColor(kWallEdge);
        wall.setOutlineThickness(1.f);
    };

    makeWall(m_wallN, { x,         y         }, { w, t });
    makeWall(m_wallS, { x,         y + h - t }, { w, t });
    makeWall(m_wallW, { x,         y         }, { t, h });
    makeWall(m_wallE, { x + w - t, y         }, { t, h });
}

void Room::render(sf::RenderTarget& target) const
{
    target.draw(m_floor);
    target.draw(m_wallN);
    target.draw(m_wallS);
    target.draw(m_wallW);
    target.draw(m_wallE);
}

} // namespace hollow
