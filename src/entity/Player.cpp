#include "entity/Player.h"

#include "input/ActionMap.h"
#include "input/InputState.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace hollow {

namespace {
    // These are fixed movement constants — not moddable by boons.
    constexpr float kAccel  = 1700.f;
    constexpr float kDrag   = 1300.f;
    constexpr float kRadius = 14.f;

    constexpr float kSwingDuration = 0.30f;
    constexpr float kHitboxStart   = 0.05f;
    constexpr float kHitboxEnd     = 0.20f;
    constexpr float kSwingReach    = 40.f;
    constexpr float kSwingRadius   = 34.f;

    constexpr float kRad2Deg  = 180.f / std::numbers::pi_v<float>;

    // Swing arc geometry constants.
    constexpr float kArcInner    = 20.f;  // start of ring (px from player)
    constexpr float kArcOuter    = 66.f;  // end of ring
    constexpr float kArcHalfDeg  = 52.f;  // ± half-span in degrees
    constexpr int   kArcSegments = 18;
}

Player::Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions)
    : Entity(startPosition)
    , m_body(kRadius)
    , m_aimIndicator({ 22.f, 3.f })
    , m_swingArc(sf::Triangles, static_cast<std::size_t>(kArcSegments * 6))
    , m_input(input)
    , m_actions(actions)
{
    m_hp = m_stats.maxHp;
    m_body.setOrigin(kRadius, kRadius);
    m_body.setFillColor(sf::Color(230, 228, 240));
    m_body.setOutlineColor(sf::Color(70, 40, 100));
    m_body.setOutlineThickness(2.f);
    m_body.setPosition(m_position);

    m_aimIndicator.setOrigin(0.f, 1.5f);
    m_aimIndicator.setFillColor(sf::Color(200, 170, 80));
}

bool Player::hitboxActive() const
{
    return m_attackState == AttackState::Swinging
        && m_attackTimer >= kHitboxStart
        && m_attackTimer <= kHitboxEnd;
}

sf::Vector2f Player::hitboxPosition() const
{
    return m_position + sf::Vector2f{
        std::cos(m_aimAngle) * kSwingReach,
        std::sin(m_aimAngle) * kSwingReach,
    };
}

float Player::hitboxRadius() const { return kSwingRadius; }

bool Player::damage(int amount)
{
    if (m_iframeTimer > 0.f) return false;
    const int effective = std::max(1, amount - m_stats.dmgReduce);
    m_hp -= effective;
    m_iframeTimer = m_stats.iframeDur;
    if (m_hp <= 0) {
        m_hp = 0;
        kill();
    }
    return true;
}

void Player::healBy(int amount)
{
    m_hp = std::min(m_hp + amount, m_stats.maxHp);
}

void Player::confine(sf::Vector2f mn, sf::Vector2f mx)
{
    if (m_position.x < mn.x) { m_position.x = mn.x; m_velocity.x = std::max(m_velocity.x, 0.f); }
    if (m_position.x > mx.x) { m_position.x = mx.x; m_velocity.x = std::min(m_velocity.x, 0.f); }
    if (m_position.y < mn.y) { m_position.y = mn.y; m_velocity.y = std::max(m_velocity.y, 0.f); }
    if (m_position.y > mx.y) { m_position.y = mx.y; m_velocity.y = std::min(m_velocity.y, 0.f); }
    m_body.setPosition(m_position);
}

void Player::update(float dt)
{
    if (m_iframeTimer > 0.f) {
        m_iframeTimer -= dt;
        const bool visible = static_cast<int>(m_iframeTimer / 0.1f) % 2 == 0;
        m_body.setFillColor(visible ? sf::Color(230, 228, 240) : sf::Color(230, 228, 240, 80));
    }

    sf::Vector2f wish{};
    if (m_actions.isDown(Action::MoveUp))    wish.y -= 1.f;
    if (m_actions.isDown(Action::MoveDown))  wish.y += 1.f;
    if (m_actions.isDown(Action::MoveLeft))  wish.x -= 1.f;
    if (m_actions.isDown(Action::MoveRight)) wish.x += 1.f;

    const float wishLen2 = wish.x * wish.x + wish.y * wish.y;
    if (wishLen2 > 0.f) {
        const float inv = 1.f / std::sqrt(wishLen2);
        wish.x *= inv;
        wish.y *= inv;
    }

    const sf::Vector2f desired = wish * m_stats.moveSpeed;
    const sf::Vector2f delta   = desired - m_velocity;
    const float        step    = (wishLen2 > 0.f ? kAccel : kDrag) * dt;
    const float        dMag2   = delta.x * delta.x + delta.y * delta.y;

    if (dMag2 > step * step) {
        const float scale = step / std::sqrt(dMag2);
        m_velocity.x += delta.x * scale;
        m_velocity.y += delta.y * scale;
    } else {
        m_velocity = desired;
    }

    m_position += m_velocity * dt;
    m_body.setPosition(m_position);

    const auto mouse = m_input.mousePosition();
    m_aimAngle = std::atan2(mouse.y - m_position.y, mouse.x - m_position.x);

    m_aimIndicator.setPosition(m_position);
    m_aimIndicator.setRotation(m_aimAngle * kRad2Deg);

    if (m_attackState == AttackState::Idle &&
        m_actions.justPressed(Action::Attack)) {
        m_attackState = AttackState::Swinging;
        m_attackTimer = 0.f;
        ++m_swingId;
    }

    if (m_attackState == AttackState::Swinging) {
        m_attackTimer += dt;

        // Rebuild arc VertexArray with alpha based on swing progress.
        {
            const float progress = m_attackTimer / kSwingDuration; // 0→1
            const float alpha    = std::max(0.f, 1.f - progress) * 200.f;
            const sf::Color arcColor(240, 200, 120, static_cast<sf::Uint8>(alpha));

            const float halfRad = kArcHalfDeg * (std::numbers::pi_v<float> / 180.f);
            const float step    = (2.f * halfRad) / kArcSegments;

            std::size_t vi = 0;
            for (int s = 0; s < kArcSegments; ++s) {
                const float a0 = m_aimAngle - halfRad + s * step;
                const float a1 = a0 + step;
                // Quad as two triangles: inner0, outer0, outer1 + inner0, outer1, inner1
                const sf::Vector2f in0  = m_position + sf::Vector2f(std::cos(a0), std::sin(a0)) * kArcInner;
                const sf::Vector2f out0 = m_position + sf::Vector2f(std::cos(a0), std::sin(a0)) * kArcOuter;
                const sf::Vector2f in1  = m_position + sf::Vector2f(std::cos(a1), std::sin(a1)) * kArcInner;
                const sf::Vector2f out1 = m_position + sf::Vector2f(std::cos(a1), std::sin(a1)) * kArcOuter;

                m_swingArc[vi++] = { in0,  arcColor };
                m_swingArc[vi++] = { out0, arcColor };
                m_swingArc[vi++] = { out1, arcColor };
                m_swingArc[vi++] = { in0,  arcColor };
                m_swingArc[vi++] = { out1, arcColor };
                m_swingArc[vi++] = { in1,  arcColor };
            }
        }

        if (m_attackTimer >= kSwingDuration) {
            m_attackState = AttackState::Idle;
            m_attackTimer = 0.f;
        }
    }
}

void Player::render(sf::RenderTarget& target) const
{
    target.draw(m_aimIndicator);
    target.draw(m_body);
    if (m_attackState == AttackState::Swinging)
        target.draw(m_swingArc);
}

} // namespace hollow
