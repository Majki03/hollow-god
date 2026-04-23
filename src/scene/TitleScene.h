#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace hollow {

class TitleScene : public Scene {
public:
    explicit TitleScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::RectangleShape m_banner;
    sf::Text           m_title;
    sf::Text           m_prompt;
    float              m_time = 0.f;
};

} // namespace hollow
