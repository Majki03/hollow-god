#pragma once

#include "entity/weapon/Weapon.h"
#include "scene/Scene.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace hollow {

// Presented before GameScene starts. Player picks one of three weapon cards
// using the same layout as BoonSelectionScene. On confirm, pops itself and
// pushes GameScene(selectedWeaponType).
class WeaponSelectScene : public Scene {
public:
    explicit WeaponSelectScene(SceneContext& ctx);

    void handleEvent(const sf::Event&) override {}
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

private:
    static constexpr int kCardCount = 3;

    struct WeaponCard {
        WeaponType         type;
        const char*        label;
        const char*        tagline;
        const char*        altDesc;   // alt-fire description shown on card
        sf::Color          accentColor;
    };

    // Defined in .cpp — sf::Color isn't constexpr in SFML 2.6.
    static const std::array<WeaponCard, kCardCount> kCards;

    void buildCards();

    int m_selected = 0;

    sf::Text m_header;
    sf::Text m_subheader;

    struct Card {
        sf::RectangleShape bg;
        sf::RectangleShape border;
        sf::RectangleShape accentBar;
        sf::Text           name;
        sf::Text           tagline;
        sf::Text           altDesc;
    };
    std::array<Card, kCardCount> m_cards;
    bool m_fontLoaded = false;
};

} // namespace hollow
