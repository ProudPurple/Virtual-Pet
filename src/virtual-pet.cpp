#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace sf;
using namespace std;




//DO MORE CONDITION STUFF LIKE BAR STUFF AND DO MOOD TEXTURE FUNCTION WOOHOO

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

void barManagment(Stats stats, vector<RectangleShape> &barHelpers) {
    if (stats.hunger <= 0)
        barHelpers[0].setFillColor(Color(5,51,6));
    else if (stats.hunger <= 25)
        barHelpers[1].setFillColor(Color(5,51,6));
    else if (stats.hunger <= 50)
        barHelpers[2].setFillColor(Color(5,51,6));
    else if (stats.hunger <= 75)
        barHelpers[3].setFillColor(Color(5,51,6));

    if (stats.hunger > 75)
        barHelpers[3].setFillColor(Color(18,219,18));
    else if (stats.hunger > 50)
        barHelpers[2].setFillColor(Color(18,219,18));
    else if (stats.hunger > 25)
        barHelpers[1].setFillColor(Color(18,219,18));
    else if (stats.hunger > 0)
        barHelpers[0].setFillColor(Color(18,219,18));
}

Texture currentTexture(string mood) {
    Texture texture;
    if (mood == "Mad") {
        if (!texture.loadFromFile("sprites/catMad.png"))
            MessageBox(NULL, "Mad Sprite Texture", "Load Error", MB_OK);
        return texture;
    } else {
        if (!texture.loadFromFile("sprites/catNormal.png"))
            MessageBox(NULL, "Base Sprite Texture", "Load Error", MB_OK);
        return texture;
    }

} 

int main() {
    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    string mood = "Normal";
    fin >> stats.hunger >> stats.mood >> stats.money;

    thread hungerDecay(statDecay, ref(stats));
    //Can copy paste line for in context erroring
    //MessageBox(NULL, to_string(stats.hunger).c_str(), "Debug", MB_OK);

    //Make window and set some basic stuff
    RenderWindow window(VideoMode(Vector2u(600, 400)), "Virtual Pet", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setPosition({0, (int)VideoMode::getDesktopMode().size.y - 400});

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(255,0,255), 0, LWA_COLORKEY);

    Texture spriteTexture = currentTexture(mood);
    RectangleShape spriteBase({200.f, 200.f});
    spriteBase.setTexture(&spriteTexture);
    spriteBase.setOrigin({100.f,100.f});
    spriteBase.setPosition({75.f,275.f});


    Texture barTexture;
    if (!barTexture.loadFromFile("sprites/happinessBar.png"))
        MessageBox(NULL, "Happiness Bar Texture", "Load Error", MB_OK);
    RectangleShape barBase({200.f, 200.f});
    barBase.setOrigin({100.f, 100.f});
    barBase.setPosition({400.f, 200.f});
    barBase.setTexture(&barTexture);
    vector<RectangleShape> barHelpers(4, RectangleShape({40.f, 32.f}));
    for (int i = 0; i < 4; i++) {
        barHelpers[i].setOrigin({20.f,16.f});
        barHelpers[i].setPosition({400.f,247.f + (i * -32.f)});
        barHelpers[i].setFillColor(Color(18,219,18));
    }

    Font font("RasterForgeRegular-JpBgm.ttf");
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
        for (RectangleShape rect : barHelpers) {
            window.draw(rect);
        }
        window.draw(barBase);
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

        barManagment(stats, barHelpers);
        
        if (stats.hunger > 0 && mood != "Normal") {
            mood = "Normal";
            spriteTexture = currentTexture(mood);
            spriteBase.setTexture(&spriteTexture);
        } else if (stats.hunger <= 0 && mood != "Mad") {
            mood = "Mad";
            spriteTexture = currentTexture(mood);
            spriteBase.setTexture(&spriteTexture);
        }
    }
}


