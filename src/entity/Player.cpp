#include "entity/Player.h"

#include "input/ActionMap.h"
#include "input/InputState.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace hollow {

namespace {
    constexpr float kAccel  = 1700.f;
    constexpr float kDrag   = 1300.f;
    constexpr float kRadius = 14.f;

    constexpr float kRad2Deg = 180.f / std::numbers::pi_v<float>;
}

Player::Player(sf::Vector2f startPosition, const InputState& input, const ActionMap& actions)
    : Entity(startPosition)
    , m_body(kRadius)
    , m_aimIndicator({ 22.f, 3.f })
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

void Player::setWeapon(std::unique_ptr<Weapon> w)
{
    m_weapon = std::move(w);
}

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
    if (m_iframeTimer   > 0.f) m_iframeTimer   -= dt;
    if (m_dashCooldown  > 0.f) m_dashCooldown  -= dt;
    if (m_echoStepTimer > 0.f) m_echoStepTimer -= dt;

    {
        const bool visible = m_iframeTimer <= 0.f ||
                             static_cast<int>(m_iframeTimer / 0.1f) % 2 == 0;
        m_body.setFillColor(visible ? sf::Color(230, 228, 240)
                                    : sf::Color(230, 228, 240, 80));
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

    const float speed = (m_stats.hasEchoStep && m_echoStepTimer > 0.f)
                        ? m_stats.moveSpeed * 1.5f : m_stats.moveSpeed;
    const sf::Vector2f desired = wish * speed;
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

    m_dashedThisFrame = false;

    if (m_actions.justPressed(Action::Dash) && m_dashCooldown <= 0.f) {
        sf::Vector2f dashDir{};
        if (wishLen2 > 0.f) {
            const float inv = 1.f / std::sqrt(wishLen2);
            dashDir = { wish.x * inv, wish.y * inv };
        } else {
            dashDir = { std::cos(m_aimAngle), std::sin(m_aimAngle) };
        }
        m_dashOrigin      = m_position;
        m_position       += dashDir * m_stats.dashDist;
        m_dashCooldown    = m_stats.dashCooldown;
        m_iframeTimer     = std::max(m_iframeTimer, 0.20f);
        m_dashedThisFrame = true;
        if (m_stats.hasEchoStep) m_echoStepTimer = 2.f;
        m_body.setPosition(m_position);
    }

    const auto mouse = m_input.mousePosition();
    m_aimAngle = std::atan2(mouse.y - m_position.y, mouse.x - m_position.x);

    m_aimIndicator.setPosition(m_position);
    m_aimIndicator.setRotation(m_aimAngle * kRad2Deg);

    if (m_weapon)
        m_weapon->update(dt, m_position, m_aimAngle, m_actions);
}

void Player::render(sf::RenderTarget& target) const
{
    target.draw(m_aimIndicator);
    target.draw(m_body);
    if (m_weapon)
        m_weapon->render(target);
}

} // namespace hollow
