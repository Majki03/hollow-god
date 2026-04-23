#include "hud/Hud.h"

#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "entity/Player.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace hollow {

namespace {
    const sf::Color kBgColor   = sf::Color(30, 10, 10, 200);
    const sf::Color kFillColor = sf::Color(190, 40, 40);
    const sf::Color kLowColor  = sf::Color(220, 100, 20);
}

Hud::Hud(const SceneContext& ctx)
    : m_hpLabel(makeText(ctx, "", 12, sf::Color(180, 150, 150)))
    , m_waveLabel(makeText(ctx, "", 16, sf::Color(160, 140, 110)))
    , m_killLabel(makeText(ctx, "", 13, sf::Color(140, 160, 130)))
{
    const float y = kWinH - kPadY - kBarH;

    m_barBg.setPosition({ kPadX, y });
    m_barBg.setSize({ kBarW, kBarH });
    m_barBg.setFillColor(kBgColor);

    m_barFill.setPosition({ kPadX, y });
    m_barFill.setSize({ kBarW, kBarH });
    m_barFill.setFillColor(kFillColor);

    // HP label sits just above the bar.
    m_hpLabel.setPosition({ kPadX, y - 18.f });

    // Wave label anchored top-right; kill count one line below it.
    m_waveLabel.setPosition({ kWinW - kPadX - 120.f, kPadY });
    m_killLabel.setPosition({ kWinW - kPadX - 120.f, kPadY + 24.f });
}

void Hud::update(const Player& player, int wave, int kills)
{
    const float ratio = static_cast<float>(player.hp()) /
                        static_cast<float>(player.maxHp());
    m_barFill.setSize({ kBarW * ratio, kBarH });
    m_barFill.setFillColor(ratio < 0.3f ? kLowColor : kFillColor);

    m_hpLabel.setString(std::to_string(player.hp()) + " / " +
                        std::to_string(player.maxHp()));
    m_waveLabel.setString("Wave  " + std::to_string(wave));
    m_killLabel.setString("Kills  " + std::to_string(kills));
}

void Hud::render(sf::RenderTarget& target) const
{
    target.draw(m_barBg);
    target.draw(m_barFill);
    target.draw(m_hpLabel);
    target.draw(m_waveLabel);
    target.draw(m_killLabel);
}

} // namespace hollow
