#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace hollow {

// Lazy loader for any SFML resource with loadFromFile(const std::string&).
// Texture, Font, and SoundBuffer all share the contract — one template
// covers them instead of duplicating the same bookkeeping three times.
template <typename T>
class ResourceCache {
public:
    const T& get(const std::string& path)
    {
        if (auto it = m_items.find(path); it != m_items.end()) {
            return *it->second;
        }

        auto res = std::make_unique<T>();
        if (!res->loadFromFile(path)) {
            // Fail loud — a missing asset in dev is always a bug.
            throw std::runtime_error("resource load failed: " + path);
        }

        auto [inserted, _] = m_items.emplace(path, std::move(res));
        return *inserted->second;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<T>> m_items;
};

using TextureCache     = ResourceCache<sf::Texture>;
using FontCache        = ResourceCache<sf::Font>;
using SoundBufferCache = ResourceCache<sf::SoundBuffer>;

} // namespace hollow
