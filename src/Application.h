#pragma once

#include "audio/AudioSystem.h"
#include "core/SceneContext.h"
#include "data/DataStore.h"
#include "render/PostProcess.h"
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
    AudioSystem      m_audio;
    DataStore        m_data;
    PostProcess      m_post{1280, 720};
    float            m_time = 0.f;
    SceneContext     m_ctx;
};

} // namespace hollow
