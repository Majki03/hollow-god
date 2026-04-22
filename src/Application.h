#pragma once

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
    SceneStack       m_scenes;
};

} // namespace hollow
