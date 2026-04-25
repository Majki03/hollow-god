#include "scene/CurseSelectionScene.h"

#include "audio/AudioSystem.h"
#include "core/SceneContext.h"
#include "core/TextUtil.h"
#include "entity/Player.h"
#include "entity/PlayerStats.h"
#include "input/ActionMap.h"
#include "resources/ResourceCache.h"
#include "scene/SceneStack.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <numeric>
#include <random>

namespace hollow {

namespace {
    constexpr float    kCardW   = 220.f;
    constexpr float    kCardH   = 260.f;
    constexpr float    kCardGap =  30.f;
    constexpr float    kCardY   = 200.f;
    constexpr float    kBarH    =   8.f;
    constexpr unsigned kNameSize = 16;
    constexpr unsigned kDescSize = 12;
}

CurseSelectionScene::CurseSelectionScene(SceneContext& ctx, Player& player,
                                         bool& rareGuaranteedOut)
    : Scene(ctx)
    , m_player(player)
    , m_rareGuaranteed(rareGuaranteedOut)
    , m_overlay(sf::Vector2f(1280.f, 720.f))
    , m_header(makeText(ctx, "A DARK BARGAIN", 28, sf::Color(200, 50, 50)))
    , m_subheader(makeText(ctx, "Accept a curse — your next boon draw will contain a Rare", 13, sf::Color(130, 60, 60)))
{
    m_overlay.setFillColor(sf::Color(0, 0, 0, 200));

    const auto hb = m_header.getLocalBounds();
    m_header.setOrigin(hb.left + hb.width * 0.5f, 0.f);
    m_header.setPosition(640.f, 130.f);

    const auto sb = m_subheader.getLocalBounds();
    m_subheader.setOrigin(sb.left + sb.width * 0.5f, 0.f);
    m_subheader.setPosition(640.f, 170.f);

    try {
        ctx.fonts.get(HG_DEV_FONT);
        m_fontLoaded = true;
    } catch (...) {}

    // Pick 2 distinct curses at random from the pool.
    const auto& pool = m_ctx.data.curses;
    if (!pool.empty()) {
        std::mt19937 rng(std::random_device{}());
        std::vector<int> idx(pool.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::shuffle(idx.begin(), idx.end(), rng);
        m_curseChoices[0] = &pool[idx[0]];
        m_curseChoices[1] = &pool[idx[std::min<std::size_t>(1, idx.size() - 1)]];
    }

    buildCards();
}

void CurseSelectionScene::buildCards()
{
    const float totalW = kCardCount * kCardW + (kCardCount - 1) * kCardGap;
    const float startX = (1280.f - totalW) * 0.5f;

    const sf::Color curseAccent(160, 20, 20);
    const sf::Color skipAccent (70, 62, 82);

    for (int i = 0; i < kCardCount; ++i) {
        const float x      = startX + i * (kCardW + kCardGap);
        const bool  isSkip = (i == kSkipIndex);
        Card& c = m_cards[i];

        c.bg.setPosition({ x, kCardY });
        c.bg.setSize({ kCardW, kCardH });
        c.bg.setFillColor(isSkip ? sf::Color(16, 14, 20) : sf::Color(28, 10, 10));

        c.border.setPosition({ x - 2.f, kCardY - 2.f });
        c.border.setSize({ kCardW + 4.f, kCardH + 4.f });
        // Fill colour is overridden each frame in render() based on selection.
        c.border.setFillColor(sf::Color(50, 40, 50));

        c.accentBar.setPosition({ x, kCardY });
        c.accentBar.setSize({ kCardW, kBarH });
        c.accentBar.setFillColor(isSkip ? skipAccent : curseAccent);

        if (m_fontLoaded) {
            const sf::Font& font = m_ctx.fonts.get(HG_DEV_FONT);

            c.name.setFont(font);
            c.name.setCharacterSize(kNameSize);
            c.name.setFillColor(isSkip ? sf::Color(140, 130, 150)
                                       : sf::Color(230, 200, 200));
            c.name.setPosition({ x + 12.f, kCardY + kBarH + 14.f });

            c.description.setFont(font);
            c.description.setCharacterSize(kDescSize);
            c.description.setFillColor(isSkip ? sf::Color(90, 82, 100)
                                              : sf::Color(170, 110, 110));
            c.description.setPosition({ x + 12.f, kCardY + kBarH + 42.f });

            if (isSkip) {
                c.name.setString("Skip");
                c.description.setString("Accept no curse this wave");
            } else {
                const DataStore::Curse& curse = *m_curseChoices[i];
                c.name.setString(curse.name);
                c.description.setString(curse.description);
            }
        }
    }
}

void CurseSelectionScene::update(float dt)
{
    (void)dt;

    if (m_ctx.actions.justPressed(Action::MoveLeft))
        m_selected = (m_selected + kCardCount - 1) % kCardCount;
    if (m_ctx.actions.justPressed(Action::MoveRight))
        m_selected = (m_selected + 1) % kCardCount;

    if (m_ctx.actions.justPressed(Action::Confirm) ||
        m_ctx.actions.justPressed(Action::Attack)) {
        if (m_selected == kSkipIndex) {
            m_ctx.scenes.pop();
        } else {
            applyCurse(*m_curseChoices[m_selected]);
            m_rareGuaranteed = true;
            m_ctx.audio.play(Sfx::BoonPick);
            m_ctx.scenes.pop();
        }
    }
}

void CurseSelectionScene::applyCurse(const DataStore::Curse& c)
{
    PlayerStats& s = m_player.mutableStats();

    switch (c.stat) {
    case BoonEffect::MoveSpeedUp:
        s.moveSpeed = std::max(60.f, s.moveSpeed + c.delta);
        break;
    case BoonEffect::IFrameUp:
        s.iframeDur = std::max(0.10f, s.iframeDur + c.delta);
        break;
    case BoonEffect::MaxHpUp: {
        const int loss = static_cast<int>(-c.delta);   // delta is negative
        s.maxHp = std::max(10, s.maxHp - loss);
        m_player.loseHpDirect(loss);
        break;
    }
    case BoonEffect::DashCooldownDown:
        // delta is negative → subtracting it adds to cooldown (makes it worse)
        s.dashCooldown -= c.delta;
        break;
    case BoonEffect::SwingDmgUp:
        s.swingDamage = std::max(0, s.swingDamage + static_cast<int>(c.delta));
        break;
    default:
        break;
    }
}

void CurseSelectionScene::render(sf::RenderTarget& target)
{
    target.draw(m_overlay);
    target.draw(m_header);
    target.draw(m_subheader);

    const sf::Color curseAccent(160, 20, 20);
    const sf::Color skipAccent (100, 90, 120);

    for (int i = 0; i < kCardCount; ++i) {
        const bool isSkip = (i == kSkipIndex);
        const bool sel    = (i == m_selected);
        Card& c = m_cards[i];

        c.border.setFillColor(sel
            ? (isSkip ? skipAccent : curseAccent)
            : sf::Color(50, 40, 50));

        target.draw(c.border);
        target.draw(c.bg);
        target.draw(c.accentBar);

        if (m_fontLoaded) {
            target.draw(c.name);
            target.draw(c.description);
        }
    }
}

} // namespace hollow
