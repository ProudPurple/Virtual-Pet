#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;
int main() {
    RenderWindow window(VideoMode(Vector2u(600, 400)), "Test Window", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setPosition({0,0});
    while (window.isOpen()) {
        window.clear(Color::Red);
        window.display();
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                window.close();
            if (event->is<Event::MouseButtonPressed>())
                window.clear(Color::Blue);
        }
    }
}