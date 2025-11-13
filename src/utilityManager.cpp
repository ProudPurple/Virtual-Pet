#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <Windows.h>
#include <SFML/Window.hpp>
#include "globals.hpp"
#include "dataTypes.hpp"
#include "utilityManager.hpp"

int utilitiesManager::hasher(string record) {
    int i = 0;
    for (char c : record) {
        i += c - '0' + 3;
        i *= 2;
        i -= 7;
    }
    return i;
}

void utilitiesManager::takeInStats() {
    ifstream fin("stats.txt");
    fin >> stats.hunger >> stats.money >> stats.happiness >> stats.record >> stats.hash;
    fin >> stats.name >> stats.mood;
    fin >> totals.foodEaten >> totals.moneyGained >> totals.moneySpent >> totals.tick >> totals.time >> totals.timesSick;
}

void utilitiesManager::save() {
    ofstream fout("stats.txt");
    fout << stats.hunger << ' '
    << stats.money << ' '
    << stats.happiness << ' '
    << stats.record << ' '
    << hasher(stats.record) << ' '
    << stats.name << ' '
    << stats.mood << endl;

    fout << totals.foodEaten << ' '
    << totals.moneyGained << ' ' 
    << totals.moneySpent << ' '
    << totals.tick << ' '
    << totals.time << ' '
    << totals.timesSick << endl;
}

void utilitiesManager::close(RenderWindow& window) {
    window.close();
    running = false;
    backgroundThread.join();
    save();
}

void utilitiesManager::background() {
    while (true && running) {
        totals.tick++;
        
        if (totals.tick % (stats.mood == "sick" ? 3 : 10) == 0 && stats.hunger >= 0) {
            stats.hunger--;
        }
        if (totals.tick % 50 == 0) {
            stats.money += 5;
            totals.moneyGained += 5;
        }
        if (totals.tick % 5 == 0)
            totals.time++;
        if (totals.tick % 900 == 0)
            save();
        utilitiesManager::moodManager();
        if (totals.tick >= 100000)
            totals.tick = 1;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void utilitiesManager::barManager  (vector<RectangleShape> &barHelpers) {
    if (stats.hunger <= 0)
        barHelpers[0].setFillColor(DARK_GREEN);
    if (stats.hunger <= 25)
        barHelpers[1].setFillColor(DARK_GREEN);
    if (stats.hunger <= 50)
        barHelpers[2].setFillColor(DARK_GREEN);
    if (stats.hunger <= 75)
        barHelpers[3].setFillColor(DARK_GREEN);

    if (stats.hunger > 75)
        barHelpers[3].setFillColor(DEFAULT_GREEN);
    if (stats.hunger > 50)
        barHelpers[2].setFillColor(DEFAULT_GREEN);
    if (stats.hunger > 25)
        barHelpers[1].setFillColor(DEFAULT_GREEN);
    if (stats.hunger > 0)
        barHelpers[0].setFillColor(DEFAULT_GREEN);
}

void utilitiesManager::moodManager() {
    if (stats.happiness <= 70 && totals.tick % 120 == 0 && rand() % 3 == 0 || stats.mood == "sick") {
        if (stats.mood != "sick")
            totals.timesSick++;
        stats.mood = "sick";
    } else if (stats.hunger <= 0) {
        stats.mood = "mad";
    } else if (stats.happiness <= 0) {
        stats.mood = "sad";
    } else {
        stats.mood = "normal";
    }
}

void utilitiesManager::rollforward(listItem& item) {
    if (item.pos % 3 == 2) {
        item.image.rectangle.setPosition(item.image.rectangle.getPosition() - Vector2f(0,60));
        item.buy.rectangle.setPosition(item.buy.rectangle.getPosition() - Vector2f(0, 60));
        item.cost.setPosition(item.cost.getPosition() - Vector2f(0, 60));
        item.description.setPosition(item.description.getPosition() - Vector2f(0, 60));
        item.title.setPosition(item.title.getPosition() - Vector2f(0, 60));
    } else {
        item.image.rectangle.setPosition(item.image.rectangle.getPosition() + Vector2f(0,60));
        item.buy.rectangle.setPosition(item.buy.rectangle.getPosition() + Vector2f(0, 60));
        item.cost.setPosition(item.cost.getPosition() + Vector2f(0, 60));
        item.description.setPosition(item.description.getPosition() + Vector2f(0, 60));
        item.title.setPosition(item.title.getPosition() + Vector2f(0, 60));
    }
    item.pos++;
}

void utilitiesManager::rollbackward(listItem& item) {
    if (item.pos % 3 == 0) {
        item.image.rectangle.setPosition(item.image.rectangle.getPosition() + Vector2f(0,120));
        item.buy.rectangle.setPosition(item.buy.rectangle.getPosition() + Vector2f(0, 120));
        item.cost.setPosition(item.cost.getPosition() + Vector2f(0, 120));
        item.description.setPosition(item.description.getPosition() + Vector2f(0, 120));
        item.title.setPosition(item.title.getPosition() + Vector2f(0, 120));
    } else {
        item.image.rectangle.setPosition(item.image.rectangle.getPosition() - Vector2f(0,60));
        item.buy.rectangle.setPosition(item.buy.rectangle.getPosition() - Vector2f(0, 60));
        item.cost.setPosition(item.cost.getPosition() - Vector2f(0, 60));
        item.description.setPosition(item.description.getPosition() - Vector2f(0, 60));
        item.title.setPosition(item.title.getPosition() - Vector2f(0, 60));
    }
    item.pos--;
}

void utilitiesManager::roll(vector<listItem>& base, int start, string command) {
    for (auto& item : base) {
        if (command == "backwards") {
            if (item.pos > start) {
                rollbackward(item);
            } else if (item.pos == start) {
                item.pos = -1;
            }
        } else if (command == "forwards") {
            if (item.pos > start) {
                rollforward(item);
            }
        }
    }
}

void utilitiesManager::textRecenter(Text& text, string command) {
    if (command == "middle")
        text.setOrigin(Vector2f(text.getLocalBounds().position.x + text.getLocalBounds().size.x / 2.f, text.getLocalBounds().position.y + text.getLocalBounds().size.y / 2.f));
    else if (command == "left")
        text.setOrigin(text.getPosition());
}