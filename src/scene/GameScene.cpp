#include "scene/GameScene.h"

#include "core/SceneContext.h"
#include "input/ActionMap.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace hollow {

GameScene::GameScene(SceneContext& ctx)
    : Scene(ctx)
    , m_playerPlaceholder(16.f)
{
    m_playerPlaceholder.setOrigin(16.f, 16.f);
    m_playerPlaceholder.setPosition(640.f, 360.f);
    m_playerPlaceholder.setFillColor(sf::Color(180, 60, 200));
}

void GameScene::handleEvent(const sf::Event& /*event*/)
{
}

void GameScene::update(float /*dt*/)
{
    if (m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.pop();
    }
}

void GameScene::render(sf::RenderTarget& target)
{
    target.draw(m_playerPlaceholder);
}

} // namespace hollow
