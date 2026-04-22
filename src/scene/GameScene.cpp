#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "input/ActionMap.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace hollow {

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
    , m_player(sf::Vector2f(640.f, 360.f), ctx.actions)
{
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

    m_player.update(dt);
}

void GameScene::render(sf::RenderTarget& target)
{
    m_player.render(target);
}

} // namespace hollow
