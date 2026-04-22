#include "entity/Player.h"

#include "input/ActionMap.h"
#include "input/InputState.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>
#include <numbers>

namespace hollow {

namespace {
    constexpr float kMaxSpeed = 260.f; // px/s cap
    constexpr float kAccel    = 1700.f; // how fast the Shard reaches top speed
    constexpr float kDrag     = 1300.f; // how fast momentum bleeds when idle
    constexpr float kRadius   = 14.f;
}

Player::Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions)
    : Entity(startPosition)
    , m_body(kRadius)
    , m_aimIndicator({ 22.f, 3.f })
    , m_input(input)
    , m_actions(actions)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setFillColor(sf::Color(230, 228, 240));
    m_body.setOutlineColor(sf::Color(70, 40, 100));
    m_body.setOutlineThickness(2.f);
    m_body.setPosition(m_position);

    // Origin at the left-center of the bar so it pivots from the Shard's core.
    m_aimIndicator.setOrigin(0.f, 1.5f);
    m_aimIndicator.setFillColor(sf::Color(200, 170, 80));
}

void Player::update(float dt)
{
    sf::Vector2f wish{};
    if (m_actions.isDown(Action::MoveUp))    wish.y -= 1.f;
    if (m_actions.isDown(Action::MoveDown))  wish.y += 1.f;
    if (m_actions.isDown(Action::MoveLeft))  wish.x -= 1.f;
    if (m_actions.isDown(Action::MoveRight)) wish.x += 1.f;

    // Normalize so diagonal movement isn't ~41% faster than cardinal.
    const float wishLen2 = wish.x * wish.x + wish.y * wish.y;
    if (wishLen2 > 0.f) {
        const float inv = 1.f / std::sqrt(wishLen2);
        wish.x *= inv;
        wish.y *= inv;
    }

    // Seek toward desired velocity. Accel when there's input, drag when not —
    // gives the Shard weight without feeling sluggish.
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

    // Aim toward the cursor. No camera yet, so screen-space == world-space.
    const auto mouse = m_input.mousePosition();
    m_aimAngle = std::atan2(mouse.y - m_position.y, mouse.x - m_position.x);

    m_aimIndicator.setPosition(m_position);
    m_aimIndicator.setRotation(m_aimAngle * 180.f / std::numbers::pi_v<float>);
}

void Player::render(sf::RenderTarget& target) const
{
    target.draw(m_aimIndicator);
    target.draw(m_body);
}

} // namespace hollow
