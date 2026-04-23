#include "scene/TitleScene.h"

#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "input/ActionMap.h"
#include "scene/GameScene.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <cmath>
#include <cstdint>
#include <memory>

namespace hollow {

TitleScene::TitleScene(SceneContext& ctx)
    : Scene(ctx)
    , m_banner(sf::Vector2f(480.f, 4.f))
    , m_title(makeText(ctx, "THE HOLLOW GOD", 52, sf::Color(200, 170, 80)))
    , m_prompt(makeText(ctx, "press Enter", 18, sf::Color(140, 120, 80)))
{
    m_banner.setOrigin(240.f, 2.f);
    m_banner.setPosition(640.f, 360.f);
    m_banner.setFillColor(sf::Color(200, 170, 80));

    // Centre the title text above the banner.
    const auto tb = m_title.getLocalBounds();
    m_title.setOrigin(tb.left + tb.width * 0.5f, tb.top + tb.height * 0.5f);
    m_title.setPosition(640.f, 310.f);

    const auto pb = m_prompt.getLocalBounds();
    m_prompt.setOrigin(pb.left + pb.width * 0.5f, 0.f);
    m_prompt.setPosition(640.f, 390.f);
}

void TitleScene::handleEvent(const sf::Event& /*event*/)
{
    // Scene transitions land in a later commit once GameScene exists.
}

void TitleScene::update(float dt)
{
    m_time += dt;

    // Pulse the prompt text; keep banner solid.
    const float alpha = 0.55f + 0.45f * std::sin(m_time * 2.f);
    sf::Color c = m_prompt.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_prompt.setFillColor(c);

    if (m_ctx.actions.justPressed(Action::Confirm)) {
        m_ctx.scenes.push(std::make_unique<GameScene>(m_ctx));
    }
    if (m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.window.close();
    }
}

void TitleScene::render(sf::RenderTarget& target)
{
    target.draw(m_title);
    target.draw(m_banner);
    target.draw(m_prompt);
}

} // namespace hollow
