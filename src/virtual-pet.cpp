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

const Font FONT("RasterForgeRegular-JpBgm.ttf");
const Color DEFAULT_GREEN = Color(34,177,76);
const Color LIGHT_GREEN = Color(18, 219, 18);
const Color DARK_GREEN = Color(5, 51, 6);

struct Stats {
    int hunger;
    int money;
    int hash;
    string mood;
    string record;
};

struct RectangleImage {
    RectangleShape rectangle;
    shared_ptr<Texture> texture;
};

struct listItem {
    RectangleImage image;
    RectangleImage buy;
    RectangleShape bar;
    Text title;
    Text description;
    Text cost;
    int id;
    int pos;

    listItem() 
        : title(FONT, "N/A", 10),
        description(FONT, "N/A", 10),
        cost(FONT, "0", 10)
    {}
};


int hasher(string record) {
    int i = 0;
    for (char c : record) {
        i += c - '0' + 3;
        i *= 3;
        i -= 7;
    }
    return i;
}

void save(Stats stats) {
    ofstream fout("stats.txt");
    fout << stats.hunger << ' '
    << stats.money << ' '
    << stats.mood << ' '
    << stats.record << ' '
    << stats.hash << endl;
}

void statDecay(Stats& stats) {
    int frame = 0;
    while (true) {
        frame++;
        stats.hunger--;

        if (frame % 5 == 0)
            stats.money += 5;

        this_thread::sleep_for(chrono::seconds(1));
    }
}

void barManagment(Stats stats, vector<RectangleShape> &barHelpers) {
    if (stats.hunger <= 0)
        barHelpers[0].setFillColor(DARK_GREEN);
    else if (stats.hunger <= 25)
        barHelpers[1].setFillColor(DARK_GREEN);
    else if (stats.hunger <= 50)
        barHelpers[2].setFillColor(DARK_GREEN);
    else if (stats.hunger <= 75)
        barHelpers[3].setFillColor(DARK_GREEN);

    if (stats.hunger > 75)
        barHelpers[3].setFillColor(DEFAULT_GREEN);
    else if (stats.hunger > 50)
        barHelpers[2].setFillColor(DEFAULT_GREEN);
    else if (stats.hunger > 25)
        barHelpers[1].setFillColor(DEFAULT_GREEN);
    else if (stats.hunger > 0)
        barHelpers[0].setFillColor(DEFAULT_GREEN);
}

void textureDefine(std::shared_ptr<Texture>& texPtr, const string& filePath) {
    texPtr = make_shared<Texture>();
    string fullPath = "sprites/" + filePath + ".png";
    if (!texPtr->loadFromFile(fullPath)) {
        MessageBox(NULL, (filePath + " Load Fail").c_str(), "Texture Error", MB_OK);
        texPtr.reset();
    }
}

RectangleShape createRectangle(float height, float width, float x, float y) {
    RectangleShape rect({height, width});
    rect.setOrigin({height/2, width/2});
    rect.setPosition({x, y});
    return rect;
}

Text createText(int charSize, float x, float y, Color color, String s) {
    static Font FONT("RasterForgeRegular-JpBgm.ttf");
    Text txt(FONT);
    txt.setString(s);
    txt.setCharacterSize(charSize);
    txt.setPosition({x,y});
    txt.setFillColor(color);
    return txt;
}


void textRecenter(Text& text, string command) {
    if (command == "middle")
        text.setOrigin(Vector2f(text.getLocalBounds().position.x + text.getLocalBounds().size.x / 2.f, text.getLocalBounds().position.y + text.getLocalBounds().size.y / 2.f));
    else if (command == "left")
        text.setOrigin(text.getPosition());
}

RectangleImage defineRectangleImage(string filepath, Vector2f size, Vector2f pos) {
    RectangleImage rect;
    rect.rectangle = createRectangle(size.x, size.y, pos.x, pos.y);
    textureDefine(rect.texture, filepath);
    if (rect.texture) rect.rectangle.setTexture(rect.texture.get());
    return move(rect);
}

string enterName(RenderWindow& window) {
    bool typing = true;
    Text nameText = createText(15, 150, 150, Color::Green, "");
    Text namePrompt = createText(25, 50, 50, Color::Green, "What is Your\n Pets Name?");
    while (window.isOpen() && typing) {
        window.clear(Color(0,1,0));
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
                textRecenter(nameText, "middle");
            }
        }
    }
    return nameText.getString();
}

void rollback(vector<listItem>& base, int start) {
    for (auto& item : base) {
        string test = item.title.getString();
        if (item.pos > start) {
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
        } else if (item.pos == start) {
            item.pos = -1;
        }
    }
}

listItem defineListItem(string filepath = "catRich", string title = "N/A", string description = "N/A", int cost = 0, int id = 0) {
    listItem item;
    item.image = defineRectangleImage(filepath, Vector2f(75,75), Vector2f(60, 40 + 60 * (id % 3)));
    item.title = createText(13, 160, 20 + 60 * (id % 3), DEFAULT_GREEN, title);
    textRecenter(item.title, "middle");
    item.description = createText(10, 165, 50 + 60 * (id % 3), DEFAULT_GREEN, description);
    textRecenter(item.description, "middle");
    item.cost = createText(10, 240, 45 + 60 * (id % 3), DEFAULT_GREEN, to_string(cost));
    textRecenter(item.cost, "middle");
    item.buy = defineRectangleImage("buyButton", Vector2f(50,50), Vector2f(240, 40 + 60 * (id%3)));
    item.id = id;
    item.pos = id;
    return item;
}

void shopMenu(Stats* stats, RenderWindow& window) {
    int pageNum = 0;
    RectangleImage background = defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
    RectangleImage close = defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
    RectangleImage arrowForward = defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
    RectangleImage arrowBackward = defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
    Angle flip = degrees(180);
    arrowBackward.rectangle.rotate(flip);
    Text money = createText(10, 15, 15, DEFAULT_GREEN, '$' + to_string(stats->money));
    vector<listItem> shopItems;
    vector<int> itemOrder;
    shopItems.push_back(defineListItem());
    shopItems.push_back(defineListItem("catSick", "SOOO SICK", "CAT IS SICK :<", 1000, 1));
    shopItems.push_back(defineListItem("Hand", "TAKE MY HAND", "ILL THINK ABOUT IT", 0, 2));
    shopItems.push_back(defineListItem("catMad", "GRRRR", "GRRRRR", 5, 3));
    shopItems.push_back(defineListItem("close", "what is this", "kys", 20, 4));
    shopItems.push_back(defineListItem("buyButton", "NO", "this is for buying what", 2, 5));
    for (auto& item : shopItems) {
        if (stats->record[item.id] == '0') {
            itemOrder.push_back(item.id);
        } else {
            rollback(shopItems, item.pos);
        }
    }


    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        money.setString('$' + to_string(stats->money));
        window.draw(background.rectangle);
        window.draw(money);
        window.draw(close.rectangle);
        for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < itemOrder.size(); i++) {
            int ind = itemOrder[i];
            //MessageBox(NULL, to_string(ind).c_str(), "Debug", MB_OK);
            window.draw(shopItems[ind].image.rectangle);
            window.draw(shopItems[ind].buy.rectangle);
            window.draw(shopItems[ind].cost);
            window.draw(shopItems[ind].description);
            window.draw(shopItems[ind].title);
        }
        //USE SECONDARY ARRAY OF ORDER TO REFERENCE WITH EACH THING BEING AN ID ALLOWING FOR BETTER ARRANGEMENT
        if ((int)pageNum + 1 <= itemOrder.size()/3 && itemOrder.size() != 3)
            window.draw(arrowForward.rectangle);
        if ((int)pageNum > 0)
            window.draw(arrowBackward.rectangle);

        window.display();

        while (const optional event = window.pollEvent()) {
             if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Right)
                    MessageBox(NULL, to_string(pageNum).c_str(), "Debug", MB_OK);
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < itemOrder.size(); i++) {
                        int ind = itemOrder[i];
                        if (shopItems[ind].buy.rectangle.getGlobalBounds().contains(mousePos)) {
                            if (stats->money >= stoi((string)shopItems[ind].cost.getString())) {
                                stats->record[ind] = '1';
                                stats->money -= stoi((string)shopItems[ind].cost.getString());
                                itemOrder.erase(itemOrder.begin() + i);
                                rollback(shopItems, shopItems[ind].pos);
                            }
                        }
                    }
                    if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                        pageNum--;
                    else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= itemOrder.size()/3 && itemOrder.size() != 3)
                        pageNum++;
                    else if (close.rectangle.getGlobalBounds().contains(mousePos))
                        return;
                }

            }
        }
    }
}


int main() {
    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    fin >> stats.hunger >> stats.mood >> stats.money >> stats.record >> stats.hash;
    if (stats.hash != hasher(stats.record))
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

    RectangleImage spriteBase = defineRectangleImage("catNormal", Vector2f(250, 250), Vector2f(150, 125));
    RectangleImage cornerMove = defineRectangleImage("cornerMove", Vector2f(50, 50), Vector2f(240, 40));
    RectangleImage shopButton = defineRectangleImage("shopButton", Vector2f(100, 100), Vector2f(45, 85));
    RectangleImage tasksButton = defineRectangleImage("tasksButton", Vector2f(100, 100), Vector2f(45, 155));
    RectangleImage barBase = defineRectangleImage("hungerBar", Vector2f(115, 115), Vector2f(250, 120));

    
    vector<RectangleShape> barHelpers(4, RectangleShape({30.f, 20.f}));
    for (int i = 0; i < 4; i++) {
        barHelpers[i] = createRectangle(30, 20, 250, 150 + (i * -20));
        barHelpers[i].setFillColor(stats.hunger > i * 25 ? DEFAULT_GREEN : DARK_GREEN);
    }

    Text hungerText = createText(20, 0, 0, LIGHT_GREEN, to_string(stats.hunger));
    Text moneyText = createText(20, 0, 30, LIGHT_GREEN, to_string(stats.money));
    Text nameText = createText(30, 150, 10, DEFAULT_GREEN, enterName(window));
    textRecenter(nameText, "middle");

    thread hungerDecay(statDecay, ref(stats));

    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        window.draw(spriteBase.rectangle);
        for (RectangleShape rect : barHelpers) {
            window.draw(rect);
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
                window.close();
                save(stats);
            }
            
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (shopButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        shopMenu(&stats, window);
                    } else if (spriteBase.rectangle.getGlobalBounds().contains(mousePos)) {
                        stats.hunger++;
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {           
                if (Keyboard::isKeyPressed(Keyboard::Key::A))
                    stats.hunger -= 5;
            }
        }

        barManagment(stats, barHelpers);
    }
}