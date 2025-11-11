#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "utilityManager.hpp"
#include "creationManager.hpp"
#include "globals.hpp"
using namespace sf;
using namespace std;

void creationManager::defineTexture(std::shared_ptr<Texture>& texPtr, const string& filePath) {
    if (!texPtr)
        texPtr = make_shared<Texture>();

    string fullPath = "sprites/" + filePath + ".png";
    if (!texPtr->loadFromFile(fullPath)) {
        MessageBoxA(NULL, (filePath + " Load Fail").c_str(), "Texture Error", MB_OK);
        texPtr.reset();
    }
}

RectangleShape creationManager::defineRectangle(float height, float width, float x, float y) {
    RectangleShape rect({height, width});
    rect.setOrigin({height/2, width/2});
    rect.setPosition({x, y});
    return rect;
}


listItem creationManager::defineListItem(string filepath = "catRich", string title = "N/A", string description = "N/A", int cost = 0, int id = 0) {
    listItem item;
    item.image = defineRectangleImage(filepath, Vector2f(60,60), Vector2f(60, 40 + 60 * (id % 3)));
    item.title = defineText(13, 160, 20 + 60 * (id % 3), DEFAULT_GREEN, title);
    utilitiesManager::textRecenter(item.title, "middle");
    item.description = defineText(10, 165, 50 + 60 * (id % 3), DEFAULT_GREEN, description);
    utilitiesManager::textRecenter(item.description, "middle");
    if (cost == -1) {
        item.buy = defineRectangleImage("startButton", Vector2f(40,40), Vector2f(250, 35 + 60 * (id % 3)));
    } else {
        item.cost = defineText(10, 250, 40 + 60 * (id % 3), DEFAULT_GREEN, to_string(cost));
        utilitiesManager::textRecenter(item.cost, "middle");
        item.buy = defineRectangleImage("buyButton", Vector2f(40,40), Vector2f(250, 35 + 60 * (id%3)));
    }
    item.id = id;
    item.pos = id;
    return item;
}

Text creationManager::defineText(int charSize, float x, float y, Color color, String s) {
    static Font FONT("RasterForgeRegular-JpBgm.ttf");
    Text txt(FONT);
    txt.setString(s);
    txt.setCharacterSize(charSize);
    txt.setPosition({x,y});
    txt.setFillColor(color);
    return txt;
}

void creationManager::mainSpriteControl(shared_ptr<Texture>& texture) {
    if (stats.mood == "mad")
        defineTexture(texture, "catMad");
    else if (stats.mood == "sick")
        defineTexture(texture, "catSick");
    else if (stats.mood == "sad")
        defineTexture(texture, "catSad");
    else if (stats.mood == "normal")
        if (stats.record[5] - '0') 
            defineTexture(texture, "catRich");
        else
            defineTexture(texture, "catNormal");
}

RectangleImage creationManager::defineRectangleImage(string filepath, Vector2f size, Vector2f pos) {
    RectangleImage rect;
    rect.rectangle = defineRectangle(size.x, size.y, pos.x, pos.y);
    defineTexture(rect.texture, filepath);
    if (rect.texture) rect.rectangle.setTexture(rect.texture.get());
    return move(rect);
}