#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

namespace {
    // 60 Hz sim tick. Chosen early so gameplay numbers (speeds, cooldowns) can
    // be reasoned about in "ticks" rather than variable frame time.
    constexpr float kFixedDt = 1.f / 60.f;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Hollow God");
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;
    float accumulator = 0.f;

    while (window.isOpen()) {
        accumulator += clock.restart().asSeconds();

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        while (accumulator >= kFixedDt) {
            // update(kFixedDt) — gameplay lands in Phase 3.
            accumulator -= kFixedDt;
        }

        window.clear(sf::Color(10, 8, 14));
        window.display();
    }

    return 0;
}
