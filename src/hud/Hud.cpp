#include "hud/Hud.h"

#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "entity/Player.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace hollow {

namespace {
    const sf::Color kBgColor    = sf::Color(30, 10, 10, 200);
    const sf::Color kFillColor  = sf::Color(190, 40, 40);
    const sf::Color kLowColor   = sf::Color(220, 100, 20);
    const sf::Color kDashReady  = sf::Color(80, 200, 220);
    const sf::Color kDashCharge = sf::Color(40, 80, 100);
}

Hud::Hud(const SceneContext& ctx)
    : m_hpLabel(makeText(ctx, "", 12, sf::Color(180, 150, 150)))
    , m_dashLabel(makeText(ctx, "Dash", 11, sf::Color(100, 170, 180)))
    , m_waveLabel(makeText(ctx, "", 16, sf::Color(160, 140, 110)))
    , m_killLabel(makeText(ctx, "", 13, sf::Color(140, 160, 130)))
{
    const float barY  = kWinH - kPadY - kBarH;
    const float dashY = barY - kDashH - 18.f;  // above HP bar, below hp label

    // HP bar.
    m_barBg.setPosition({ kPadX, barY });
    m_barBg.setSize({ kBarW, kBarH });
    m_barBg.setFillColor(kBgColor);

    m_barFill.setPosition({ kPadX, barY });
    m_barFill.setSize({ kBarW, kBarH });
    m_barFill.setFillColor(kFillColor);

    // HP label.
    m_hpLabel.setPosition({ kPadX, barY - 18.f });

    // Dash cooldown bar (sits between hp label and top of hp bar).
    m_dashBg.setPosition({ kPadX, dashY });
    m_dashBg.setSize({ kDashW, kDashH });
    m_dashBg.setFillColor(sf::Color(20, 40, 50, 200));

    m_dashFill.setPosition({ kPadX, dashY });
    m_dashFill.setSize({ kDashW, kDashH });
    m_dashFill.setFillColor(kDashReady);

    m_dashLabel.setPosition({ kPadX + kDashW + 6.f, dashY - 1.f });

    // Wave + kill labels anchored top-right.
    m_waveLabel.setPosition({ kWinW - kPadX - 120.f, kPadY });
    m_killLabel.setPosition({ kWinW - kPadX - 120.f, kPadY + 24.f });
}

void Hud::update(const Player& player, int wave, int kills)
{
    // HP bar.
    const float ratio = static_cast<float>(player.hp()) /
                        static_cast<float>(player.maxHp());
    m_barFill.setSize({ kBarW * ratio, kBarH });
    m_barFill.setFillColor(ratio < 0.3f ? kLowColor : kFillColor);
    m_hpLabel.setString(std::to_string(player.hp()) + " / " +
                        std::to_string(player.maxHp()));

    // Dash cooldown bar.
    const float cd    = player.dashCooldownRemaining();
    const float cdMax = player.dashCooldownMax();
    const float cdRatio = (cdMax > 0.f) ? std::max(0.f, 1.f - cd / cdMax) : 1.f;
    m_dashFill.setSize({ kDashW * cdRatio, kDashH });
    m_dashFill.setFillColor(cdRatio >= 1.f ? kDashReady : kDashCharge);

    // Wave / kill.
    m_waveLabel.setString("Wave  " + std::to_string(wave));
    m_killLabel.setString("Kills  " + std::to_string(kills));
}

void Hud::render(sf::RenderTarget& target) const
{
    target.draw(m_barBg);
    target.draw(m_barFill);
    target.draw(m_dashBg);
    target.draw(m_dashFill);
    target.draw(m_hpLabel);
    target.draw(m_dashLabel);
    target.draw(m_waveLabel);
    target.draw(m_killLabel);
}

} // namespace hollow
