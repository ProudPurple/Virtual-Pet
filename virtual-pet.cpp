#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Event Test");

    while (window.isOpen()) {
        sf::Event event;              // default constructor is valid
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}
