#include <Windows.h>
#include "globals.hpp"
#include "utilityManager.hpp"
#include "creationManager.hpp"
#include "windowManager.hpp"

using namespace sf;
using namespace std;

int main() {
    creationManager creations;
    utilitiesManager utilities;
    windowManager windows;
    Image icon;
    if (!icon.loadFromFile("bin/virtualPet.png"))
        MessageBoxA(NULL, "No More Icon", "Erro", MB_OK);
    window.setIcon(icon);
    
    //Takes in saved stats from txt file
    utilities.takeInStats();
    stats.hash = utilities.hasher(stats.record);
    utilities.save();
    if (stats.hash != utilities.hasher(stats.record))
        return 0;

    creations.playSound("happy");
    lastClick = totals.tick;
    bool tutTrigger = false;
    int hungerBacklog = 0, pastHunger = stats.hunger, hungerDifference = 0, tutDisplay = totals.time + 5;
    
    //Can copy paste line for in context erroring
    //MessageBox(NULL, "hELLO", "Debug", MB_OK);

    //Make window and set some basic stuff
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setPosition({0, (int)VideoMode::getDesktopMode().size.y - 275});

    //Make window background clear
    HWND hwnd = window.getNativeHandle();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0,1,0), 0, LWA_COLORKEY);
    
    if (stats.record[stats.record.size() - 2] == '0')
        windows.petType();
    if (stats.type == "N/A")
        return 0;
    stats.record[stats.record.size() - 2] = '1';

    RectangleImage spriteBase = creations.defineRectangleImage(stats.type + "Normal", Vector2f(250, 250), Vector2f(150, 125));
    RectangleImage cornerMove = creations.defineRectangleImage("cornerMove", Vector2f(50, 50), Vector2f(240, 40));
    RectangleImage shopButton = creations.defineRectangleImage("shopButton", Vector2f(50, 50), Vector2f(45, 85));
    RectangleImage tasksButton = creations.defineRectangleImage("tasksButton", Vector2f(50, 50), Vector2f(45, 155));
    RectangleImage barBase = creations.defineRectangleImage("hungerBar", Vector2f(115, 115), Vector2f(250, 120));
    RectangleImage hand = creations.defineRectangleImage("Hand", Vector2f(60,60),Vector2f(150,70));
    Animation petting;

    RectangleShape hungerBar = creations.defineRectangle(30,75,250,80);
    hungerBar.setFillColor(LIGHT_GREEN);
    hungerBar.setOrigin(Vector2f(15,75));
    hungerBar.rotate(degrees(180));

    Text tutorial = creations.defineText(15, 150, 30, LIGHT_GREEN, "    Hi! Lets explore what\n you can do with your pet");
    utilities.textRecenter(tutorial, "middle");
    
    if (stats.record[stats.record.size() - 1] - '0' == 0)
        stats.name = windows.enterName();
    if (stats.name == "N/A")
        return 0;

    stats.record[stats.record.size() - 1] = '1';

    Text nameText = creations.defineText(30, 150, 10, DEFAULT_GREEN, stats.name);
    utilities.textRecenter(nameText, "middle");

    backgroundThread = thread(utilities.background);
    utilities.save();

    while (window.isOpen()) {
        if (stats.record[stats.record.size() - 5] == '1') {
            tutorial.setString("Go to the tasks menu");
            utilities.textRecenter(tutorial, "middle");
        } else if (totals.time > tutDisplay) {
            if (!tutTrigger) {
                tutorial.setString("  The bar on the right is\n hunger its looking low");
                tutTrigger = true;
                tutDisplay += 4;
            } else
                tutorial.setString("  We can fix that with the shop\n             hit the button\n          on the left to go");
            utilities.textRecenter(tutorial, "middle");
        }
        sleepy = stats.record[5] - '0' && totals.tick - lastClick >= 180 || sleepy;
        window.clear(Color(0,1,0));
 
        if (totals.tick % 10)
            creations.defineTexture(spriteBase.texture, stats.type+(stats.mood == "Normal" && stats.record[8] - '0' ? "Rich" : stats.mood));

        hungerDifference = stats.hunger - hungerBar.getSize().y * 4 / 3;
        hungerBar.setSize(Vector2f(30, hungerBar.getSize().y + (hungerDifference > 0 ? min(1, hungerDifference) : max(-1, hungerDifference))));

        if (sleepy) {
            creations.defineTexture(spriteBase.texture, stats.type + "Sleepy");
            creations.defineTexture(tasksButton.texture, "sleepyButton");
            creations.defineTexture(shopButton.texture, "sleepyButton");
            hungerBacklog = (totals.tick - lastClick) / 20;
            pastHunger = max(pastHunger, stats.hunger);
            stats.hunger = pastHunger;
        } else {
            creations.defineTexture(tasksButton.texture, "tasksButton");
            creations.defineTexture(shopButton.texture, "shopButton");
        }

        window.draw(spriteBase.rectangle);
        
        if (petting.time >= 0) {
            petting.time = petting.start - totals.tick + petting.time;
            if (petting.time >= 24)
                hand.rectangle.rotate(degrees(2));
            else if (petting.time >= 9)
                hand.rectangle.rotate(degrees(-2));
            else
                hand.rectangle.rotate(degrees(2));
            window.draw(hand.rectangle);
            if (petting.time <= 0) {
                hand.rectangle.setRotation(degrees(0));
                petting.start = 0;
                petting.time = -1;
            }
        }

        window.draw(hungerBar);
        window.draw(barBase.rectangle);
        window.draw(tasksButton.rectangle);
        window.draw(shopButton.rectangle);

        if (stats.record[stats.record.size() - 4] == '0')
            window.draw(tutorial);
        else
            window.draw(nameText);

        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilities.close(window);
            }
            
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (shopButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        if (!sleepy) {
                            creations.playSound("open");
                            windows.shopMenu();
                        }
                    } else if (tasksButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        if (!sleepy) {
                            creations.playSound("open");
                            windows.taskMenu();
                        }
                    } else if (spriteBase.rectangle.getGlobalBounds().contains(mousePos)) {
                        if (sleepy) {
                            creations.playSound("buy");
                            sleepy = false;
                            stats.hunger -= min(hungerBacklog, stats.hunger);
                            hungerBacklog = 0;
                        } else if (!petting.start && stats.record[2] - '0') {
                            creations.playSound("happy");
                            stats.happiness++;
                            petting.start = totals.tick, petting.time = 27;
                        }
                    }
                    lastClick = totals.tick;
                }
            }
        }
    }
    return 0;
}