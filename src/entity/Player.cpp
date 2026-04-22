#include "entity/Player.h"

#include "input/ActionMap.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <cmath>

namespace hollow {

namespace {
    constexpr float kSpeed  = 260.f; // px/s
    constexpr float kRadius = 14.f;
}

Player::Player(sf::Vector2f startPosition, const ActionMap& actions)
    : Entity(startPosition)
    , m_body(kRadius)
    , m_actions(actions)
{
    m_body.setOrigin(kRadius, kRadius);
    m_body.setFillColor(sf::Color(230, 228, 240));
    m_body.setOutlineColor(sf::Color(70, 40, 100));
    m_body.setOutlineThickness(2.f);
    m_body.setPosition(m_position);
}

void Player::update(float dt)
{
    sf::Vector2f dir{};
    if (m_actions.isDown(Action::MoveUp))    dir.y -= 1.f;
    if (m_actions.isDown(Action::MoveDown))  dir.y += 1.f;
    if (m_actions.isDown(Action::MoveLeft))  dir.x -= 1.f;
    if (m_actions.isDown(Action::MoveRight)) dir.x += 1.f;

    // Normalize so diagonal movement isn't ~41% faster than cardinal.
    const float len2 = dir.x * dir.x + dir.y * dir.y;
    if (len2 > 0.f) {
        const float inv = 1.f / std::sqrt(len2);
        dir.x *= inv;
        dir.y *= inv;
    }

    m_position += dir * (kSpeed * dt);
    m_body.setPosition(m_position);
}

void Player::render(sf::RenderTarget& target) const
{
    target.draw(m_body);
}

} // namespace hollow
