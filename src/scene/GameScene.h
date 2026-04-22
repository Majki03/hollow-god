#pragma once

#include "entity/Player.h"
#include "scene/Scene.h"

namespace hollow {

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    Player m_player;
};

} // namespace hollow
