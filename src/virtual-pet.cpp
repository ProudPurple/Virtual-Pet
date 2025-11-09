#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>
#include <thread>
#include <algorithm>
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

thread backgroundThread;
atomic<bool> running = true;
int tick;

struct Stats {
    int hunger;
    int money;
    int happiness;
    int hash;
    string name;
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

class utilitiesManager {
public:
    static int hasher(string record) {
        int i = 0;
        for (char c : record) {
            i += c - '0' + 3;
            i *= 2;
            i -= 7;
        }
        return i;
    }

    static void save(Stats stats) {
        ofstream fout("stats.txt");
        fout << stats.hunger << ' '
        << stats.money << ' '
        << stats.happiness << ' '
        << stats.record << ' '
        << hasher(stats.record) << ' '
        << stats.name << ' '
        << stats.mood << endl;
    }

    static void close(RenderWindow& window, Stats stats) {
        window.close();
        running = false;
        backgroundThread.join();
        save(stats);
    }

    static void background(Stats& stats) {
        while (true && running) {
            tick++;
            
            if (tick % 20 == 0) {
                stats.hunger--;
            }
            if (tick % 50 == 0) {
                stats.money += 5;
            }
            moodManager(stats);
            if (tick >= 100000)
                tick = 1;
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }

    static void barManagment(Stats stats, vector<RectangleShape> &barHelpers) {
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

    static void moodManager(Stats& stats) {
        if (stats.happiness <= 40 && tick % 100 == 0 && rand() % 4) {
            stats.mood = "sick";
        } else if (stats.hunger <= 0) {
            stats.mood = "mad";
        } else if (stats.happiness <= 0) {
            stats.mood = "sad";
        } else {
            stats.mood = "normal";
        }
    }

    static void rollforward(listItem& item) {
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

    static void rollbackward(listItem& item) {
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

    static void roll(vector<listItem>& base, int start, string command) {
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
    
    static void textRecenter(Text& text, string command) {
        if (command == "middle")
            text.setOrigin(Vector2f(text.getLocalBounds().position.x + text.getLocalBounds().size.x / 2.f, text.getLocalBounds().position.y + text.getLocalBounds().size.y / 2.f));
        else if (command == "left")
            text.setOrigin(text.getPosition());
    }
};

class creationManager {
public:
    static void defineTexture(std::shared_ptr<Texture>& texPtr, const string& filePath) {
        if (!texPtr)
            texPtr = make_shared<Texture>();

        string fullPath = "sprites/" + filePath + ".png";
        if (!texPtr->loadFromFile(fullPath)) {
            MessageBox(NULL, (filePath + " Load Fail").c_str(), "Texture Error", MB_OK);
            texPtr.reset();
        }
    }

    static RectangleShape defineRectangle(float height, float width, float x, float y) {
        RectangleShape rect({height, width});
        rect.setOrigin({height/2, width/2});
        rect.setPosition({x, y});
        return rect;
    }

    
    static listItem defineListItem(string filepath = "catRich", string title = "N/A", string description = "N/A", int cost = 0, int id = 0) {
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

    static Text defineText(int charSize, float x, float y, Color color, String s) {
        static Font FONT("RasterForgeRegular-JpBgm.ttf");
        Text txt(FONT);
        txt.setString(s);
        txt.setCharacterSize(charSize);
        txt.setPosition({x,y});
        txt.setFillColor(color);
        return txt;
    }

    static void mainSpriteControl(Stats stats, shared_ptr<Texture>& texture) {
        if (stats.mood == "mad")
            defineTexture(texture, "catMad");
        else if (stats.mood == "sick")
            defineTexture(texture, "catSick");
        else if (stats.mood == "sad")
            defineTexture(texture, "catSad");
        else if (stats.mood == "normal")
            if (stats.record[1]) 
                defineTexture(texture, "catRich");
            else
                defineTexture(texture, "catNormal");
    }

    static RectangleImage defineRectangleImage(string filepath, Vector2f size, Vector2f pos) {
        RectangleImage rect;
        rect.rectangle = defineRectangle(size.x, size.y, pos.x, pos.y);
        defineTexture(rect.texture, filepath);
        if (rect.texture) rect.rectangle.setTexture(rect.texture.get());
        return move(rect);
    }
};

class windowManager {
public:
    static string enterName(RenderWindow& window, Stats stats) {
        bool typing = true;
        Text nameText = creationManager::defineText(15, 150, 150, Color::Green, "");
        Text namePrompt = creationManager::defineText(25, 50, 50, Color::Green, "What is Your\n Pets Name?");
        while (window.isOpen() && typing) {
            window.clear(Color(0,1,0));
            window.draw(nameText);
            window.draw(namePrompt);
            window.display();

            while (const optional event = window.pollEvent()) {
                if (event->is<Event::Closed>())
                    return "CLOSE PLEASE";
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
                    utilitiesManager::textRecenter(nameText, "middle");
                }
            }
        }
        return nameText.getString();
    }


    static void shopMenu(Stats* stats, RenderWindow& window) {
        int pageNum = 0;
        RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
        RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
        RectangleImage arrowForward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
        RectangleImage arrowBackward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
        Angle flip = degrees(180);
        arrowBackward.rectangle.rotate(flip);
        Text money = creationManager::defineText(10, 15, 15, DEFAULT_GREEN, '$' + to_string(stats->money));
        vector<listItem> shopItems;
        vector<int> itemOrder;
        shopItems.push_back(creationManager::defineListItem("foodBag", "Food", "FEED", 5, 0));
        shopItems.push_back(creationManager::defineListItem("catRich", "MONEY", "now this is some\nreal cash", 1000, 1));
        shopItems.push_back(creationManager::defineListItem("Hand", "TAKE MY HAND", "ILL THINK ABOUT IT", 0, 2));
        shopItems.push_back(creationManager::defineListItem("catMad", "GRRRR", "GRRRRR", 5, 3));
        shopItems.push_back(creationManager::defineListItem("close", "what is this", "kys", 20, 4));
        shopItems.push_back(creationManager::defineListItem("buyButton", "NO", "this is for buying what", 2, 5));
        for (auto& item : shopItems) {
            if (stats->record[item.id] == '0') {
                itemOrder.push_back(item.id);
            } else {
                utilitiesManager::roll(shopItems, item.pos, "backwards");
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
                if (event->is<Event::Closed>()) {
                    utilitiesManager::close(window, *stats);
                }
                if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                    if (mousePressed->button == Mouse::Button::Left) {
                        Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                        for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < itemOrder.size(); i++) {
                            int ind = itemOrder[i];
                            if (shopItems[ind].buy.rectangle.getGlobalBounds().contains(mousePos)) {
                                if (stats->money >= stoi((string)shopItems[ind].cost.getString())) {
                                    stats->record[ind] = '1';
                                    stats->money -= stoi((string)shopItems[ind].cost.getString());
                                    itemOrder.erase(itemOrder.begin() + i);
                                    utilitiesManager::roll(shopItems, shopItems[ind].pos, "backwards");
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

    static void taskMenu(Stats& stats, RenderWindow& window) {
        RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
        RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
        RectangleImage arrowForward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
        RectangleImage arrowBackward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
        Angle flip = degrees(180);
        arrowBackward.rectangle.rotate(flip);

        int pageNum = 0;
        vector<listItem> taskList;
        taskList.push_back(creationManager::defineListItem("foodBag", "FEED", "feed him", -1, 0));
        taskList.push_back(creationManager::defineListItem("catSad", "Cheer Up", "Make Happy", -1, 1));
        taskList.push_back(creationManager::defineListItem("hand", "what?", "where do you\nkeep finding this", -1, 2));
        taskList.push_back(creationManager::defineListItem("catMad", "UH OH", "MAD", -1, 3));
        taskList.push_back(creationManager::defineListItem("catRich", "MOOLAH", "Make That\nMONEY", -1, 4));
        vector<int> taskOrder;
        
        if (stats.record[0] - '0')
            taskOrder.push_back(0);
        if (stats.mood == "sad")
            taskOrder.push_back(1);
        if (stats.record[1] - '0')
            taskOrder.push_back(2);
        if (stats.record[2] - '0')
            taskOrder.push_back(3);
        if (stats.record[3] - '0')
            taskOrder.push_back(4);
        
        while (window.isOpen()) {
            if (tick % 10 == 0) {
                if (stats.record[0] - '0' && find(taskOrder.begin(), taskOrder.end(), 0) == taskOrder.end())
                    taskOrder.push_back(0);
                if (stats.mood == "sad" && find(taskOrder.begin(), taskOrder.end(), 1) == taskOrder.end())
                    taskOrder.push_back(1);
                if (stats.record[1] - '0' && find(taskOrder.begin(), taskOrder.end(), 2) == taskOrder.end())
                    taskOrder.push_back(2);
                if (stats.record[2] - '0' && find(taskOrder.begin(), taskOrder.end(), 3) == taskOrder.end())
                    taskOrder.push_back(3);
                if (stats.record[3] - '0' && find(taskOrder.begin(), taskOrder.end(), 4) == taskOrder.end())
                    taskOrder.push_back(4);
            }
            window.clear(Color(0,1,0));
            window.draw(background.rectangle);
            window.draw(close.rectangle);
           if ((int)pageNum + 1 <= taskOrder.size()/3 && taskOrder.size() > 3)
                window.draw(arrowForward.rectangle);
            if ((int)pageNum > 0)
                window.draw(arrowBackward.rectangle);
         
            for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < taskOrder.size(); i++) {
                listItem& item = taskList[taskOrder[i]];
                while (item.pos < i) {
                    utilitiesManager::rollforward(item);
                }
                while (item.pos > i) {
                    utilitiesManager::rollbackward(item);
                }
                window.draw(item.image.rectangle);
                window.draw(item.buy.rectangle);
                window.draw(item.description);
                window.draw(item.title);
            }

            window.display();

            while (const optional event = window.pollEvent()) {
                if (event->is<Event::Closed>()) {
                    utilitiesManager::close(window, stats);
                }
                if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                    if (mousePressed->button == Mouse::Button::Left) {
                        Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                        for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < taskOrder.size(); i++) {
                            listItem& item = taskList[taskOrder[i]];
                            if (item.buy.rectangle.getGlobalBounds().contains(mousePos)) {
                                if (item.id == 0) {
                                    stats.hunger += 70;
                                    stats.record[0] = '0';
                                    taskOrder.erase(taskOrder.begin() + i);
                                } else if (item.id == 1) {
                                    stats.happiness += 35;
                                    taskOrder.erase(taskOrder.begin() + i);
                                }
                            }
                        }
                        if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                            pageNum--;
                        else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= taskOrder.size()/3 && taskOrder.size() > 3)
                            pageNum++;
                        else if (close.rectangle.getGlobalBounds().contains(mousePos))
                            return;
                    }

                }
            }
        }
    }
};



int main() {
    creationManager creations;
    utilitiesManager utilities;

    //Takes in saved stats from txt file
    ifstream fin("stats.txt");
    Stats stats;
    fin >> stats.hunger >> stats.money >> stats.happiness >> stats.record >> stats.hash;
    stats.hash = utilities.hasher(stats.record);
    if (stats.hash != utilitiesManager::hasher(stats.record))
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
    RectangleImage shopButton = creations.defineRectangleImage("shopButton", Vector2f(100, 100), Vector2f(45, 85));
    RectangleImage tasksButton = creations.defineRectangleImage("tasksButton", Vector2f(100, 100), Vector2f(45, 155));
    RectangleImage barBase = creations.defineRectangleImage("hungerBar", Vector2f(115, 115), Vector2f(250, 120));

    
    vector<RectangleShape> barHelpers(4, RectangleShape({30.f, 20.f}));
    for (int i = 0; i < 4; i++) {
        barHelpers[i] = creations.defineRectangle(30, 20, 250, 150 + (i * -20));
        barHelpers[i].setFillColor(stats.hunger > i * 25 ? DEFAULT_GREEN : DARK_GREEN);
    }

    Text hungerText = creations.defineText(20, 0, 0, LIGHT_GREEN, to_string(stats.hunger));
    Text moneyText = creations.defineText(20, 0, 30, LIGHT_GREEN, to_string(stats.money));

    if (stats.record[stats.record.size() - 1] - '0')
        fin >> stats.name;
    else
        stats.name = windowManager::enterName(window, stats);
    if (stats.name == "CLOSE PLEASE")
        return 0;
    stats.record[stats.record.size() - 1] = '1';

    Text nameText = creations.defineText(30, 150, 10, DEFAULT_GREEN, stats.name);
    utilities.textRecenter(nameText, "middle");

    fin >> stats.mood;
    backgroundThread = thread(utilities.background, ref(stats));

    while (window.isOpen()) {
        window.clear(Color(0,1,0));
 
        if (tick % 20)
            creations.mainSpriteControl(stats, spriteBase.texture);
                    //MessageBox(NULL, stats.mood.c_str(), "Debug", MB_OK);
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
                utilitiesManager::close(window, stats);
            }
            
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (shopButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        windowManager::shopMenu(&stats, window);
                    } else if (tasksButton.rectangle.getGlobalBounds().contains(mousePos)) {
                        windowManager::taskMenu(stats, window);
                    } else if (spriteBase.rectangle.getGlobalBounds().contains(mousePos)) {
                        stats.happiness++;
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {           
                if (Keyboard::isKeyPressed(Keyboard::Key::A))
                    stats.hunger -= 5;
            }
        }
        utilitiesManager::barManagment(stats, barHelpers);
    }
    return 0;
}