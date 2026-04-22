#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "entity/Player.h"
#include "input/ActionMap.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace hollow {

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
{
    auto player = std::make_unique<Player>(
        sf::Vector2f(640.f, 360.f), ctx.input, ctx.actions);
    m_player = player.get();
    m_world.add(std::move(player));
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
    m_world.pruneDead();
}

void GameScene::render(sf::RenderTarget& target)
{
    m_world.render(target);
}

} // namespace hollow
