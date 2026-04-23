#include "scene/PauseScene.h"

#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "input/ActionMap.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <cmath>
#include <cstdint>

namespace hollow {

PauseScene::PauseScene(SceneContext& ctx)
    : Scene(ctx)
    , m_overlay({ 1280.f, 720.f })
    , m_banner({ 300.f, 6.f })
    , m_heading(makeText(ctx, "PAUSED", 60, sf::Color(190, 190, 220)))
    , m_options{
        makeText(ctx, "Resume",       22, sf::Color::White),
        makeText(ctx, "Quit to title", 22, sf::Color::White),
    }
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 150));

    m_banner.setOrigin(150.f, 3.f);
    m_banner.setPosition(640.f, 316.f);
    m_banner.setFillColor(sf::Color(80, 70, 110));

    const auto hb = m_heading.getLocalBounds();
    m_heading.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
    m_heading.setPosition(640.f, 280.f);

    // Stack the two menu items vertically.
    const float startY = 360.f;
    const float stepY  = 40.f;
    for (int i = 0; i < 2; ++i) {
        const auto lb = m_options[i].getLocalBounds();
        m_options[i].setOrigin(lb.left + lb.width * 0.5f, lb.top);
        m_options[i].setPosition(640.f, startY + i * stepY);
    }

    refreshOptionColors();
}

void PauseScene::refreshOptionColors()
{
    for (int i = 0; i < 2; ++i) {
        m_options[i].setFillColor(i == m_cursor
            ? sf::Color(240, 220, 100)
            : sf::Color(130, 120, 150));
    }
}

void PauseScene::update(float dt)
{
    m_time += dt;

    // Toggle pause off (Tab) or Esc also resumes.
    if (m_ctx.actions.justPressed(Action::Pause) ||
        m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.pop();
        return;
    }

    // Navigate options.
    if (m_ctx.actions.justPressed(Action::MoveUp) ||
        m_ctx.actions.justPressed(Action::MoveDown)) {
        m_cursor ^= 1;
        refreshOptionColors();
    }

    // Confirm selection.
    if (m_ctx.actions.justPressed(Action::Confirm)) {
        if (m_cursor == 0) {
            m_ctx.scenes.pop(); // resume
        } else {
            m_ctx.scenes.pop(); // pop PauseScene
            m_ctx.scenes.pop(); // pop GameScene
        }
    }

    // Pulse the heading.
    const float alpha = 0.65f + 0.35f * std::sin(m_time * 1.6f);
    sf::Color c = m_heading.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_heading.setFillColor(c);
}

void PauseScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_banner);
    target.draw(m_heading);
    for (const auto& opt : m_options)
        target.draw(opt);
}

} // namespace hollow
