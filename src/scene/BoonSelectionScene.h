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
//
// Draw weights: each slot has a 15% chance of drawing from the Rare pool.
// Forced-Rare path: wave % 5 == 0, or forceRare == true (curse was accepted).
// At least one slot is guaranteed Rare on forced-Rare waves.
class BoonSelectionScene : public Scene {
public:
    BoonSelectionScene(SceneContext& ctx, Player& player,
                       int wave = 1, bool forceRare = false);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void applyBoon(const Boon& boon);
    void drawBoons(int wave, bool forceRare);
    void buildCards();

    static constexpr int kCardCount = 3;

    Player& m_player;

    std::array<const Boon*, kCardCount> m_choices{};
    int                                 m_selected = 0;

    sf::RectangleShape m_overlay;
    sf::Text           m_header;
    sf::Text           m_subheader;

    struct Card {
        sf::RectangleShape bg;
        sf::RectangleShape border;
        sf::RectangleShape schoolBar;
        sf::Text           name;
        sf::Text           tagline;
        sf::Text           tierBadge;  // "RARE" (gold) or "COMMON" (grey)
    };
    std::array<Card, kCardCount> m_cards;
    bool                         m_fontLoaded = false;
};

} // namespace hollow
