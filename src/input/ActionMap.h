#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <array>
#include <cstddef>
#include <variant>

namespace hollow {

class InputState;

// Game-level actions. Scenes should speak in these, never in raw keys, so we
// can remap bindings later without chasing every call site.
enum class Action : std::size_t {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Attack,
    Dash,
    Interact,
    Pause,
    Confirm,
    Back,
    Count,
};

// A single binding — key or mouse button. std::variant leaves room for
// controller buttons/axes without rewriting every query.
using Binding = std::variant<sf::Keyboard::Key, sf::Mouse::Button>;

class ActionMap {
public:
    explicit ActionMap(const InputState& input);

    void bind(Action a, Binding b);

    bool isDown(Action a) const;
    bool justPressed(Action a) const;

private:
    static constexpr std::size_t kCount = static_cast<std::size_t>(Action::Count);

    const InputState&           m_input;
    std::array<Binding, kCount> m_bindings;
};

} // namespace hollow
