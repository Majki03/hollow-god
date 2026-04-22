#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

class TitleScene : public Scene {
public:
    explicit TitleScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::RectangleShape m_banner;
    float              m_time = 0.f;
};

} // namespace hollow
