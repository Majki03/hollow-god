#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf { class RenderTarget; }

namespace hollow {

class Player;
struct SceneContext;

// Bottom-left HP bar + label; top-right wave counter.
class Hud {
public:
    explicit Hud(const SceneContext& ctx);

    // isBossWave = true shows a centred "HOLLOW HERALD" warning banner.
    void update(const Player& player, int wave, int kills, bool isBossWave = false);
    void render(sf::RenderTarget& target) const;

private:
    sf::RectangleShape m_barBg;
    sf::RectangleShape m_barFill;
    sf::RectangleShape m_dashBg;
    sf::RectangleShape m_dashFill;
    sf::Text           m_hpLabel;
    sf::Text           m_dashLabel;
    sf::Text           m_waveLabel;
    sf::Text           m_killLabel;
    sf::Text           m_bossLabel;   // shown only on boss waves
    bool               m_isBossWave = false;

    static constexpr float kBarW  = 200.f;
    static constexpr float kBarH  =  14.f;
    static constexpr float kDashW =  80.f;
    static constexpr float kDashH =   6.f;
    static constexpr float kPadX  =  20.f;
    static constexpr float kPadY  =  20.f;
    static constexpr float kWinW  = 1280.f;
    static constexpr float kWinH  =  720.f;
};

} // namespace hollow
