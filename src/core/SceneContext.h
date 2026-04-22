#pragma once

#include "resources/ResourceCache.h"

namespace sf { class RenderWindow; }

namespace hollow {

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
};

} // namespace hollow
