#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "entity/Enemy.h"
#include "entity/Player.h"
#include "input/ActionMap.h"
#include "physics/Collision.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

namespace hollow {

namespace {
    constexpr int   kSwingDamage     = 10;
    constexpr float kKnockbackImpulse = 420.f;
}

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
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

    std::uniform_real_distribution<float> rX(120.f, 1160.f);
    std::uniform_real_distribution<float> rY(120.f,  600.f);

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
    m_world.render(target);
}

} // namespace hollow
