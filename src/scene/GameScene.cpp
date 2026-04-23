#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "entity/Enemy.h"
#include "entity/Player.h"
#include "input/ActionMap.h"
#include "physics/Collision.h"
#include "scene/SceneStack.h"
#include "world/Room.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

namespace hollow {

namespace {
    constexpr int   kSwingDamage      = 10;
    constexpr float kKnockbackImpulse = 420.f;

    // Arena sits centred in the 1280×720 window with a generous border.
    const sf::Vector2f kRoomOrigin = { 120.f, 80.f };
    const sf::Vector2f kRoomSize   = { 1040.f, 560.f };
}

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
    , m_room(kRoomOrigin, kRoomSize)
    , m_rng(std::random_device{}())
{
    auto player = std::make_unique<Player>(
        sf::Vector2f(640.f, 360.f), ctx.input, ctx.actions);
    m_player = player.get();
    m_world.add(std::move(player));

    spawnWave();
}

void GameScene::spawnEnemy(sf::Vector2f position)
{
    auto e = std::make_unique<Enemy>(position);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

void GameScene::spawnWave()
{
    ++m_wave;
    // Each wave adds one extra enemy, capped so early rooms stay manageable.
    const int count = std::min(m_wave, 5);

    // wall thickness (24) + enemy radius (18) + small gap
    constexpr float kEdge = 50.f;
    std::uniform_real_distribution<float> rX(
        m_room.topLeft().x + kEdge, m_room.topLeft().x + m_room.size().x - kEdge);
    std::uniform_real_distribution<float> rY(
        m_room.topLeft().y + kEdge, m_room.topLeft().y + m_room.size().y - kEdge);

    const sf::Vector2f playerPos = m_player->position();
    constexpr float    kMinDist  = 220.f;

    for (int i = 0; i < count; ++i) {
        // Rejection-sample until we're far enough from the player.
        sf::Vector2f pos;
        for (int attempt = 0; attempt < 20; ++attempt) {
            pos = { rX(m_rng), rY(m_rng) };
            const sf::Vector2f d = pos - playerPos;
            if (d.x * d.x + d.y * d.y >= kMinDist * kMinDist) break;
        }
        spawnEnemy(pos);
    }
}

void GameScene::handleEvent(const sf::Event& /*event*/)
{
}

void GameScene::update(float dt)
{
    if (m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.pop();
        return;
    }

    m_world.update(dt);

    // Keep entities inside the walkable area.
    constexpr float kPlayerR = 14.f;
    constexpr float kEnemyR  = 18.f;
    const sf::Vector2f pMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    const sf::Vector2f pMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    m_player->confine(pMin, pMax);

    const sf::Vector2f eMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + kEnemyR, Room::kWallThick + kEnemyR);
    const sf::Vector2f eMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + kEnemyR, Room::kWallThick + kEnemyR);
    for (Enemy* e : m_enemies) {
        if (e->alive()) e->confine(eMin, eMax);
    }

    resolveCombat();

    // Drop dangling enemy refs BEFORE World frees the memory they point to.
    std::erase_if(m_enemies, [](const Enemy* e) { return !e->alive(); });
    m_world.pruneDead();

    if (m_enemies.empty()) {
        spawnWave();
    }
}

void GameScene::resolveCombat()
{
    if (!m_player->hitboxActive()) {
        return;
    }

    const auto  center = m_player->hitboxPosition();
    const float radius = m_player->hitboxRadius();
    const int   swing  = m_player->swingId();

    for (Enemy* e : m_enemies) {
        if (!e->alive() || e->lastHitSwing() == swing) {
            continue;
        }
        if (physics::circlesOverlap(center, radius, e->position(), e->radius())) {
            e->damage(kSwingDamage);
            e->setLastHitSwing(swing);

            // Knock the enemy away from the swing centre.
            sf::Vector2f dir = e->position() - center;
            const float  d2  = dir.x * dir.x + dir.y * dir.y;
            if (d2 > 0.f) {
                dir *= 1.f / std::sqrt(d2);
                e->applyImpulse(dir * kKnockbackImpulse);
            }
        }
    }
}

void GameScene::render(sf::RenderTarget& target)
{
    m_room.render(target);
    m_world.render(target);
}

} // namespace hollow
