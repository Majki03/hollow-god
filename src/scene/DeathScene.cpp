#include "scene/DeathScene.h"

#include "core/SceneContext.h"
#include "input/ActionMap.h"
#include "scene/GameScene.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <cmath>
#include <cstdint>
#include <memory>

namespace hollow {

DeathScene::DeathScene(SceneContext& ctx)
    : Scene(ctx)
    , m_overlay({ 1280.f, 720.f })
    , m_banner({ 340.f, 6.f })
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 160));

    m_banner.setOrigin(170.f, 3.f);
    m_banner.setPosition(640.f, 320.f);
    m_banner.setFillColor(sf::Color(180, 30, 30));
}

void DeathScene::update(float dt)
{
    m_time += dt;

    // Slow pulse so the screen doesn't feel frozen.
    const float alpha = 0.6f + 0.4f * std::sin(m_time * 1.8f);
    sf::Color c = m_banner.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_banner.setFillColor(c);

    if (m_ctx.actions.justPressed(Action::Confirm)) {
        // Pop DeathScene, pop GameScene, push a fresh GameScene.
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
        m_ctx.scenes.push(std::make_unique<GameScene>(m_ctx));
    }
    if (m_ctx.actions.justPressed(Action::Back)) {
        // Pop DeathScene and GameScene — title is underneath.
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
    }
}

void DeathScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_banner);
}

} // namespace hollow
