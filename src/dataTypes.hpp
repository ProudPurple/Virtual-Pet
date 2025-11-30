#pragma once
#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace sf;
using namespace std;

struct Totals {
    int moneySpent;
    int moneyGained;
    int foodEaten;
    int timesSick;
    int tick;
    long long time;

    Totals();
};

extern Totals totals;

struct Stats {
    int hunger;
    int money;
    int happiness;
    int hash;
    string name;
    string type;
    string mood;
    string record;

    Stats();
};

extern Stats stats;

struct RectangleImage {
    RectangleShape rectangle;
    shared_ptr<Texture> texture;
};

extern RectangleImage image;

struct listItem {
    RectangleImage image;
    RectangleImage buy;
    RectangleShape bar;
    Text title;
    Text description;
    Text cost;
    int id;
    int pos;

    listItem();
};

extern listItem item;

struct Animation {
    int start;
    int time;

    Animation();
};

extern Animation animation;