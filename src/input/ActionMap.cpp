#include "input/ActionMap.h"

#include "input/InputState.h"

namespace hollow {

namespace {
    constexpr std::size_t idx(Action a) { return static_cast<std::size_t>(a); }
}

ActionMap::ActionMap(const InputState& input)
    : m_input(input)
{
    // WASD + mouse defaults. Rebinding lands in the options menu (Phase 8).
    m_bindings[idx(Action::MoveUp)]    = sf::Keyboard::W;
    m_bindings[idx(Action::MoveDown)]  = sf::Keyboard::S;
    m_bindings[idx(Action::MoveLeft)]  = sf::Keyboard::A;
    m_bindings[idx(Action::MoveRight)] = sf::Keyboard::D;
    m_bindings[idx(Action::Attack)]    = sf::Mouse::Left;
    m_bindings[idx(Action::Dash)]      = sf::Keyboard::Space;
    m_bindings[idx(Action::Interact)]  = sf::Keyboard::E;
    m_bindings[idx(Action::Pause)]     = sf::Keyboard::Tab;
    m_bindings[idx(Action::Confirm)]   = sf::Keyboard::Enter;
    m_bindings[idx(Action::Back)]      = sf::Keyboard::Escape;
}

void ActionMap::bind(Action a, Binding b)
{
    m_bindings[idx(a)] = b;
}

bool ActionMap::isDown(Action a) const
{
    const Binding& b = m_bindings[idx(a)];
    if (const auto* key = std::get_if<sf::Keyboard::Key>(&b)) {
        return m_input.isDown(*key);
    }
    return m_input.mouseDown(std::get<sf::Mouse::Button>(b));
}

bool ActionMap::justPressed(Action a) const
{
    const Binding& b = m_bindings[idx(a)];
    if (const auto* key = std::get_if<sf::Keyboard::Key>(&b)) {
        return m_input.justPressed(*key);
    }
    return m_input.mouseJustPressed(std::get<sf::Mouse::Button>(b));
}

} // namespace hollow
