#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace hollow {

class DeathScene : public Scene {
public:
    explicit DeathScene(SceneContext& ctx);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::RectangleShape m_overlay;
    sf::RectangleShape m_banner;
    sf::Text           m_heading;
    sf::Text           m_hints;
    float              m_time = 0.f;
};

} // namespace hollow
