#include "world/Room.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace hollow {

namespace {
    const sf::Color kFloorColor    = sf::Color(28, 22, 36);
    const sf::Color kWallColor     = sf::Color(55, 42, 70);
    const sf::Color kWallEdge      = sf::Color(90, 68, 110);
    const sf::Color kGridColor     = sf::Color(48, 38, 60, 90);  // subtle purple tint
    constexpr float kTileSize      = 80.f;
}

Room::Room(sf::Vector2f topLeft, sf::Vector2f size)
    : m_topLeft(topLeft)
    , m_size(size)
{
    buildGeometry();
}

sf::Vector2f Room::clamp(sf::Vector2f point, float margin) const
{
    const float minX = m_topLeft.x + kWallThick + margin;
    const float maxX = m_topLeft.x + m_size.x - kWallThick - margin;
    const float minY = m_topLeft.y + kWallThick + margin;
    const float maxY = m_topLeft.y + m_size.y - kWallThick - margin;

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
    const float t = kWallThick;

    m_floor.setPosition({ x + t, y + t });
    m_floor.setSize({ w - 2.f * t, h - 2.f * t });
    m_floor.setFillColor(kFloorColor);

    // Build grid lines spanning the inner floor area.
    {
        const float fx0 = x + t;
        const float fy0 = y + t;
        const float fx1 = x + w - t;
        const float fy1 = y + h - t;

        m_grid.clear();
        m_grid.setPrimitiveType(sf::Lines);

        // Vertical lines.
        for (float lx = fx0 + kTileSize; lx < fx1; lx += kTileSize) {
            m_grid.append({ { lx, fy0 }, kGridColor });
            m_grid.append({ { lx, fy1 }, kGridColor });
        }
        // Horizontal lines.
        for (float ly = fy0 + kTileSize; ly < fy1; ly += kTileSize) {
            m_grid.append({ { fx0, ly }, kGridColor });
            m_grid.append({ { fx1, ly }, kGridColor });
        }
    }

    // Vignette removed — replaced by the screen-space vignette in post.frag
    // which covers the full window and requires zero CPU geometry.

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
    target.draw(m_grid);
    target.draw(m_wallN);
    target.draw(m_wallS);
    target.draw(m_wallW);
    target.draw(m_wallE);
}

} // namespace hollow
