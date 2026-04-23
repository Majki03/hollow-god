#include "scene/DeathScene.h"

#include "core/SceneContext.h"
#include "core/TextUtil.h"
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
    , m_heading(makeText(ctx, "YOU DIED", 64, sf::Color(200, 40, 40)))
    , m_hints(makeText(ctx, "Enter  restart     Esc  title", 15, sf::Color(120, 100, 100)))
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 170));

    m_banner.setOrigin(170.f, 3.f);
    m_banner.setPosition(640.f, 340.f);
    m_banner.setFillColor(sf::Color(180, 30, 30));

    const auto hb = m_heading.getLocalBounds();
    m_heading.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
    m_heading.setPosition(640.f, 300.f);

    const auto pb = m_hints.getLocalBounds();
    m_hints.setOrigin(pb.left + pb.width * 0.5f, 0.f);
    m_hints.setPosition(640.f, 400.f);
}

void DeathScene::update(float dt)
{
    m_time += dt;

    const float alpha = 0.6f + 0.4f * std::sin(m_time * 1.8f);
    sf::Color c = m_heading.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_heading.setFillColor(c);

    if (m_ctx.actions.justPressed(Action::Confirm)) {
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
        m_ctx.scenes.push(std::make_unique<GameScene>(m_ctx));
    }
    if (m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
    }
}

void DeathScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_banner);
    target.draw(m_heading);
    target.draw(m_hints);
}

} // namespace hollow
