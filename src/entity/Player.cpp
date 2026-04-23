#include "entity/Player.h"

#include "input/ActionMap.h"
#include "input/InputState.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <numbers>

namespace hollow {

namespace {
    constexpr float kMaxSpeed = 260.f;
    constexpr float kAccel    = 1700.f;
    constexpr float kDrag     = 1300.f;
    constexpr float kRadius   = 14.f;

    // Swing timing — tuned for a snappy Hades-like feel, not a slow souls-like.
    constexpr float kSwingDuration = 0.30f;
    constexpr float kHitboxStart   = 0.05f; // short windup
    constexpr float kHitboxEnd     = 0.20f; // then recovery until duration
    constexpr float kSwingReach    = 40.f;
    constexpr float kSwingRadius   = 34.f;

    constexpr float kRad2Deg = 180.f / std::numbers::pi_v<float>;
}

Player::Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions)
    : Entity(startPosition)
    , m_body(kRadius)
    , m_aimIndicator({ 22.f, 3.f })
    , m_swingVisual({ 64.f, 10.f })
    , m_input(input)
    , m_actions(actions)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setFillColor(sf::Color(230, 228, 240));
    m_body.setOutlineColor(sf::Color(70, 40, 100));
    m_body.setOutlineThickness(2.f);
    m_body.setPosition(m_position);

    m_aimIndicator.setOrigin(0.f, 1.5f);
    m_aimIndicator.setFillColor(sf::Color(200, 170, 80));

    m_swingVisual.setOrigin(32.f, 5.f);
    m_swingVisual.setFillColor(sf::Color(240, 200, 120, 200));
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

float Player::hitboxRadius() const
{
    return kSwingRadius;
}

void Player::update(float dt)
{
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

    const sf::Vector2f desired = wish * kMaxSpeed;
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

    // Attack state machine. Swing locks out further swings until it finishes.
    if (m_attackState == AttackState::Idle &&
        m_actions.justPressed(Action::Attack)) {
        m_attackState = AttackState::Swinging;
        m_attackTimer = 0.f;
        ++m_swingId;
    }

    if (m_attackState == AttackState::Swinging) {
        m_attackTimer += dt;
        m_swingVisual.setPosition(hitboxPosition());
        m_swingVisual.setRotation(m_aimAngle * kRad2Deg);

        if (m_attackTimer >= kSwingDuration) {
            m_attackState = AttackState::Idle;
            m_attackTimer = 0.f;
        }
    }
}

void Player::confine(sf::Vector2f mn, sf::Vector2f mx)
{
    if (m_position.x < mn.x) { m_position.x = mn.x; m_velocity.x = std::max(m_velocity.x, 0.f); }
    if (m_position.x > mx.x) { m_position.x = mx.x; m_velocity.x = std::min(m_velocity.x, 0.f); }
    if (m_position.y < mn.y) { m_position.y = mn.y; m_velocity.y = std::max(m_velocity.y, 0.f); }
    if (m_position.y > mx.y) { m_position.y = mx.y; m_velocity.y = std::min(m_velocity.y, 0.f); }
    m_body.setPosition(m_position);
}

void Player::render(sf::RenderTarget& target) const
{
    target.draw(m_aimIndicator);
    target.draw(m_body);
    if (hitboxActive()) {
        target.draw(m_swingVisual);
    }
}

} // namespace hollow
