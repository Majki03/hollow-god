#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace hollow {

class Enemy : public Entity {
public:
    explicit Enemy(sf::Vector2f position);

    void  damage(int amount);
    int   hp() const     { return m_hp; }
    float radius() const { return kRadius; }

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::RectangleShape m_body;
    int                m_hp = kMaxHp;

    static constexpr int   kMaxHp  = 20;
    static constexpr float kRadius = 18.f; // circle approx for collision
};

} // namespace hollow
