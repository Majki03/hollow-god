#include "render/PostProcess.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <iostream>

namespace hollow {

PostProcess::PostProcess(unsigned w, unsigned h)
{
    if (!sf::Shader::isAvailable()) {
        std::cerr << "[PostProcess] shaders unavailable — running without post-processing\n";
        return;
    }

    if (!m_tex.create(w, h)) {
        std::cerr << "[PostProcess] failed to create " << w << "x" << h << " render texture\n";
        return;
    }

    // Fragment-only shader; SFML supplies the passthrough vertex stage.
    if (!m_shader.loadFromFile(HG_SHADER_DIR "/post.frag", sf::Shader::Fragment)) {
        std::cerr << "[PostProcess] failed to load post.frag\n";
        return;
    }

    // CurrentTexture binds the sprite's texture automatically at draw time,
    // so we don't need to update this uniform every frame.
    m_shader.setUniform("texture", sf::Shader::CurrentTexture);

    // Point the sprite at the render texture. The texture object lives as long
    // as PostProcess does, so the pointer stays valid for the whole run.
    m_sprite.setTexture(m_tex.getTexture());

    m_active = true;
}

sf::RenderTarget& PostProcess::sceneTarget(sf::RenderWindow& fallback)
{
    return m_active
        ? static_cast<sf::RenderTarget&>(m_tex)
        : static_cast<sf::RenderTarget&>(fallback);
}

void PostProcess::composite(sf::RenderWindow& window, float time)
{
    if (!m_active) return;

    // Finalise the texture — must be called before sampling it in the shader.
    m_tex.display();

    m_shader.setUniform("uTime",   time);
    m_shader.setUniform("uTrauma", m_trauma);

    window.draw(m_sprite, &m_shader);
}

} // namespace hollow
