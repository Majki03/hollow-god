#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "The Hollow God");

    while (window.isOpen()) {
        window.clear();
        window.display();
    }

    return 0;
}
