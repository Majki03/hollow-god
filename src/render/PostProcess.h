#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace sf {
    class RenderTarget;
    class RenderWindow;
}

namespace hollow {

// Off-screen render target + GLSL composite shader.
//
// Usage each frame:
//   1. Render all scenes to sceneTarget(window).
//   2. Call composite(window, time) to blit with the shader.
//
// If GLSL shaders are unavailable (some drivers/VMs), sceneTarget() returns
// the window directly and composite() is a no-op — the game still runs, just
// without post-processing.
class PostProcess {
public:
    PostProcess(unsigned width, unsigned height);

    // Returns the offscreen texture when active, the window as a plain fallback.
    sf::RenderTarget& sceneTarget(sf::RenderWindow& fallback);

    // Finalise the offscreen texture and draw it to the window with the shader.
    void composite(sf::RenderWindow& window, float time);

    // Trauma level (0–1) drives chromatic aberration intensity.
    // GameScene calls this each frame after updating m_shakeTrauma.
    void setTrauma(float t) { m_trauma = t; }

    bool active() const { return m_active; }

private:
    bool              m_active = false;
    sf::RenderTexture m_tex;
    sf::Shader        m_shader;
    sf::Sprite        m_sprite;
    float             m_trauma = 0.f;
};

} // namespace hollow
