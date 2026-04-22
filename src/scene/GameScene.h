#pragma once

#include "scene/Scene.h"
#include "world/World.h"

namespace hollow {

class Player;

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    World   m_world;
    Player* m_player = nullptr; // non-owning; World owns the lifetime
};

} // namespace hollow
