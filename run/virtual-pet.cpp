#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(400, 300)), "Test Window");
    while (window.isOpen()) {
        window.clear(sf::Color::Red);
        window.display();
    }
}