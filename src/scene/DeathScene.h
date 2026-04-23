#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>

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
    float              m_time = 0.f;
};

} // namespace hollow
