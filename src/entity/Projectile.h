#pragma once

#include "entity/Entity.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace hollow {

// Travelling projectile. Used for both enemy arrows (pierces=false, damage=0
// so GameScene uses kContactDamage) and player weapon shots (damage set
// per-weapon, piercing for charged bow / thrown spear).
class Projectile : public Entity {
public:
    // Default color / radius matches the original enemy arrow.
    Projectile(sf::Vector2f origin, sf::Vector2f direction,
               float speed = 200.f, int damage = 0, bool pierces = false,
               sf::Color color = sf::Color(240, 160, 60), float radius = kDefaultRadius);

    float        radius()  const { return m_radius; }
    sf::Vector2f travelDir() const { return m_dir; }
    int          damage()  const { return m_damage; }
    bool         pierces() const { return m_pierces; }

    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    sf::CircleShape m_body;
    sf::Vector2f    m_dir;
    float           m_speed;
    float           m_radius;
    int             m_damage;
    bool            m_pierces;
    float           m_traveled = 0.f;

    static constexpr float kDefaultRadius = 5.f;
    static constexpr float kMaxRange      = 780.f;
};

} // namespace hollow
