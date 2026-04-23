#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "entity/EnemyBase.h"
#include "entity/Grunt.h"
#include "entity/Player.h"
#include "hud/Hud.h"
#include "input/ActionMap.h"
#include "physics/Collision.h"
#include "scene/DeathScene.h"
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
    constexpr float kPlayerR          = 14.f;

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

template<typename T>
void GameScene::spawnEnemy(sf::Vector2f position)
{
    auto e = std::make_unique<T>(position);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
}

// Explicit instantiation so the template body doesn't need to live in the header.
template void GameScene::spawnEnemy<Grunt>(sf::Vector2f);

void GameScene::spawnWave()
{
    ++m_wave;
    const int count = std::min(m_wave, 5);

    constexpr float kEdge = 50.f;
    std::uniform_real_distribution<float> rX(
        m_room.topLeft().x + kEdge, m_room.topLeft().x + m_room.size().x - kEdge);
    std::uniform_real_distribution<float> rY(
        m_room.topLeft().y + kEdge, m_room.topLeft().y + m_room.size().y - kEdge);

    const sf::Vector2f playerPos = m_player->position();
    constexpr float    kMinDist  = 220.f;

    for (int i = 0; i < count; ++i) {
        sf::Vector2f pos;
        for (int attempt = 0; attempt < 20; ++attempt) {
            pos = { rX(m_rng), rY(m_rng) };
            const sf::Vector2f d = pos - playerPos;
            if (d.x * d.x + d.y * d.y >= kMinDist * kMinDist) break;
        }
        spawnEnemy<Grunt>(pos);
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

    const sf::Vector2f playerPos = m_player->position();
    for (EnemyBase* e : m_enemies) {
        if (e->alive()) e->seek(playerPos);
    }
    separateEnemies();

    m_world.update(dt);

    // Keep entities inside the walkable area (each enemy type may have a
    // different radius so we read it from the object, not a hardcoded constant).
    const sf::Vector2f pMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    const sf::Vector2f pMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + kPlayerR, Room::kWallThick + kPlayerR);
    m_player->confine(pMin, pMax);

    for (EnemyBase* e : m_enemies) {
        if (!e->alive()) continue;
        const float r = e->radius();
        const sf::Vector2f eMin = m_room.topLeft() + sf::Vector2f(Room::kWallThick + r, Room::kWallThick + r);
        const sf::Vector2f eMax = m_room.topLeft() + m_room.size() - sf::Vector2f(Room::kWallThick + r, Room::kWallThick + r);
        e->confine(eMin, eMax);
    }

    resolveCombat();
    resolveEnemyContact();
    m_hud.update(*m_player);

    if (!m_player->alive()) {
        m_ctx.scenes.push(std::make_unique<DeathScene>(m_ctx));
        return;
    }

    std::erase_if(m_enemies, [](const EnemyBase* e) { return !e->alive(); });
    m_world.pruneDead();

    if (m_enemies.empty()) {
        spawnWave();
    }
}

void GameScene::resolveCombat()
{
    if (!m_player->hitboxActive()) return;

    const auto  center = m_player->hitboxPosition();
    const float radius = m_player->hitboxRadius();
    const int   swing  = m_player->swingId();

    for (EnemyBase* e : m_enemies) {
        if (!e->alive() || e->lastHitSwing() == swing) continue;
        if (physics::circlesOverlap(center, radius, e->position(), e->radius())) {
            e->damage(kSwingDamage);
            e->setLastHitSwing(swing);

            sf::Vector2f dir = e->position() - center;
            const float  d2  = dir.x * dir.x + dir.y * dir.y;
            if (d2 > 0.f) {
                dir *= 1.f / std::sqrt(d2);
                e->applyImpulse(dir * kKnockbackImpulse);
            }
        }
    }
}

void GameScene::separateEnemies()
{
    // Push pairs of enemies apart when they overlap. Applied as a direct
    // impulse on m_knockback so the existing decay smooths it out naturally.
    constexpr float kPushStrength = 300.f;

    for (std::size_t i = 0; i < m_enemies.size(); ++i) {
        for (std::size_t j = i + 1; j < m_enemies.size(); ++j) {
            EnemyBase* a = m_enemies[i];
            EnemyBase* b = m_enemies[j];
            if (!a->alive() || !b->alive()) continue;

            sf::Vector2f delta = a->position() - b->position();
            const float  d2    = delta.x * delta.x + delta.y * delta.y;
            const float  minD  = a->radius() + b->radius();

            if (d2 < minD * minD && d2 > 0.f) {
                const float  d   = std::sqrt(d2);
                const float  pen = (minD - d) / minD; // 0→touching, 1→fully overlapping
                sf::Vector2f push = (delta / d) * (pen * kPushStrength);
                a->applyImpulse( push);
                b->applyImpulse(-push);
            }
        }
    }
}

void GameScene::resolveEnemyContact()
{
    for (EnemyBase* e : m_enemies) {
        if (!e->alive()) continue;
        if (physics::circlesOverlap(m_player->position(), kPlayerR,
                                    e->position(),         e->radius())) {
            m_player->damage(Player::kContactDamage);
        }
    }
}

void GameScene::render(sf::RenderTarget& target)
{
    m_room.render(target);
    m_world.render(target);
    m_hud.render(target);
}

} // namespace hollow
