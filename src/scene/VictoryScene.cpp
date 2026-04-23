#include "scene/VictoryScene.h"

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
#include <string>

namespace hollow {

VictoryScene::VictoryScene(SceneContext& ctx, int wavesCleared, int hpRemaining, int kills)
    : Scene(ctx)
    , m_overlay({ 1280.f, 720.f })
    , m_banner({ 400.f, 6.f })
    , m_heading(makeText(ctx, "VICTORY", 68, sf::Color(220, 180, 50)))
    , m_stats(makeText(ctx,
        "Waves: " + std::to_string(wavesCleared) +
        "     Kills: " + std::to_string(kills) +
        "     HP: " + std::to_string(hpRemaining),
        18, sf::Color(180, 160, 100)))
    , m_hints(makeText(ctx, "Enter  new run     Esc  title", 15, sf::Color(120, 110, 70)))
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 160));

    m_banner.setOrigin(200.f, 3.f);
    m_banner.setPosition(640.f, 330.f);
    m_banner.setFillColor(sf::Color(160, 130, 20));

    const auto hb = m_heading.getLocalBounds();
    m_heading.setOrigin(hb.left + hb.width * 0.5f, hb.top + hb.height * 0.5f);
    m_heading.setPosition(640.f, 280.f);

    const auto sb = m_stats.getLocalBounds();
    m_stats.setOrigin(sb.left + sb.width * 0.5f, 0.f);
    m_stats.setPosition(640.f, 375.f);

    const auto pb = m_hints.getLocalBounds();
    m_hints.setOrigin(pb.left + pb.width * 0.5f, 0.f);
    m_hints.setPosition(640.f, 420.f);
}

void VictoryScene::update(float dt)
{
    m_time += dt;

    const float alpha = 0.65f + 0.35f * std::sin(m_time * 1.5f);
    sf::Color c = m_heading.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_heading.setFillColor(c);

    if (m_ctx.actions.justPressed(Action::Confirm)) {
        // Pop VictoryScene + GameScene, start fresh.
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
        m_ctx.scenes.push(std::make_unique<GameScene>(m_ctx));
    }
    if (m_ctx.actions.justPressed(Action::Back)) {
        m_ctx.scenes.pop();
        m_ctx.scenes.pop();
    }
}

void VictoryScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_banner);
    target.draw(m_heading);
    target.draw(m_stats);
    target.draw(m_hints);
}

} // namespace hollow
