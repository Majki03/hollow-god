#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf { class RenderTarget; }

namespace hollow {

// Thin base for any game-world actor (Player, Enemy, Projectile). Deliberately
// bare: just a position, an alive flag, and the update/render contract.
// Scale/rotation/collider live on subclasses until we have a real reason to
// hoist them.
class Entity {
public:
    Entity() = default;
    explicit Entity(sf::Vector2f position) : m_position(position) {}
    virtual ~Entity() = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderTarget& target) const = 0;

    sf::Vector2f position() const       { return m_position; }
    void         setPosition(sf::Vector2f p) { m_position = p; }

    bool alive() const { return m_alive; }
    void kill()        { m_alive = false; }

protected:
    sf::Vector2f m_position{};
    bool         m_alive = true;
};

} // namespace hollow
