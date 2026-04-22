#include "input/InputState.h"

#include <SFML/Window/Event.hpp>

namespace hollow {

namespace {
    bool inKeyRange(int code)   { return code >= 0 && code < static_cast<int>(sf::Keyboard::KeyCount); }
    bool inMouseRange(int code) { return code >= 0 && code < static_cast<int>(sf::Mouse::ButtonCount); }
}

void InputState::beginFrame()
{
    m_keysPrev  = m_keysNow;
    m_mousePrev = m_mouseNow;
}

void InputState::onEvent(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::KeyPressed:
        if (inKeyRange(event.key.code)) m_keysNow.set(event.key.code);
        break;
    case sf::Event::KeyReleased:
        if (inKeyRange(event.key.code)) m_keysNow.reset(event.key.code);
        break;
    case sf::Event::MouseButtonPressed:
        if (inMouseRange(event.mouseButton.button)) m_mouseNow.set(event.mouseButton.button);
        break;
    case sf::Event::MouseButtonReleased:
        if (inMouseRange(event.mouseButton.button)) m_mouseNow.reset(event.mouseButton.button);
        break;
    case sf::Event::MouseMoved:
        m_mousePos = { event.mouseMove.x, event.mouseMove.y };
        break;
    case sf::Event::LostFocus:
        // Forget state so held keys don't stick while the window is inactive.
        m_keysNow.reset();
        m_mouseNow.reset();
        break;
    default:
        break;
    }
}

bool InputState::isDown(sf::Keyboard::Key k) const
{
    return inKeyRange(k) && m_keysNow.test(k);
}

bool InputState::justPressed(sf::Keyboard::Key k) const
{
    return inKeyRange(k) && m_keysNow.test(k) && !m_keysPrev.test(k);
}

bool InputState::justReleased(sf::Keyboard::Key k) const
{
    return inKeyRange(k) && !m_keysNow.test(k) && m_keysPrev.test(k);
}

bool InputState::mouseDown(sf::Mouse::Button b) const
{
    return inMouseRange(b) && m_mouseNow.test(b);
}

bool InputState::mouseJustPressed(sf::Mouse::Button b) const
{
    return inMouseRange(b) && m_mouseNow.test(b) && !m_mousePrev.test(b);
}

} // namespace hollow
