#pragma once
#include "dataTypes.hpp"

class creationManager {
public:
    static void defineTexture(shared_ptr<Texture>& texPtr, const string& filePath);
    static RectangleShape defineRectangle(float height, float width, float x, float y);
    static listItem defineListItem(string filepath, string title, string description, int cost, int id);
    static Text defineText(int charSize, float x, float y, Color color, String s);
    static RectangleImage defineRectangleImage(string filepath, Vector2f size, Vector2f pos);
};