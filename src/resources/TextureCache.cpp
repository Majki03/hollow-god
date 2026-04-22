#include "resources/TextureCache.h"

#include <stdexcept>

namespace hollow {

const sf::Texture& TextureCache::get(const std::string& path)
{
    if (auto it = m_textures.find(path); it != m_textures.end()) {
        return *it->second;
    }

    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(path)) {
        // Fail loud. A missing asset in debug is always a developer bug; no
        // point rendering magenta and hoping someone notices in QA.
        throw std::runtime_error("texture load failed: " + path);
    }

    auto [it, _] = m_textures.emplace(path, std::move(tex));
    return *it->second;
}

} // namespace hollow
