#pragma once
#include "dataTypes.hpp"
#include <SFML/Audio.hpp>

class creationManager {
public:
    static void defineTexture(shared_ptr<Texture>& texPtr, const string& filePath);
    static void playSound(string filepath);
    static void stopSound();
    static RectangleShape defineRectangle(float height, float width, float x, float y);
    static listItem defineListItem(string filepath, string title, string description, int cost, int id);
    static Text defineText(int charSize, float x, float y, Color color, String s);
    static RectangleImage defineRectangleImage(string filepath, Vector2f size, Vector2f pos);
};