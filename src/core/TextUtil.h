#pragma once

#include "core/SceneContext.h"

#include <SFML/Graphics/Text.hpp>

#include <string>

namespace hollow {

// Build an sf::Text pre-configured with the project font.
// Returns a default-constructed Text if no font has been loaded yet.
inline sf::Text makeText(const SceneContext& ctx,
                         const std::string& str,
                         unsigned charSize,
                         sf::Color colour = sf::Color::White)
{
    sf::Text t;
    if (!ctx.fontKey.empty()) {
        t.setFont(ctx.fonts.get(ctx.fontKey));
        t.setString(str);
        t.setCharacterSize(charSize);
        t.setFillColor(colour);
    }
    return t;
}

} // namespace hollow
