#pragma once

#include "data/DataStore.h"
#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace hollow {

class Player;

// Presented every third wave: shows two random curses and a Skip option.
// Accepting a curse applies a permanent debuff and guarantees the next boon
// draw contains at least one Rare. Skipping yields nothing.
class CurseSelectionScene : public Scene {
public:
    CurseSelectionScene(SceneContext& ctx, Player& player,
                        bool& rareGuaranteedOut);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    void applyCurse(const DataStore::Curse& c);
    void buildCards();

    static constexpr int kCardCount = 3;  // 2 curses + 1 skip
    static constexpr int kSkipIndex = 2;

    Player& m_player;
    bool&   m_rareGuaranteed;

    // Indices 0-1 are curse pointers; index 2 (kSkipIndex) has no pointer.
    std::array<const DataStore::Curse*, 2> m_curseChoices{};
    int                                    m_selected = 0;

    sf::RectangleShape m_overlay;
    sf::Text           m_header;
    sf::Text           m_subheader;

    struct Card {
        sf::RectangleShape bg;
        sf::RectangleShape border;
        sf::RectangleShape accentBar;
        sf::Text           name;
        sf::Text           description;
    };
    std::array<Card, kCardCount> m_cards;
    bool                         m_fontLoaded = false;
};

} // namespace hollow
