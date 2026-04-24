#include "Application.h"

#include "scene/TitleScene.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace hollow {

namespace {
    // 60 Hz sim tick. Render runs every frame and is vsync-capped so motion
    // stays smooth on high-refresh displays without the simulation drifting.
    constexpr float kFixedDt = 1.f / 60.f;
}

Application::Application()
    : m_window(sf::VideoMode(1280, 720), "The Hollow God")
    , m_actions(m_input)
    , m_ctx{ m_window, m_input, m_actions, m_scenes, m_textures, m_fonts, "", m_audio, m_data }
{
    m_window.setVerticalSyncEnabled(true);

    m_data.load(HG_DATA_DIR);

    // Best-effort font load. Missing font means text is absent, not a crash.
    // TODO: bundle a proper font asset in assets/fonts/ and remove HG_DEV_FONT.
    try {
        m_fonts.get(HG_DEV_FONT);
        m_ctx.fontKey = HG_DEV_FONT;
    } catch (...) {}

    m_scenes.push(std::make_unique<TitleScene>(m_ctx));
}

int Application::run()
{
    sf::Clock clock;
    float accumulator = 0.f;

    while (m_window.isOpen()) {
        accumulator += clock.restart().asSeconds();

        processEvents();

        while (accumulator >= kFixedDt) {
            update(kFixedDt);
            accumulator -= kFixedDt;
        }

        render();
    }

    return 0;
}

void Application::processEvents()
{
    m_input.beginFrame();

    sf::Event event{};
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        m_input.onEvent(event);
        m_scenes.handleEvent(event);
    }
}

void Application::update(float dt)
{
    m_scenes.update(dt);
}

void Application::render()
{
    m_window.clear(sf::Color(10, 8, 14));
    m_scenes.render(m_window);
    m_window.display();
}

} // namespace hollow
