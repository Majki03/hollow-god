#pragma once

#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace hollow {

// Lazy, reference-stable texture store. First get() loads from disk, every
// subsequent call returns the same cached texture. Textures live in unique_ptrs
// so returned references stay valid even if the underlying map ever changes
// container (it's currently unordered_map, but that assumption shouldn't leak).
class TextureCache {
public:
    const sf::Texture& get(const std::string& path);

private:
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};

} // namespace hollow
