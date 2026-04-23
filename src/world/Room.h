#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf { class RenderTarget; }

namespace hollow {

// Axis-aligned rectangular arena. Owns its own visual geometry — the floor
// grid and the four walls — so GameScene doesn't need to know about pixels.
class Room {
public:
    Room(sf::Vector2f topLeft, sf::Vector2f size);

    static constexpr float kWallThick = 24.f;

    sf::Vector2f topLeft() const { return m_topLeft; }
    sf::Vector2f size()    const { return m_size; }
    sf::Vector2f center()  const { return m_topLeft + m_size * 0.5f; }

    // Returns the closest point inside the room, shrunk by `margin`.
    // Pass the entity's radius as the margin to keep the body fully inside.
    sf::Vector2f clamp(sf::Vector2f point, float margin) const;

    void render(sf::RenderTarget& target) const;

private:
    void buildGeometry();

    sf::Vector2f       m_topLeft;
    sf::Vector2f       m_size;
    sf::RectangleShape m_floor;
    sf::VertexArray    m_grid;
    sf::RectangleShape m_wallN, m_wallS, m_wallW, m_wallE;
};

} // namespace hollow
