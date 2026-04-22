#pragma once

#include "core/SceneContext.h"
#include "input/ActionMap.h"
#include "input/InputState.h"
#include "resources/ResourceCache.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderWindow.hpp>

namespace hollow {

class Application {
public:
    Application();

    int run();

private:
    void processEvents();
    void update(float dt);
    void render();

    // Order matters — m_ctx references every member above it, so they must be
    // fully constructed before m_ctx is aggregate-initialized below.
    sf::RenderWindow m_window;
    InputState       m_input;
    ActionMap        m_actions;
    SceneStack       m_scenes;
    TextureCache     m_textures;
    FontCache        m_fonts;
    SceneContext     m_ctx;
};

} // namespace hollow
