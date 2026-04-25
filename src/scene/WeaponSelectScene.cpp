#include "scene/WeaponSelectScene.h"

#include "audio/AudioSystem.h"
#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "input/ActionMap.h"
#include "resources/ResourceCache.h"
#include "scene/GameScene.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <memory>

namespace hollow {

const std::array<WeaponSelectScene::WeaponCard, WeaponSelectScene::kCardCount>
WeaponSelectScene::kCards = {{
    { WeaponType::Sword, "SWORD",
      "Heavy melee arcs",
      "Hold to charge a wide slam",
      sf::Color(200, 90, 10) },
    { WeaponType::Bow, "BOW",
      "Precise ranged arrows",
      "Hold to charge a piercing shot",
      sf::Color(20, 160, 180) },
    { WeaponType::Spear, "SPEAR",
      "Long forward thrust, pierces",
      "Press E to throw — unarmed 1.5s",
      sf::Color(110, 30, 170) },
}};

namespace {
    constexpr float kCardW   = 240.f;
    constexpr float kCardH   = 300.f;
    constexpr float kCardGap =  28.f;
    constexpr float kCardY   = 185.f;
    constexpr float kBarH    =   8.f;
    constexpr unsigned kNameSize    = 18;
    constexpr unsigned kTagSize     = 12;
    constexpr unsigned kAltDescSize = 11;
}

WeaponSelectScene::WeaponSelectScene(SceneContext& ctx)
    : Scene(ctx)
    , m_header(makeText(ctx, "CHOOSE YOUR WEAPON", 28, sf::Color(200, 170, 80)))
    , m_subheader(makeText(ctx, "Arrow keys to browse    Enter to begin", 13, sf::Color(100, 88, 68)))
{
    const auto hb = m_header.getLocalBounds();
    m_header.setOrigin(hb.left + hb.width * 0.5f, 0.f);
    m_header.setPosition(640.f, 120.f);

    const auto sb = m_subheader.getLocalBounds();
    m_subheader.setOrigin(sb.left + sb.width * 0.5f, 0.f);
    m_subheader.setPosition(640.f, 160.f);

    try {
        ctx.fonts.get(HG_DEV_FONT);
        m_fontLoaded = true;
    } catch (...) {}

    buildCards();
}

void WeaponSelectScene::buildCards()
{
    const float totalW = kCardCount * kCardW + (kCardCount - 1) * kCardGap;
    const float startX = (1280.f - totalW) * 0.5f;

    for (int i = 0; i < kCardCount; ++i) {
        const float x = startX + i * (kCardW + kCardGap);
        const WeaponCard& wc = kCards[i];
        Card& c = m_cards[i];

        c.bg.setPosition({ x, kCardY });
        c.bg.setSize({ kCardW, kCardH });
        c.bg.setFillColor(sf::Color(20, 14, 28));

        c.border.setPosition({ x - 2.f, kCardY - 2.f });
        c.border.setSize({ kCardW + 4.f, kCardH + 4.f });

        c.accentBar.setPosition({ x, kCardY });
        c.accentBar.setSize({ kCardW, kBarH });
        c.accentBar.setFillColor(wc.accentColor);

        if (m_fontLoaded) {
            const sf::Font& font = m_ctx.fonts.get(HG_DEV_FONT);

            c.name.setFont(font);
            c.name.setString(wc.label);
            c.name.setCharacterSize(kNameSize);
            c.name.setFillColor(sf::Color(240, 230, 210));
            c.name.setPosition({ x + 14.f, kCardY + kBarH + 16.f });

            c.tagline.setFont(font);
            c.tagline.setString(wc.tagline);
            c.tagline.setCharacterSize(kTagSize);
            c.tagline.setFillColor(sf::Color(160, 148, 130));
            c.tagline.setPosition({ x + 14.f, kCardY + kBarH + 50.f });

            c.altDesc.setFont(font);
            c.altDesc.setString(wc.altDesc);
            c.altDesc.setCharacterSize(kAltDescSize);
            c.altDesc.setFillColor(sf::Color(120, 110, 90));
            c.altDesc.setPosition({ x + 14.f, kCardY + kBarH + 76.f });
        }
    }
}

void WeaponSelectScene::update(float dt)
{
    (void)dt;

    if (m_ctx.actions.justPressed(Action::MoveLeft))
        m_selected = (m_selected + kCardCount - 1) % kCardCount;
    if (m_ctx.actions.justPressed(Action::MoveRight))
        m_selected = (m_selected + 1) % kCardCount;

    if (m_ctx.actions.justPressed(Action::Confirm) ||
        m_ctx.actions.justPressed(Action::Attack)) {
        m_ctx.audio.play(Sfx::BoonPick);
        const WeaponType chosen = kCards[m_selected].type;
        m_ctx.scenes.pop();
        m_ctx.scenes.push(std::make_unique<GameScene>(m_ctx, chosen));
    }
}

void WeaponSelectScene::render(sf::RenderTarget& target)
{
    target.draw(m_header);
    target.draw(m_subheader);

    for (int i = 0; i < kCardCount; ++i) {
        Card& c = m_cards[i];
        const bool sel = (i == m_selected);

        c.border.setFillColor(sel ? kCards[i].accentColor : sf::Color(60, 50, 70));

        target.draw(c.border);
        target.draw(c.bg);
        target.draw(c.accentBar);

        if (m_fontLoaded) {
            target.draw(c.name);
            target.draw(c.tagline);
            target.draw(c.altDesc);
        }
    }
}

} // namespace hollow
