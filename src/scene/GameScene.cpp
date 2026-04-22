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
#include <memory>

namespace hollow {

namespace {
    constexpr int kSwingDamage = 10;
}

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
{
    auto player = std::make_unique<Player>(
        sf::Vector2f(640.f, 360.f), ctx.input, ctx.actions);
    m_player = player.get();
    m_world.add(std::move(player));

    spawnEnemy(sf::Vector2f(880.f, 300.f));
}

void GameScene::spawnEnemy(sf::Vector2f position)
{
    auto e = std::make_unique<Enemy>(position);
    m_enemies.push_back(e.get());
    m_world.add(std::move(e));
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
        }
    }
}

void GameScene::render(sf::RenderTarget& target)
{
    m_world.render(target);
}

} // namespace hollow
