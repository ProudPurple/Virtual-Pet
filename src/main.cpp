#include <Windows.h>
#include "globals.hpp"
#include "utilityManager.hpp"
#include "creationManager.hpp"
#include "windowManager.hpp"

using namespace sf;
using namespace std;

/*###############################################
#                    TODO:                      #
#              MINIGAME TIME!!!                 #      
#                  CHECKUP                      #
###############################################*/



int main() {
    creationManager creations;
    utilitiesManager utilities;
    windowManager windows;

    //Takes in saved stats from txt file
    utilities.takeInStats();
    stats.hash = utilities.hasher(stats.record);
    if (stats.hash != utilities.hasher(stats.record))
        return 0;
    
    //Can copy paste line for in context erroring
    //MessageBox(NULL, "hELLO", "Debug", MB_OK);

    //Make window and set some basic stuff
    RenderWindow window(VideoMode(Vector2u(300, 275)), "Virtual Pet", Style::None, State::Windowed);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setPosition({0, (int)VideoMode::getDesktopMode().size.y - 275});

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0,1,0), 0, LWA_COLORKEY);

    RectangleImage spriteBase = creations.defineRectangleImage("catNormal", Vector2f(250, 250), Vector2f(150, 125));
    RectangleImage cornerMove = creations.defineRectangleImage("cornerMove", Vector2f(50, 50), Vector2f(240, 40));
    RectangleImage shopButton = creations.defineRectangleImage("shopButton", Vector2f(50, 50), Vector2f(45, 85));
    RectangleImage tasksButton = creations.defineRectangleImage("tasksButton", Vector2f(50, 50), Vector2f(45, 155));
    RectangleImage barBase = creations.defineRectangleImage("hungerBar", Vector2f(115, 115), Vector2f(250, 120));
    RectangleImage hand = creations.defineRectangleImage("Hand", Vector2f(60,60),Vector2f(150,70));
    Animation petting;

    
    vector<RectangleShape> barHelpers(4, RectangleShape({30.f, 20.f}));
    for (int i = 0; i < 4; i++) {
        barHelpers[i] = creations.defineRectangle(30, 20, 250, 150 + (i * -20));
        barHelpers[i].setFillColor(stats.hunger > i * 25 ? DEFAULT_GREEN : DARK_GREEN);
    }

    Text hungerText = creations.defineText(20, 0, 0, LIGHT_GREEN, to_string(stats.hunger));
    Text moneyText = creations.defineText(20, 0, 30, LIGHT_GREEN, to_string(stats.money));

    if (stats.record[stats.record.size() - 1] - '0' == 0)
        stats.name = windows.enterName(window);
    if (stats.name == "CLOSE PLEASE")
        return 0;
    stats.record[stats.record.size() - 1] = '1';

    Text nameText = creations.defineText(30, 150, 10, DEFAULT_GREEN, stats.name);
    utilities.textRecenter(nameText, "middle");

    backgroundThread = thread(utilities.background);

    while (window.isOpen()) {
        window.clear(Color(0,1,0));
 
        if (totals.tick % 10)
            creations.mainSpriteControl(spriteBase.texture);
                    //MessageBox(NULL, stats.mood.c_str(), "Debug", MB_OK);
        window.draw(spriteBase.rectangle);
        for (RectangleShape rect : barHelpers) {
            window.draw(rect);
        }
        if (petting.start - totals.tick + petting.time >= 0) {
            int cur = petting.start - totals.tick + petting.time;
            //MessageBoxA(NULL, cur / 4 >= 15 || (cur / 4 < 10 && cur / 4 >= 5) ? "1" : "0", "D", MB_OK);
            Angle rot = hand.rectangle.getRotation();
            hand.rectangle.rotate(degrees(cur > 6 || cur < 3 ? 2 : -2));
            window.draw(hand.rectangle);
            if (cur == 0) {
                hand.rectangle.setRotation(degrees(0));
                petting.start = 0;
                petting.time = 0;
            }
        }
        window.draw(barBase.rectangle);
        window.draw(tasksButton.rectangle);
        window.draw(shopButton.rectangle);
        window.draw(nameText);


        hungerText.setString(to_string(stats.hunger));
        moneyText.setString(to_string(stats.money));

        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilities.close(window);
            }
            
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (shopButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        windows.shopMenu(window);
                    } else if (tasksButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        windows.taskMenu(window);
                    } else if (spriteBase.rectangle.getGlobalBounds().contains(mousePos) && !petting.start && stats.record[2] - '0') {
                        stats.happiness++;
                        petting.start = totals.tick, petting.time = 8;
                    }
                }
            }
        }
        utilities.barManager(barHelpers);
    }
    return 0;
}