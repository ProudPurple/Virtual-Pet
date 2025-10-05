#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
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
    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    fin >> stats.hunger >> stats.mood >> stats.money;
    //Can copy paste line for in context erroring
    //MessageBox(NULL, to_string(stats.hunger).c_str(), "Debug", MB_OK);

    //Make window and set some basic stuff
    RenderWindow window(VideoMode(Vector2u(600, 400)), "Virtual Pet", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setPosition({0,0});

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);

    Texture texture{"sprites/cat1.png", false, IntRect({-50,-50}, {100,100})};
    Sprite sprite(texture);

    Font font("bin/RasterForgeRegular-JpBgm.ttf");
    Text text(font);
    text.setString(to_string(stats.hunger));
    text.setCharacterSize(20);
    text.setFillColor(Color::Green);
    text.setStyle(Text::Bold);

    while (window.isOpen()) {
        window.clear(Color::Black);
        window.draw(sprite);
        window.draw(text);
        text.setString(to_string(stats.hunger));
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
