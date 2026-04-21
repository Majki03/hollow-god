#pragma once

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
};

} // namespace hollow
