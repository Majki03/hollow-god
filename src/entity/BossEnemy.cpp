#include "entity/BossEnemy.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <numbers>

namespace hollow {

namespace {
    constexpr float kDt            = 1.f / 60.f;  // seek() called at sim rate
    constexpr float kSlamActiveDur = 0.18f;        // how long slam AoE is live
    constexpr float kSlamRestDur   = 0.70f;
    constexpr float kVolleyPauseDur = 0.50f;
}

BossEnemy::BossEnemy(sf::Vector2f position, const BossStats& stats)
    : EnemyBase(position, stats.radius, stats.maxHp)
    , m_stats(stats)
    , m_body(stats.radius)
    , m_outerRing(stats.radius + 8.f)
    , m_slamRing(0.f)
{
    // Body — dark void-purple circle with a thin bright outline.
    m_body.setOrigin(stats.radius, stats.radius);
    m_body.setPosition(position);
    m_body.setFillColor(normalColor());
    m_body.setOutlineColor(sf::Color(130, 60, 180));
    m_body.setOutlineThickness(3.f);

    // Outer ambient ring — slightly transparent purple.
    const float or_ = stats.radius + 8.f;
    m_outerRing.setOrigin(or_, or_);
    m_outerRing.setPosition(position);
    m_outerRing.setFillColor(sf::Color::Transparent);
    m_outerRing.setOutlineColor(sf::Color(100, 40, 140, 160));
    m_outerRing.setOutlineThickness(3.f);

    // Slam telegraph ring — starts invisible, grown in seek().
    m_slamRing.setFillColor(sf::Color::Transparent);
    m_slamRing.setOutlineThickness(2.f);
    m_slamRing.setOutlineColor(sf::Color(220, 40, 40, 0));
}

// ── seek() — AI state machine + visual timer updates ─────────────────────────

void BossEnemy::seek(sf::Vector2f playerPos)
{
    m_playerPos = playerPos;

    const sf::Vector2f delta = playerPos - m_position;
    const float        dist2 = delta.x * delta.x + delta.y * delta.y;
    const float        dist  = (dist2 > 0.f) ? std::sqrt(dist2) : 0.f;

    m_pulseTimer += kDt;

    switch (m_state) {
    case State::Idle:
        // Creep toward player.
        m_moveVel = (dist > 0.f) ? delta * (m_stats.moveSpeed / dist) : sf::Vector2f{};

        // Count down both cooldowns simultaneously; slam takes priority.
        m_slamCd   -= kDt;
        m_volleyCd -= kDt;

        if (m_slamCd <= 0.f) {
            m_state        = State::SlamWindup;
            m_stateTimer   = m_stats.slamWindup;
            m_slamProgress = 0.f;
            m_slamCd       = m_stats.slamCooldown;
        } else if (m_volleyCd <= 0.f) {
            m_state          = State::VolleyPause;
            m_stateTimer     = kVolleyPauseDur;
            m_pendingVolley  = true;
            m_volleyCd       = m_stats.volleyCooldown;
        }
        break;

    case State::SlamWindup:
        m_moveVel     = {};  // stop during telegraph
        m_stateTimer -= kDt;
        m_slamProgress = 1.f - (m_stateTimer / m_stats.slamWindup);
        if (m_stateTimer <= 0.f) {
            m_state      = State::SlamActive;
            m_stateTimer = kSlamActiveDur;
            m_slamProgress = 1.f;
        }
        break;

    case State::SlamActive:
        m_moveVel    = {};
        m_stateTimer -= kDt;
        if (m_stateTimer <= 0.f) {
            m_state      = State::SlamRest;
            m_stateTimer = kSlamRestDur;
            m_slamProgress = 0.f;
        }
        break;

    case State::SlamRest:
        m_moveVel    = {};
        m_stateTimer -= kDt;
        if (m_stateTimer <= 0.f)
            m_state = State::Idle;
        break;

    case State::VolleyPause:
        m_moveVel    = {};
        m_stateTimer -= kDt;
        if (m_stateTimer <= 0.f)
            m_state = State::Idle;
        break;
    }
}

// ── syncShape() — called by EnemyBase::update() after physics integration ────

void BossEnemy::syncShape()
{
    m_body.setPosition(m_position);
    m_outerRing.setPosition(m_position);
    m_slamRing.setPosition(m_position);

    // Pulse the outer ring radius gently.
    const float pulse = 0.5f + 0.5f * std::sin(m_pulseTimer * 2.5f);
    const float outerR = m_stats.radius + 6.f + pulse * 6.f;
    m_outerRing.setRadius(outerR);
    m_outerRing.setOrigin(outerR, outerR);

    // Grow slam telegraph ring.
    if (m_state == State::SlamWindup || m_state == State::SlamActive) {
        const float r = m_stats.slamRadius * m_slamProgress;
        m_slamRing.setRadius(r);
        m_slamRing.setOrigin(r, r);

        const sf::Uint8 alpha = (m_state == State::SlamActive)
            ? static_cast<sf::Uint8>(255)
            : static_cast<sf::Uint8>(80 + 175 * m_slamProgress);
        m_slamRing.setOutlineColor(sf::Color(220, 40, 40, alpha));
    } else {
        m_slamRing.setRadius(0.f);
        m_slamRing.setOutlineColor(sf::Color(220, 40, 40, 0));
    }
}

// ── takeVolley() — consumed once by GameScene ─────────────────────────────────

std::vector<sf::Vector2f> BossEnemy::takeVolley()
{
    m_pendingVolley = false;
    std::vector<sf::Vector2f> dirs;
    dirs.reserve(m_stats.volleyCount);

    // Distribute shots evenly around a full circle, with a small offset toward
    // the player so at least one projectile roughly aims at them.
    const sf::Vector2f toPlayer = m_playerPos - m_position;
    const float baseAngle = (toPlayer.x != 0.f || toPlayer.y != 0.f)
        ? std::atan2(toPlayer.y, toPlayer.x)
        : 0.f;

    const float step = 2.f * std::numbers::pi_v<float> / static_cast<float>(m_stats.volleyCount);
    for (int i = 0; i < m_stats.volleyCount; ++i) {
        const float a = baseAngle + i * step;
        dirs.push_back({ std::cos(a), std::sin(a) });
    }
    return dirs;
}

// ── render() ──────────────────────────────────────────────────────────────────

void BossEnemy::render(sf::RenderTarget& target) const
{
    // Draw slam ring behind everything so it reads as a floor-level effect.
    if (m_state == State::SlamWindup || m_state == State::SlamActive)
        target.draw(m_slamRing);

    target.draw(m_outerRing);
    target.draw(m_body);
    renderHpBar(target);
}

} // namespace hollow
