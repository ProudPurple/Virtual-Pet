#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>
#include <thread>
#include <chrono>
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

void statDecay(Stats& stats) {
    while (true) {
        stats.hunger--;
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    fin >> stats.hunger >> stats.mood >> stats.money;

    string mood = "Happy";

    thread hungerDecay(statDecay, ref(stats));
    //Can copy paste line for in context erroring
    //MessageBox(NULL, to_string(stats.hunger).c_str(), "Debug", MB_OK);

    //Make window and set some basic stuff
    RenderWindow window(VideoMode(Vector2u(600, 400)), "Virtual Pet", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setPosition({0,0});
    window.setKeyRepeatEnabled(false);

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), 0, LWA_COLORKEY);

    Texture texture;
    if (!texture.loadFromFile("sprites/cat1.png"))
        MessageBox(NULL, "cat1.png", "Error", MB_OK);
    RectangleShape spriteBase({200.f, 200.f});
    spriteBase.setTexture(&texture);

    Font font("bin/RasterForgeRegular-JpBgm.ttf");
    Text text(font);
    text.setString(to_string(stats.hunger));
    text.setCharacterSize(20);
    text.setFillColor(Color::Green);
    text.setStyle(Text::Bold);

    while (window.isOpen()) {
        window.clear(Color::Black);
        window.draw(spriteBase);
        window.draw(text);
        text.setString(to_string(stats.hunger));
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                save(stats);
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {             
                if (Keyboard::isKeyPressed(Keyboard::Key::A))
                    stats.hunger -= 5;
            }
        }
        if (stats.hunger <= 0) {
            if (mood != "Mad") {
                static Texture madCatTexture;
                if (!madCatTexture.loadFromFile("sprites/cat2.png")) {
                    MessageBox(NULL, "cat2.png", "Error", MB_OK);
                } else {
                    spriteBase.setTexture(&madCatTexture);
                    mood = "Mad";
                }
            }
        }

    }
}


