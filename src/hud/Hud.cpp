#include "hud/Hud.h"

#include "entity/Player.h"

#include <SFML/Graphics/RenderTarget.hpp>

namespace hollow {

namespace {
    const sf::Color kBgColor   = sf::Color(30, 10, 10, 200);
    const sf::Color kFillColor = sf::Color(190, 40, 40);
    const sf::Color kLowColor  = sf::Color(220, 100, 20);
}

Hud::Hud()
{
    const float y = kWinH - kPadY - kBarH;

    m_barBg.setPosition({ kPadX, y });
    m_barBg.setSize({ kBarW, kBarH });
    m_barBg.setFillColor(kBgColor);

    m_barFill.setPosition({ kPadX, y });
    m_barFill.setSize({ kBarW, kBarH });
    m_barFill.setFillColor(kFillColor);
}

void Hud::update(const Player& player)
{
    const float ratio = static_cast<float>(player.hp()) /
                        static_cast<float>(player.maxHp());
    m_barFill.setSize({ kBarW * ratio, kBarH });
    m_barFill.setFillColor(ratio < 0.3f ? kLowColor : kFillColor);
}

void Hud::render(sf::RenderTarget& target) const
{
    target.draw(m_barBg);
    target.draw(m_barFill);
}

} // namespace hollow
