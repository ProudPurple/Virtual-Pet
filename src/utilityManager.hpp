#pragma once
#include "dataTypes.hpp"

class utilitiesManager {
public:
    static int hasher(string record);
    static void takeInStats();
    static void save();
    static void close(RenderWindow& window);
    static void background();
    static void barManager  (vector<RectangleShape> &barHelpers);
    static void moodManager();
    static void rollforward(listItem& item);
    static void rollbackward(listItem& item);
    static void roll(vector<listItem>& base, int start, string command);
    static void textRecenter(Text& text, string command);
};