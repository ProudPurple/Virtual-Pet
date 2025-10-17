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
    int frame = 0;
    while (true) {
        frame++;
        stats.hunger--;

        if (frame % 10 == 0)
            stats.money += 5;
        if (frame % 4 == 0)
            stats.mood -= 5;

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
    SetLayeredWindowAttributes(hwnd, RGB(255,0,255), 0, LWA_COLORKEY);

    Texture texture;
    if (!texture.loadFromFile("sprites/catNormal.png"))
        MessageBox(NULL, "cat1.png", "Error", MB_OK);
    RectangleShape spriteBase({200.f, 200.f});
    spriteBase.setTexture(&texture);
    

    Font font("bin/RasterForgeRegular-JpBgm.ttf");
    Text hText(font);
    hText.setString(to_string(stats.hunger));
    hText.setCharacterSize(20);
    hText.setFillColor(Color::Green);
    hText.setStyle(Text::Bold);
    Text dText(font);
    dText.setString(to_string(stats.hunger));
    dText.setCharacterSize(20);
    dText.setFillColor(Color::Green);
    dText.setStyle(Text::Bold);
    dText.setPosition(Vector2f(0, 30));
    

    while (window.isOpen()) {
        window.clear(Color(255,0,255));
        window.draw(spriteBase);
        window.draw(hText);
        window.draw(dText);
        hText.setString(to_string(stats.hunger));
        dText.setString(to_string(stats.money));
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                save(stats);
            }
            
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (spriteBase.getGlobalBounds().contains(mousePos)) {
                        stats.hunger++;
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {             
                if (Keyboard::isKeyPressed(Keyboard::Key::A))
                    stats.hunger -= 5;
            }
        }


        if (stats.hunger <= 0) {
            if (mood != "Mad") {
                static Texture madCatTexture;
                if (!madCatTexture.loadFromFile("sprites/catMad.png")) {
                    MessageBox(NULL, "catMad.png", "Error", MB_OK);
                } else {
                    spriteBase.setTexture(&madCatTexture);
                    mood = "Mad";
                }
            }
        }

    }
}


