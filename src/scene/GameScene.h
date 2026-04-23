#pragma once

#include "scene/Scene.h"
#include "world/Room.h"
#include "world/World.h"

#include <SFML/System/Vector2.hpp>

#include <random>
#include <vector>

namespace hollow {

class Enemy;
class Player;

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void spawnEnemy(sf::Vector2f position);
    void spawnWave();
    void resolveCombat();

    Room                m_room;
    World               m_world;
    Player*             m_player = nullptr;   // non-owning; World owns lifetime
    std::vector<Enemy*> m_enemies;            // non-owning; pruned in lockstep
    int                 m_wave   = 0;
    std::mt19937        m_rng;
};

} // namespace hollow
