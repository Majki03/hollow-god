#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace sf { class RenderTarget; }

namespace hollow {

class Entity;

// Owns every live entity in the current play session. Scenes drive it:
// update() ticks everything, pruneDead() reaps whatever got killed this frame,
// render() draws in insertion order. No spatial index yet — we'll add one when
// we actually have enough entities to feel the O(n²) cost.
class World {
public:
    World();
    ~World();

    void add(std::unique_ptr<Entity> entity);

    void update(float dt);
    void render(sf::RenderTarget& target) const;
    void pruneDead();

    std::size_t size() const { return m_entities.size(); }

private:
    std::vector<std::unique_ptr<Entity>> m_entities;
};

} // namespace hollow
