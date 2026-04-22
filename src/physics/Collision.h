#pragma once

#include <SFML/System/Vector2.hpp>

namespace hollow::physics {

// Circle-vs-circle overlap test. Squared distance avoids a sqrt and is fine
// since we only care about the boolean. Inline: hot path, tiny body.
inline bool circlesOverlap(sf::Vector2f a, float ra, sf::Vector2f b, float rb)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float r  = ra + rb;
    return (dx * dx + dy * dy) <= (r * r);
}

} // namespace hollow::physics
