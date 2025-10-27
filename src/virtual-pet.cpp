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

struct Stats {
    int hunger;
    int money;
    string mood;
};

const Font font("RasterForgeRegular-JpBgm.ttf");
const Color defaultGreen = Color(34,177,76);
const Color lightGreen = Color(18, 219, 18);
const Color darkGreen = Color(5, 51, 6);

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

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void textureDefine(Texture& texture, string filePath) {
    if (!texture.loadFromFile("sprites/" + filePath))
        MessageBox(NULL, (filePath + " Load Fail").c_str(), "Texture Error", MB_OK);
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

RectangleShape createRectangle(const Texture& texture, float height, float width, float x, float y) {
    RectangleShape rect({height, width});
    rect.setOrigin({height/2, width/2});
    rect.setPosition({x, y});
    rect.setTexture({&texture});
    return rect;
}

Text createText(int charSize, float x, float y, Color color, String s) {
    Text txt(font);
    txt.setString(s);
    txt.setCharacterSize(charSize);
    txt.setPosition({x,y});
    txt.setFillColor(color);
    return txt;
}

Texture currentTexture(string mood) {
    Texture texture;
    if (mood == "Mad")
        textureDefine(texture, "catMad.png");
    else
        textureDefine(texture, "catNormal.png");
    return texture;
} 

string enterName(RenderWindow& window) {
    bool typing = true;
    Text nameText = createText(20, 200, 200, Color::Green, "");
    Text namePrompt = createText(25, 100, 100, Color::Green, "What is Your Pets Name?");
    while (window.isOpen() && typing) {
        window.clear(Color(255,0,255));
        window.draw(nameText);
        window.draw(namePrompt);
        window.display();

        while (const optional event = window.pollEvent()) {
            if (const auto* textEntered = event->getIf<Event::TextEntered>()) {
                char text = textEntered->unicode;
                if (text == 8 && nameText.getString().getSize() > 0) {
                    string temp = nameText.getString();
                    temp.pop_back();
                    nameText.setString(temp);
                } else if (text == 13) {
                    typing = false;
                } else if (nameText.getString().getSize() <= 10 && text != 8) {
                    nameText.setString(nameText.getString() + text);
                }
            }
        }
    }
    return nameText.getString();
}

void shopMenu(Stats stats) {

}


int main() {
    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    fin >> stats.hunger >> stats.mood >> stats.money;

    //Can copy paste line for in context erroring
    //MessageBox(NULL, "hELLO".c_str(), "Debug", MB_OK);

    //Make window and set some basic stuff
    RenderWindow window(VideoMode(Vector2u(300, 275)), "Virtual Pet", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setPosition({0, (int)VideoMode::getDesktopMode().size.y - 275});

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(255,0,255), 0, LWA_COLORKEY);

    Texture spriteTexture = currentTexture(stats.mood);
    RectangleShape spriteBase = createRectangle(spriteTexture, 250, 250, 150, 125);

    Texture shopButtonTexture;
    textureDefine(shopButtonTexture, "shopButton.png");
    Texture tasksButtonTexture;
    textureDefine(tasksButtonTexture, "tasksButton.png");
    Texture barTexture;
    textureDefine(barTexture, "happinessBar.png");
    Texture cornerMoveTexture;
    textureDefine(cornerMoveTexture, "cornerMove.png");

    RectangleShape cornerMove = createRectangle(cornerMoveTexture, 50, 50, 240, 40);
    RectangleShape shopButton = createRectangle(shopButtonTexture, 100, 100, 45, 85);
    RectangleShape tasksButton = createRectangle(tasksButtonTexture, 100, 100, 45, 155);
    RectangleShape barBase = createRectangle(barTexture, 115, 115, 280, 165);
    vector<RectangleShape> barHelpers(4, RectangleShape({30.f, 24.f}));
    for (int i = 0; i < 4; i++) {
        barHelpers[i].setOrigin({15.f,12.f});
        barHelpers[i].setPosition({295.f,200.f + (i * -24.f)});
        barHelpers[i].setFillColor(stats.hunger > i * 25 ? Color(18,219,18) : Color(5, 51, 6));
    }

    Text hungerText = createText(20, 0, 0, lightGreen, to_string(stats.hunger));
    Text moneyText = createText(20, 0, 30, lightGreen, to_string(stats.money));
    Text nameText = createText(20, 0, 60, lightGreen, enterName(window));

    thread hungerDecay(statDecay, ref(stats));

    while (window.isOpen()) {
        window.clear(Color(255,0,255));
        window.draw(spriteBase);
        for (RectangleShape rect : barHelpers) {
            window.draw(rect);
        }
        window.draw(barBase);
        window.draw(tasksButton);
        window.draw(nameText);
        window.draw(shopButton);
        window.draw(hungerText);
        window.draw(moneyText);

        hungerText.setString(to_string(stats.hunger));
        moneyText.setString(to_string(stats.money));

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
                    } else if (shopButton.getGlobalBounds().contains(mousePos)) {
                        thread shopWindow(shopMenu, ref(stats));
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {           
                if (Keyboard::isKeyPressed(Keyboard::Key::A))
                    stats.hunger -= 5;
            }
        }

        barManagment(stats, barHelpers);
        
        if (stats.hunger > 0 && stats.mood != "Normal") {
            stats.mood = "Normal";
            spriteTexture = currentTexture(stats.mood);
            spriteBase.setTexture(&spriteTexture);
        } else if (stats.hunger <= 0 && stats.mood != "Mad") {
            stats.mood = "Mad";
            spriteTexture = currentTexture(stats.mood);
            spriteBase.setTexture(&spriteTexture);
        }
    }
}


