#pragma once

#include "scene/Scene.h"

#include <SFML/Graphics/CircleShape.hpp>

namespace hollow {

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    // Temporary stand-in for the real player. Phase 3 replaces this with a
    // proper Player entity (sprite, velocity, collider, attacks).
    sf::CircleShape m_playerPlaceholder;
};

} // namespace hollow
