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
        
        if (totals.tick % (stats.mood == "sick" ? 3 : 10) == 0 && stats.hunger >= 0 && !sleepy) {
            stats.hunger--;
        } else if (stats.hunger > 100)
            stats.hunger = 100;
            
        if (totals.tick % 50 == 0) {
            stats.money += 5;
            totals.moneyGained += 5;
        }
        if (totals.tick % 5 == 0)
            totals.time++;
        if (totals.tick % 500 == 0)
            save();
        utilitiesManager::moodManager();
        if (totals.tick >= 100000)
            totals.tick = 1;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void utilitiesManager::moodManager() {
    if (stats.happiness <= 70 && totals.tick % 120 == 0 && rand() % 3 == 0 || stats.mood == "sick") {
        if (stats.mood != "sick")
            totals.timesSick++;
        stats.mood = "sick";
    } else if (stats.mood == "dirty") {
        stats.mood = "dirty";
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

void utilitiesManager::taskListCheck(vector<listItem>& taskItems, vector<int>& taskOrder) {
    if (stats.record[0] - '0' && find(taskOrder.begin(), taskOrder.end(), 0) == taskOrder.end()) {
        taskOrder.push_back(0);
    }
    if (stats.mood == "sad" && find(taskOrder.begin(), taskOrder.end(), 1) == taskOrder.end()) {
        taskOrder.push_back(1);
    }
    if (stats.record[5] - '0' && stats.mood == "dirty" && find(taskOrder.begin(), taskOrder.end(), 2) == taskOrder.end()) {
        taskOrder.push_back(2);
    }
    if (stats.mood == "sick" && stats.record[1] - '0' && find(taskOrder.begin(), taskOrder.end(), 3) == taskOrder.end()) {
        taskOrder.push_back(3);
    }
    if (trashDelay - totals.time < 0 && find(taskOrder.begin(), taskOrder.end(), 4) == taskOrder.end()) {
        taskOrder.push_back(4);
    }
    if (stats.record[4] - '0' && dishDelay - totals.time < 0 && find(taskOrder.begin(), taskOrder.end(), 5) == taskOrder.end()) {
        taskOrder.push_back(5);
    }
    if (stats.record[3] - '0' && find(taskOrder.begin(), taskOrder.end(), 6) == taskOrder.end() && frisbeeDelay - totals.time <= 0) {
        taskOrder.push_back(6);
    }
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