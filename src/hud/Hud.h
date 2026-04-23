#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf { class RenderTarget; }

namespace hollow {

class Player;

// Draws the HP bar at the bottom-left corner of the screen.
// Owns no game state — reads from Player each frame.
class Hud {
public:
    Hud();

    void update(const Player& player);
    void render(sf::RenderTarget& target) const;

private:
    sf::RectangleShape m_barBg;
    sf::RectangleShape m_barFill;

    static constexpr float kBarW  = 200.f;
    static constexpr float kBarH  = 14.f;
    static constexpr float kPadX  = 20.f;
    static constexpr float kPadY  = 20.f;
    // bar sits at bottom-left; y origin is from top so we anchor via window h
    static constexpr float kWinH  = 720.f;
};

} // namespace hollow
