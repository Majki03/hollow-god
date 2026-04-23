#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace hollow {

class PauseScene : public Scene {
public:
    explicit PauseScene(SceneContext& ctx);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void refreshOptionColors();

    sf::RectangleShape m_overlay;
    sf::RectangleShape m_banner;
    sf::Text           m_heading;
    std::array<sf::Text, 2> m_options;  // 0 = Resume, 1 = Quit to Title
    int                m_cursor = 0;
    float              m_time   = 0.f;
};

} // namespace hollow
