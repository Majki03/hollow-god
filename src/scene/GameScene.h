#pragma once

#include "hud/Hud.h"
#include "scene/Scene.h"
#include "world/Room.h"
#include "world/World.h"

#include <SFML/System/Vector2.hpp>

#include <random>
#include <vector>

namespace hollow {

class EnemyBase;
class Player;

class GameScene : public Scene {
public:
    explicit GameScene(SceneContext& ctx);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    template<typename T>
    void spawnEnemy(sf::Vector2f position);

    void spawnWave();
    void resolveCombat();
    void resolveEnemyContact();
    void separateEnemies();

    void onEnter() override;

    Room                    m_room;
    World                   m_world;
    Hud                     m_hud;
    Player*                 m_player = nullptr;
    std::vector<EnemyBase*> m_enemies;
    int                     m_wave        = 0;
    bool                    m_boonPending = false;
    float                   m_shakeTrauma = 0.f;
    int                     m_prevHp      = 0;     // detect damage this frame for shake trigger
    std::mt19937            m_rng;
};

} // namespace hollow
