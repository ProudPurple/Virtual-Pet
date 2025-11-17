#include <Windows.h>
#include "creationManager.hpp"
#include "utilityManager.hpp"
#include "windowManager.hpp"
#include "globals.hpp"

string windowManager::enterName(RenderWindow& window) {
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


void windowManager::shopMenu(RenderWindow& window) {
    int pageNum = 0;
    RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
    RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
    RectangleImage arrowForward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
    RectangleImage arrowBackward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
    Angle flip = degrees(180);
    arrowBackward.rectangle.rotate(flip);
    Text money = creationManager::defineText(10, 15, 15, DEFAULT_GREEN, '$' + to_string(stats.money));
    vector<listItem> shopItems;
    vector<int> itemOrder;
    shopItems.push_back(creationManager::defineListItem("foodBag", "Food", "FEED", 5, 0));
    shopItems.push_back(creationManager::defineListItem("medKit", "Healing", "Lets you help\na sick pet", 15, 1));
    shopItems.push_back(creationManager::defineListItem("Hand", "TAKE MY HAND", "ILL THINK ABOUT IT", 0, 2));
    shopItems.push_back(creationManager::defineListItem("frisbee", "Frisbee", "Play with your pet", 5, 3));
    shopItems.push_back(creationManager::defineListItem("soapBottle", "Cleaning Supplies", "Clean Broto", 10, 4));
    shopItems.push_back(creationManager::defineListItem("catRich", "MONEY", "SO MONEY", 100, 5));
    for (auto& item : shopItems) {
        if (stats.record[item.id] == '0') {
            itemOrder.push_back(item.id);
        } else {
            utilitiesManager::roll(shopItems, item.pos, "backwards");
        }
    }


    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        money.setString('$' + to_string(stats.money));
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
        if ((int)pageNum + 1 <= itemOrder.size()/3 && itemOrder.size()/3 != pageNum + 1)
            window.draw(arrowForward.rectangle);
        if ((int)pageNum > 0)
            window.draw(arrowBackward.rectangle);

        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < itemOrder.size(); i++) {
                        int ind = itemOrder[i];
                        if (shopItems[ind].buy.rectangle.getGlobalBounds().contains(mousePos)) {
                            if (stats.money >= stoi((string)shopItems[ind].cost.getString())) {
                                stats.record[ind] = '1';
                                stats.money -= stoi((string)shopItems[ind].cost.getString());
                                totals.moneySpent += stoi((string)shopItems[ind].cost.getString());
                                itemOrder.erase(itemOrder.begin() + i);
                                utilitiesManager::roll(shopItems, shopItems[ind].pos, "backwards");
                            }
                        }
                    }
                    if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                        pageNum--;
                    else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= itemOrder.size()/3 && itemOrder.size()/3 != pageNum + 1)
                        pageNum++;
                    else if (close.rectangle.getGlobalBounds().contains(mousePos))
                        return;
                }

            }
        }
    }
}

void windowManager::statDisplay(RenderWindow& window) {
    RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
    RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
    Text title = creationManager::defineText(15, 150, 20, DEFAULT_GREEN, "All Time Stats");
    utilitiesManager::textRecenter(title, "middle");
    vector<Text> statList;
    statList.push_back(creationManager::defineText(10, 150, 40, DEFAULT_GREEN, "Money Spent: " + to_string(totals.moneySpent)));
    statList.push_back(creationManager::defineText(10, 150, 55, DEFAULT_GREEN, "Money Gained: " + to_string(totals.moneyGained)));
    statList.push_back(creationManager::defineText(10, 150, 70, DEFAULT_GREEN, "Food Eaten: " + to_string(totals.foodEaten)));
    statList.push_back(creationManager::defineText(10, 150, 85, DEFAULT_GREEN, "Seconds Spent: " + to_string(totals.time)));
    statList.push_back(creationManager::defineText(10, 150, 100, DEFAULT_GREEN, "Times Sick: " + to_string(totals.timesSick)));

    for (auto& txt : statList) 
        utilitiesManager::textRecenter(txt, "middle");

    while (window.isOpen()) {
        window.clear(Color(0,1,0));

        statList[1].setString("Money Gained: " + to_string(totals.moneyGained));
        statList[3].setString("Seconds Spent: " + to_string(totals.time));

        window.draw(background.rectangle);
        window.draw(close.rectangle);
        window.draw(title);
        for (auto txt : statList)
            window.draw(txt);
        window.display();

            while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (close.rectangle.getGlobalBounds().contains(mousePos)) {
                        return;
                    }
                }
            }
        }
    }
    return;
}

int windowManager::foodMini(RenderWindow& window) {
    RectangleImage foodBag = creationManager::defineRectangleImage("foodBag", Vector2f(100, 100), Vector2f(150,50));
    RectangleImage foodBowl = creationManager::defineRectangleImage("foodBowl", Vector2f(100,50), Vector2f(150,200));
    Text timeText = creationManager::defineText(20, 150, 130, DEFAULT_GREEN, "3");
    Text instructions = creationManager::defineText(15, 160, 100, DEFAULT_GREEN, "The bowl will move to your\nmouse catch the food with it!");
    utilitiesManager::textRecenter(instructions, "middle");
    vector<RectangleImage> foodFall;
    foodBag.rectangle.rotate(degrees(180));
    int foodTotal = 0, speed = 2, curTick = totals.tick, offset = 4, start = totals.time;
    bool movement = false;
    while (window.isOpen() && totals.time <= start + 14) {
        if (totals.time <= start + 3) {
            window.clear(Color(0,1,0));
            timeText.setString(to_string(start + 4 - totals.time));
            window.draw(timeText);
            window.draw(instructions);
            window.draw(foodBag.rectangle);
            window.draw(foodBowl.rectangle);
            window.display();
        } else {
            timeText.setPosition(Vector2f(20,20));
            if (totals.tick % offset == 0 && curTick != totals.tick) {
                offset += rand() % 2 * movement ? -1 : 1;
                if (offset > 5) offset = 5;
                if (offset < 1) offset = 1;
                curTick = totals.tick;
                foodFall.push_back(creationManager::defineRectangleImage("foodBag", Vector2f(20,20), foodBag.rectangle.getPosition()));
                movement = !movement;
            }
            window.clear(Color(0,1,0));
            Vector2i mousePos = Mouse::getPosition(window);
            Vector2f foodBowlPos = foodBowl.rectangle.getPosition();
            for (int i = 0; i < foodFall.size(); i++) {
                foodFall[i].rectangle.setPosition(foodFall[i].rectangle.getPosition() + Vector2f(0, speed + 1));
                Vector2f curPosition = foodFall[i].rectangle.getPosition();
                if (curPosition.y >= 170 && curPosition.y <= 190 && curPosition.x >= foodBowlPos.x - 50 && curPosition.x <= foodBowlPos.x + 50) {
                    foodTotal += 5;
                    foodFall.erase(foodFall.begin() + i);
                    i--;
                } else
                    window.draw(foodFall[i].rectangle);
            }
            if (movement) {
                foodBag.rectangle.setPosition(foodBag.rectangle.getPosition() + Vector2f(foodBag.rectangle.getPosition().x <= 250 ? speed + 1 : 0, 0));
            } else {
                foodBag.rectangle.setPosition(foodBag.rectangle.getPosition() - Vector2f(foodBag.rectangle.getPosition().x >= 50 ? speed + 1 : 0, 0));
            }
            if (mousePos.x > foodBowlPos.x && foodBowlPos.x < 250) {
                foodBowl.rectangle.setPosition(Vector2f(foodBowlPos.x + speed, foodBowlPos.y));
            } else if (mousePos.x < foodBowlPos.x && foodBowlPos.x > 50) {
                foodBowl.rectangle.setPosition(Vector2f(foodBowlPos.x - speed, foodBowlPos.y));
            }
            timeText.setString(to_string(start - totals.time + 14));
            window.draw(foodBag.rectangle);
            window.draw(foodBowl.rectangle);
            window.draw(timeText);
            window.display();
            
            while (const optional event = window.pollEvent()) {
                if (event->is<Event::Closed>()) {
                    utilitiesManager::close(window);
                }
            }
        }
    }
    return foodTotal;
}

void windowManager::vetVisit(RenderWindow& window) {
    RectangleImage catSick = creationManager::defineRectangleImage("catSick", Vector2f(200,200), Vector2f(150, 140));
    RectangleImage medkit = creationManager::defineRectangleImage("medKit", Vector2f(60,60), Vector2f(40, 160));
    RectangleImage syringe = creationManager::defineRectangleImage("syringe", Vector2f(20,40), Vector2f(40,100));
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Get the Syring\nand Stab Away");
    utilitiesManager::textRecenter(instructions, "middle");
    syringe.rectangle.rotate(degrees(-20));
    bool clickOne = false, clickTwo = false;
    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        if (clickOne && !clickTwo) {
            if (syringe.rectangle.getPosition().y <= 100)
                syringe.rectangle.setPosition(syringe.rectangle.getPosition() + Vector2f(0,1));
        }

        if (clickTwo && catSick.rectangle.getGlobalBounds().contains(syringe.rectangle.getPosition() - Vector2f(40,-10))) {
            creationManager::defineTexture(catSick.texture, "catNormal");
            window.draw(instructions);
            window.draw(medkit.rectangle);
            window.draw(catSick.rectangle);
            window.draw(syringe.rectangle);
            window.display();
            sleep(seconds(1));
            return;
        } else if (clickTwo) {
            syringe.rectangle.setPosition(Vector2f(Mouse::getPosition(window)));
        }
        if (clickOne)
            window.draw(syringe.rectangle);
        window.draw(instructions);
        window.draw(medkit.rectangle);
        window.draw(catSick.rectangle);
        window.display();

         while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (medkit.rectangle.getGlobalBounds().contains(mousePos))
                        clickOne = true;
                    else if (syringe.rectangle.getGlobalBounds().contains(mousePos) && clickOne) {
                        clickTwo = true;
                    }
                }
            }
        }
    }
}

int windowManager::playFrisbee(RenderWindow& window) {
    RectangleImage frisbee = creationManager::defineRectangleImage("frisbee", Vector2f(60,60), Vector2f(150, 200));
    RectangleImage cat = creationManager::defineRectangleImage("catNormal", Vector2f(100,100), Vector2f(150, 60));
    RectangleShape startPos = creationManager::defineRectangle(5,5,150,200);
    RectangleImage nice = creationManager::defineRectangleImage("exclamationPoint", Vector2f(50, 100), Vector2f(150, 100));
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Throw to your pet\nwith space");
    Text duration = creationManager::defineText(15, 0, 0, DEFAULT_GREEN, "10");
    utilitiesManager::textRecenter(instructions, "middle");
    Vector2f throwPos, catPos = Vector2f(150, 60);
    int score = 0, pastTime = totals.tick, startTime = totals.time, grabTime;
    bool active = false, canThrow = true;

    while (window.isOpen()) {
        if (startTime - totals.time + 10 <= 0)
            break;
        window.clear(Color(0,1,0));

        Vector2f catDif = cat.rectangle.getPosition() - Vector2f(catPos.x, catPos.y);

        if (frisbee.rectangle.getGlobalBounds().contains(cat.rectangle.getPosition()) && active) {
            score += 3;
            active = false;
            catPos = Vector2f(150,200);
            grabTime = totals.tick;
            nice.rectangle.setPosition(frisbee.rectangle.getPosition());
        }
        if (pastTime + 2 <= totals.tick) {
            catPos = Vector2f((rand() % 150) + 50, (rand() % 75) + 50);
            pastTime = totals.tick;
        }

        if (active) {
            Vector2f dif = frisbee.rectangle.getPosition() - Vector2f(throwPos.x, throwPos.y);
            frisbee.rectangle.setPosition(frisbee.rectangle.getPosition() - Vector2f((float) min(dif.x / 5, 5), min(dif.y / 5, 5)));
            if (frisbee.rectangle.getGlobalBounds().contains(throwPos))
                active = false;
        } else if (!canThrow) {
            if (frisbee.rectangle.getGlobalBounds().contains(cat.rectangle.getPosition())) {
                frisbee.rectangle.setPosition(frisbee.rectangle.getPosition() - Vector2f(min((frisbee.rectangle.getPosition().x - 150) / 5, 5), min((frisbee.rectangle.getPosition().y - 200) / 5, 5)));
                cat.rectangle.setPosition(frisbee.rectangle.getPosition());
                if (startPos.getGlobalBounds().contains(frisbee.rectangle.getPosition())) {
                    canThrow = true;
                    catPos = Vector2f((rand() % 150) + 50, (rand() % 75) + 50);
                    pastTime = totals.tick;
                }
            } else {
                catPos = frisbee.rectangle.getPosition();
            }
        }

        duration.setString(to_string(startTime - totals.time + 10));
        window.draw(duration);
        window.draw(frisbee.rectangle);
        window.draw(cat.rectangle);
        window.draw(instructions);
        if (grabTime >= totals.tick) {
            window.draw(nice.rectangle);
            window.display();
            sleep(chrono::milliseconds(200));
        } else if (catPos != cat.rectangle.getPosition())
            cat.rectangle.setPosition(cat.rectangle.getPosition() - Vector2f(min(catDif.x / 5, 5), min(catDif.y / 5, 5)));
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }
            
            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && canThrow) {
                active = true, canThrow = false;
                throwPos = Vector2f(Mouse::getPosition(window));
            }
        }
    }
    return score;
}

void windowManager::cleanPet(RenderWindow& window) {
    vector<bool> triggers(4,false);
    vector<pair<int,int>> bubbleCoords = {{102,68},{100,100},{103,112}, {133,72},{130,97},{127,128}, {190,50},{175,93},{210,143}, {100,170},{150,172},{173, 163}};
    vector<RectangleImage> bubbles;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Grab The Spray");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleImage cat = creationManager::defineRectangleImage("catDirty", Vector2f(200,200), Vector2f(150,125));
    RectangleImage spray = creationManager::defineRectangleImage("spray", Vector2f(20,40),Vector2f(50,130));
    RectangleImage soap = creationManager::defineRectangleImage("soap", Vector2f(50,40), Vector2f(50, 130));
    RectangleImage spraySFX = creationManager::defineRectangleImage("spraySFX", Vector2f(20,20),Vector2f(0,0));
    int sfxOffset = 0, sprayCount = 0;
    while (window.isOpen()) {
        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));
        window.clear(Color(0,1,0));
        if (triggers[3]) {
            sleep(chrono::seconds(2));
            break;
        }
        if (triggers[2] == true) {
            soap.rectangle.setPosition(mouseCoords);
            for (int i = 0; i < bubbleCoords.size(); i++) {
                if (soap.rectangle.getGlobalBounds().contains(Vector2f(bubbleCoords[i].first, bubbleCoords[i].second))) {
                    bubbles.push_back(creationManager::defineRectangleImage("bubble", Vector2f(30,30), Vector2f(bubbleCoords[i].first, bubbleCoords[i].second)));
                    bubbles.push_back(creationManager::defineRectangleImage("bubble", Vector2f(30,30), Vector2f(bubbleCoords[i].first - 10, bubbleCoords[i].second + 3)));
                    bubbles.push_back(creationManager::defineRectangleImage("bubble", Vector2f(30,30), Vector2f(bubbleCoords[i].first - 12, bubbleCoords[i].second - 7)));
                    bubbleCoords.erase(bubbleCoords.begin() + i);
                    i--;
                }
            }
        } else if (sprayCount >= 5) {
            instructions.setString("Grab The Soap");
            utilitiesManager::textRecenter(instructions, "middle");
            triggers[1] = true;
        } else if (triggers[0]) {
            spray.rectangle.setPosition(mouseCoords);
        }
        if (bubbleCoords.size() == 0 && bubbles.size() != 0) {
            triggers[3] = true;
            instructions.setString("Good Job");
            utilitiesManager::textRecenter(instructions, "middle");
            creationManager::defineTexture(cat.texture, "catNormal");
            for (int i = bubbles.size() - 1; i >= 0; i--) {
                bubbles.pop_back();
            }
        }
        window.draw(instructions);
        window.draw(cat.rectangle);
        if (sfxOffset - totals.tick > 0 && !triggers[1])
            window.draw(spraySFX.rectangle);
        for (auto bubble : bubbles)
            window.draw(bubble.rectangle);
        if (!triggers[1])
            window.draw(spray.rectangle);
        else
            window.draw(soap.rectangle);
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    if (triggers[1] && soap.rectangle.getGlobalBounds().contains(mousePos)) {
                        triggers[2] = true;
                        instructions.setString("SCRUB!");
                        utilitiesManager::textRecenter(instructions, "middle");
                    } else if (triggers[0]) {
                        sfxOffset = totals.tick + 1;
                        spraySFX.rectangle.setPosition(mousePos - Vector2f(20,10));
                        if (cat.rectangle.getGlobalBounds().contains(mousePos - Vector2f(20,10)))
                            sprayCount++;
                    } else if (spray.rectangle.getGlobalBounds().contains(mousePos)) {
                        triggers[0] = true;
                        instructions.setString("SPRAY!");
                        utilitiesManager::textRecenter(instructions, "middle");
                    }
                }
            }
        }
    }
}

void windowManager::taskMenu(RenderWindow& window) {
    RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
    RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
    RectangleImage arrowForward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
    RectangleImage arrowBackward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
    arrowBackward.rectangle.rotate(degrees(180));

    int pageNum = 0;
    vector<listItem> taskList;
    taskList.push_back(creationManager::defineListItem("foodBag", "FEED", "feed him", -1, 0));
    taskList.push_back(creationManager::defineListItem("catSad", "Cheer Up", "Make Happy", -1, 1));
    taskList.push_back(creationManager::defineListItem("soapBottle", "Clean Bro Up", "He Dirty", -1, 2));
    taskList.push_back(creationManager::defineListItem("medKit", "Heal", "help ur\nsick pet", -1, 3));
    taskList.push_back(creationManager::defineListItem("frisbee", "Play", "Play with bro", -1, 4));
    taskList.push_back(creationManager::defineListItem("statsImage", "stats", "see your stats", -1, taskList.size()));
    vector<int> taskOrder;
    
    if (stats.record[0] - '0')
        taskOrder.push_back(0);
    if (stats.mood == "sad")
        taskOrder.push_back(1);
    if (stats.record[4] - '0' && stats.mood == "dirty")
        taskOrder.push_back(2);
    if (stats.mood == "sick" && stats.record[1] - '0')
        taskOrder.push_back(3);
    if (stats.record[3] - '0' && frisbeeDelay - totals.time <= 0)
        taskOrder.push_back(4);
    taskOrder.push_back(taskList.size()-1);
    
    while (window.isOpen()) {
        if (totals.tick % 10 == 0) {
            if (stats.record[0] - '0' && find(taskOrder.begin(), taskOrder.end(), 0) == taskOrder.end())
                taskOrder.push_back(0);
            if (stats.mood == "sad" && find(taskOrder.begin(), taskOrder.end(), 1) == taskOrder.end())
                taskOrder.push_back(1);
            if (stats.record[4] - '0' && stats.mood == "dirty" && find(taskOrder.begin(), taskOrder.end(), 2) == taskOrder.end())
                taskOrder.push_back(2);
            if (stats.mood == "sick" && stats.record[1] - '0' && find(taskOrder.begin(), taskOrder.end(), 3) == taskOrder.end())
                taskOrder.push_back(3);
            if (stats.record[3] - '0' && find(taskOrder.begin(), taskOrder.end(), 4) == taskOrder.end() && frisbeeDelay - totals.time <= 0)
                taskOrder.push_back(4);
        }
        window.clear(Color(0,1,0));
        //MessageBoxA(NULL, to_string(frisbeeDelay - totals.time).c_str(), "debug", MB_OK);
        window.draw(background.rectangle);
        window.draw(close.rectangle);
        if ((int)pageNum + 1 <= taskOrder.size()/3 && taskOrder.size()/3 != pageNum + 1)
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
                utilitiesManager::close(window);
            }
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < taskOrder.size(); i++) {
                        listItem& item = taskList[taskOrder[i]];
                        if (item.buy.rectangle.getGlobalBounds().contains(mousePos)) {
                            if (item.id == 0) {
                                stats.hunger += foodMini(window);
                                stats.record[0] = '0';
                                totals.foodEaten++;
                                taskOrder.erase(taskOrder.begin() + i);
                            } else if (item.id == 1) {
                                stats.happiness += 35;
                                taskOrder.erase(taskOrder.begin() + i);
                            } else if (item.id == 2) {
                                stats.mood = "normal";
                                cleanPet(window);
                                taskOrder.erase(taskOrder.begin() + i);
                            } else if (item.id == 3) {
                                stats.mood = "normal";
                                vetVisit(window);
                                stats.record[1] = '0';
                                taskOrder.erase(taskOrder.begin() + i);
                            } else if (item.id == 4) {
                                stats.happiness += playFrisbee(window);
                                taskOrder.erase(taskOrder.begin() + i);
                                frisbeeDelay = 60 + totals.time;
                            } else if (item.id == 5) {
                                statDisplay(window);
                            }
                            if (rand() % 3 == 0 && item.id != 5 && item.id != 2)
                                stats.mood = "dirty"; 
                        }
                    }
                    if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                        pageNum--;
                    else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= taskOrder.size()/3 && taskOrder.size()/3 != pageNum + 1)
                        pageNum++;
                    else if (close.rectangle.getGlobalBounds().contains(mousePos))
                        return;
                }

            }
        }
    }
}