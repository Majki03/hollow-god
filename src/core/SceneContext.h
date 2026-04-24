#pragma once

#include "resources/ResourceCache.h"

#include <string>

namespace sf { class RenderWindow; }

namespace hollow {

class AudioSystem;
class InputState;
class ActionMap;
class SceneStack;

// Shared services bundle. Passed by reference into every Scene at construction
// so scenes never reach for globals or know how the Application is wired.
struct SceneContext {
    sf::RenderWindow& window;
    const InputState& input;
    const ActionMap&  actions;
    SceneStack&       scenes;
    TextureCache&     textures;
    FontCache&        fonts;
    // Key into fonts cache. Empty string means no font was loaded; scenes must
    // guard all sf::Text usage behind !fontKey.empty().
    std::string  fontKey;
    AudioSystem& audio;
};

} // namespace hollow
