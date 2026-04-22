#pragma once

#include "input/ActionMap.h"
#include "input/InputState.h"
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

    sf::RenderWindow m_window;
    InputState       m_input;
    ActionMap        m_actions;
    SceneStack       m_scenes;
};

} // namespace hollow
