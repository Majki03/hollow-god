#include "world/World.h"

#include "entity/Entity.h"

#include <algorithm>

namespace hollow {

World::World()  = default;
World::~World() = default;

void World::add(std::unique_ptr<Entity> entity)
{
    m_entities.push_back(std::move(entity));
}

void World::update(float dt)
{
    for (auto& e : m_entities) {
        e->update(dt);
    }
}

void World::render(sf::RenderTarget& target) const
{
    for (const auto& e : m_entities) {
        e->render(target);
    }
}

void World::pruneDead()
{
    std::erase_if(m_entities, [](const auto& e) { return !e->alive(); });
}

} // namespace hollow
