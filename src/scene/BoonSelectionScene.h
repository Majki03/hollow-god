#pragma once

#include "boon/Boon.h"
#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace hollow {

class Player;

// Pauses the run and presents three boon choices. Pops itself once the player
// confirms a pick; the chosen boon is applied to the player before the pop.
class BoonSelectionScene : public Scene {
public:
    BoonSelectionScene(SceneContext& ctx, Player& player);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void applyBoon(const Boon& boon);
    void buildCards();

    static constexpr int kCardCount = 3;

    Player& m_player;

    std::array<const Boon*, kCardCount> m_choices{};
    int                                 m_selected = 0;

    // Visuals
    sf::RectangleShape m_overlay;

    struct Card {
        sf::RectangleShape bg;
        sf::RectangleShape border;
        sf::RectangleShape schoolBar;
        sf::Text           name;
        sf::Text           tagline;
    };
    std::array<Card, kCardCount> m_cards;
    bool                         m_fontLoaded = false;
};

} // namespace hollow
