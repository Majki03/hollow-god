#include "scene/TitleScene.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <cmath>
#include <cstdint>

namespace hollow {

TitleScene::TitleScene(SceneContext& ctx)
    : Scene(ctx)
    , m_banner(sf::Vector2f(480.f, 4.f))
{
    m_banner.setOrigin(240.f, 2.f);
    m_banner.setPosition(640.f, 360.f);
    m_banner.setFillColor(sf::Color(200, 170, 80));
}

void TitleScene::handleEvent(const sf::Event& /*event*/)
{
    // Scene transitions land in a later commit once GameScene exists.
}

void TitleScene::update(float dt)
{
    m_time += dt;

    // Slow pulse on the banner — proof that the fixed-timestep loop is alive.
    const float alpha = 0.55f + 0.45f * std::sin(m_time * 2.f);
    sf::Color c = m_banner.getFillColor();
    c.a = static_cast<std::uint8_t>(alpha * 255.f);
    m_banner.setFillColor(c);
}

void TitleScene::render(sf::RenderTarget& target)
{
    target.draw(m_banner);
}

} // namespace hollow
