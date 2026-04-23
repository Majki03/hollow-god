#include "scene/BoonSelectionScene.h"

#include "boon/BoonPool.h"
#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "entity/Player.h"
#include "input/ActionMap.h"
#include "resources/ResourceCache.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <random>

namespace hollow {

namespace {
    constexpr float kCardW    = 220.f;
    constexpr float kCardH    = 280.f;
    constexpr float kCardGap  =  30.f;
    constexpr float kCardY    = 200.f;  // top edge of cards
    constexpr float kBarH     =   8.f;  // school colour stripe at top of card
    constexpr unsigned kNameSize    = 16;
    constexpr unsigned kTaglineSize = 12;

    sf::Color schoolColor(BoonSchool s)
    {
        switch (s) {
        case BoonSchool::Vigor:     return sf::Color(180,  30,  30);
        case BoonSchool::Wrath:     return sf::Color(200,  90,  10);
        case BoonSchool::Swiftness: return sf::Color( 20, 160, 180);
        case BoonSchool::Dominion:  return sf::Color(110,  30, 170);
        }
        return sf::Color::White;
    }
}

BoonSelectionScene::BoonSelectionScene(SceneContext& ctx, Player& player)
    : Scene(ctx)
    , m_player(player)
    , m_overlay(sf::Vector2f(1280.f, 720.f))
    , m_header(makeText(ctx, "CHOOSE A DIVINE ECHO", 28, sf::Color(200, 170, 80)))
    , m_subheader(makeText(ctx, "Arrow keys to browse    Enter to accept", 13, sf::Color(100, 88, 68)))
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 180));

    const auto hb = m_header.getLocalBounds();
    m_header.setOrigin(hb.left + hb.width * 0.5f, 0.f);
    m_header.setPosition(640.f, 130.f);

    const auto sb = m_subheader.getLocalBounds();
    m_subheader.setOrigin(sb.left + sb.width * 0.5f, 0.f);
    m_subheader.setPosition(640.f, 170.f);

    // Pick three distinct random boons.
    auto pool = BoonPool::all();
    std::vector<int> indices(pool.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::mt19937 rng(std::random_device{}());
    std::shuffle(indices.begin(), indices.end(), rng);

    for (int i = 0; i < kCardCount; ++i) {
        m_choices[i] = &pool[indices[i]];
    }

    // Try to load the dev font; cards degrade gracefully without it.
    try {
        ctx.fonts.get(HG_DEV_FONT);
        m_fontLoaded = true;
    } catch (...) {}

    buildCards();
}

void BoonSelectionScene::buildCards()
{
    const float totalW = kCardCount * kCardW + (kCardCount - 1) * kCardGap;
    const float startX = (1280.f - totalW) * 0.5f;

    for (int i = 0; i < kCardCount; ++i) {
        const float x = startX + i * (kCardW + kCardGap);
        const Boon& boon = *m_choices[i];
        Card& c = m_cards[i];

        c.bg.setPosition({ x, kCardY });
        c.bg.setSize({ kCardW, kCardH });
        c.bg.setFillColor(sf::Color(20, 14, 28));

        c.border.setPosition({ x - 2.f, kCardY - 2.f });
        c.border.setSize({ kCardW + 4.f, kCardH + 4.f });
        // border color set each frame in render()

        c.schoolBar.setPosition({ x, kCardY });
        c.schoolBar.setSize({ kCardW, kBarH });
        c.schoolBar.setFillColor(schoolColor(boon.school));

        if (m_fontLoaded) {
            const sf::Font& font = m_ctx.fonts.get(HG_DEV_FONT);

            c.name.setFont(font);
            c.name.setString(std::string(boon.name));
            c.name.setCharacterSize(kNameSize);
            c.name.setFillColor(sf::Color(240, 230, 210));
            c.name.setPosition({ x + 12.f, kCardY + kBarH + 14.f });

            c.tagline.setFont(font);
            c.tagline.setString(std::string(boon.tagline));
            c.tagline.setCharacterSize(kTaglineSize);
            c.tagline.setFillColor(sf::Color(160, 148, 130));
            c.tagline.setPosition({ x + 12.f, kCardY + kBarH + 42.f });
        }
    }
}

void BoonSelectionScene::update(float dt)
{
    (void)dt;

    if (m_ctx.actions.justPressed(Action::MoveLeft)) {
        m_selected = (m_selected + kCardCount - 1) % kCardCount;
    }
    if (m_ctx.actions.justPressed(Action::MoveRight)) {
        m_selected = (m_selected + 1) % kCardCount;
    }
    if (m_ctx.actions.justPressed(Action::Confirm) ||
        m_ctx.actions.justPressed(Action::Attack)) {
        applyBoon(*m_choices[m_selected]);
        m_ctx.scenes.pop();
    }
}

void BoonSelectionScene::applyBoon(const Boon& boon)
{
    PlayerStats& s = m_player.mutableStats();
    const int    v = static_cast<int>(boon.value);

    switch (boon.effect) {
    case BoonEffect::MaxHpUp:
        s.maxHp += v;
        m_player.healBy(v);   // also top up HP by the same amount
        break;
    case BoonEffect::HealNow:
        m_player.healBy(v);
        break;
    case BoonEffect::DmgReduceUp:
        s.dmgReduce += v;
        break;
    case BoonEffect::SwingDmgUp:
        s.swingDamage += v;
        break;
    case BoonEffect::KnockbackUp:
        s.knockback += boon.value;
        break;
    case BoonEffect::MoveSpeedUp:
        s.moveSpeed += boon.value;
        break;
    case BoonEffect::IFrameUp:
        s.iframeDur += boon.value;
        break;
    case BoonEffect::OnKillHealUp:
        s.onKillHeal += v;
        break;
    }
}

void BoonSelectionScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_header);
    target.draw(m_subheader);

    for (int i = 0; i < kCardCount; ++i) {
        Card& c = m_cards[i];
        const bool sel = (i == m_selected);

        c.border.setFillColor(sel
            ? schoolColor(m_choices[i]->school)
            : sf::Color(60, 50, 70));

        target.draw(c.border);
        target.draw(c.bg);
        target.draw(c.schoolBar);

        if (m_fontLoaded) {
            target.draw(c.name);
            target.draw(c.tagline);
        }
    }
}

} // namespace hollow
