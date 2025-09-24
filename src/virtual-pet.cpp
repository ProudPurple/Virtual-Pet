#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
using namespace sf;
using namespace std;

struct Stats {
    int hunger;
    int mood;
    int money;
};

void save(Stats stats) {
    ofstream fout("stats.txt");
    fout << stats.hunger << endl
    << stats.mood << endl
    << stats.money << endl;
}

int main() {
    ifstream fin("stats.txt");
    if (!fin)
        cerr << "Fail " << filesystem::current_path() << endl;
    Stats stats;
    fin >> stats.hunger >> stats.mood >> stats.money;
    cout << stats.hunger << ' ' << stats.mood << ' ' << stats.money;

    RenderWindow window(VideoMode(Vector2u(600, 400)), "Virtual Pet", Style::None, State::Windowed);

    window.setFramerateLimit(60);
    window.setPosition({0,0});

    HWND hwnd = window.getNativeHandle();

    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);

    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);

    CircleShape circle(100.f);
    circle.setFillColor(Color::Green);

    while (window.isOpen()) {
        window.clear(Color::Black);
        window.draw(circle);
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                save(stats);
            }
        }
            
        if (Keyboard::isKeyPressed(Keyboard::Key::A))
            stats.hunger--;
    }
}
