#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <bitset>
#include <cstddef>

namespace sf { class Event; }

namespace hollow {

// Edge-aware input snapshot. Events flow in via onEvent(); beginFrame() rolls
// current state into "previous" so queries can distinguish held from just-
// pressed. Scenes read from this instead of touching SFML globals directly.
class InputState {
public:
    void beginFrame();
    void onEvent(const sf::Event& event);

    bool isDown(sf::Keyboard::Key k) const;
    bool justPressed(sf::Keyboard::Key k) const;
    bool justReleased(sf::Keyboard::Key k) const;

    bool mouseDown(sf::Mouse::Button b) const;
    bool mouseJustPressed(sf::Mouse::Button b) const;

    sf::Vector2i mousePosition() const { return m_mousePos; }

private:
    static constexpr std::size_t kKeyCount   = sf::Keyboard::KeyCount;
    static constexpr std::size_t kMouseCount = sf::Mouse::ButtonCount;

    std::bitset<kKeyCount>   m_keysNow;
    std::bitset<kKeyCount>   m_keysPrev;
    std::bitset<kMouseCount> m_mouseNow;
    std::bitset<kMouseCount> m_mousePrev;
    sf::Vector2i             m_mousePos{};
};

} // namespace hollow
