#include <Windows.h>
#include "creationManager.hpp"
#include "utilityManager.hpp"
#include "windowManager.hpp"
#include "globals.hpp"

void windowManager::petType() {
    string type = "N/A";
    Text instruction = creationManager::defineText(25, 150,50, DEFAULT_GREEN, "   What Type of\nPet Do You Want");
    utilitiesManager::textRecenter(instruction, "middle");
    RectangleImage dog = creationManager::defineRectangleImage("dogNormal", Vector2f(75,75), Vector2f(50,150));
    RectangleImage cat = creationManager::defineRectangleImage("catNormal", Vector2f(75,75), Vector2f(150,150));
    RectangleImage bear = creationManager::defineRectangleImage("bearNormal", Vector2f(75,75), Vector2f(250,150));
    RectangleImage select = creationManager::defineRectangleImage("select", Vector2f(150,50), Vector2f(150,200));
    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        window.draw(dog.rectangle);
        window.draw(cat.rectangle);
        window.draw(bear.rectangle);
        window.draw(instruction);
        window.draw(select.rectangle);

        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                return;
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(mousePressed->position);
                    if (select.rectangle.getGlobalBounds().contains(mousePos) && type != "N/A") {
                        stats.type = type;
                        return;
                    } else if (dog.rectangle.getGlobalBounds().contains(mousePos)) {
                        type = "dog";
                        dog.rectangle.setSize(Vector2f(90,90));
                        cat.rectangle.setSize(Vector2f(75,75));
                        bear.rectangle.setSize(Vector2f(75,75));
                        dog.rectangle.setPosition(Vector2f(50,150));
                    } else if (cat.rectangle.getGlobalBounds().contains(mousePos)) {
                        type = "cat";
                        dog.rectangle.setSize(Vector2f(75,75));
                        cat.rectangle.setSize(Vector2f(90,90));
                        bear.rectangle.setSize(Vector2f(75,75));
                        cat.rectangle.setPosition(Vector2f(150,150));
                    } else if (bear.rectangle.getGlobalBounds().contains(mousePos)) {
                        type = "bear";
                        dog.rectangle.setSize(Vector2f(75,75));
                        cat.rectangle.setSize(Vector2f(75,75));
                        bear.rectangle.setSize(Vector2f(90,90));
                        bear.rectangle.setPosition(Vector2f(250,150));
                    }
                }
            }
        }

    }
}

string windowManager::enterName() {
    bool typing = true;
    Text nameText = creationManager::defineText(15, 150, 150, DEFAULT_GREEN, "");
    Text namePrompt = creationManager::defineText(25, 50, 50, DEFAULT_GREEN, "What is Your\n Pets Name?");
    while (window.isOpen() && typing) {
        window.clear(Color(0,1,0));
        window.draw(nameText);
        window.draw(namePrompt);
        window.display();

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>())
                return "N/A";
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


void windowManager::shopMenu() {
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
    shopItems.push_back(creationManager::defineListItem("close", "Brush", "Groom Your Pet", 25, 4));
    shopItems.push_back(creationManager::defineListItem("closeWasher", "DishWasher", "Clean Dishes for your parents", 50, 5));
    shopItems.push_back(creationManager::defineListItem("soapBottle", "Cleaning Supplies", "Clean Broto", 10, 6));
    shopItems.push_back(creationManager::defineListItem("catSleepy", "Sleep", "Let your cat rest decreasing\nhunger lost when inactive", 50, 7));
    shopItems.push_back(creationManager::defineListItem(stats.type + "Rich", "MONEY", "SO MONEY", 100, 8));
    for (int i = 0; i < shopItems.size(); i++) {
        if (stats.record[shopItems[i].id] == '0') {
            itemOrder.push_back(shopItems[i].id);
        } else {
            for (int j = i + 1; j < shopItems.size(); j++)
                utilitiesManager::rollListItem(shopItems[j], shopItems[j].pos - 1);
        }
    }


    while (window.isOpen()) {
        window.clear(Color(0,1,0));
        money.setString('$' + to_string(stats.money));
        window.draw(background.rectangle);
        window.draw(close.rectangle);
        for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < itemOrder.size(); i++) {
            int ind = itemOrder[i];
            window.draw(shopItems[ind].image.rectangle);
            window.draw(shopItems[ind].buy.rectangle);
            window.draw(shopItems[ind].cost);
            window.draw(shopItems[ind].description);
            window.draw(shopItems[ind].title);
        }
        window.draw(money);

        if ((int)pageNum + 1 <= itemOrder.size()/3 && (float)itemOrder.size()/3 != pageNum + 1)
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
                                for (int j = i + 1; j < itemOrder.size(); j++)
                                    utilitiesManager::rollListItem(shopItems[itemOrder[j]], shopItems[itemOrder[j]].pos - 1);
                                itemOrder.erase(itemOrder.begin() + i);
                                break;
                            }
                        }
                    }
                    if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                        pageNum--;
                    else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= itemOrder.size()/3 && (float)itemOrder.size()/3 != pageNum + 1)
                        pageNum++;
                    else if (close.rectangle.getGlobalBounds().contains(mousePos))
                        return;
                }

            }
        }
    }
}

void windowManager::statDisplay() {
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

int windowManager::foodMini() {
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

void windowManager::vetVisit() {
    RectangleImage catSick = creationManager::defineRectangleImage(stats.type + "Sick", Vector2f(200,200), Vector2f(150, 140));
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
            creationManager::defineTexture(catSick.texture, stats.type + "Normal");
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

int windowManager::playFrisbee() {
    RectangleImage frisbee = creationManager::defineRectangleImage("frisbee", Vector2f(60,60), Vector2f(150, 200));
    RectangleImage cat = creationManager::defineRectangleImage(stats.type + "Normal", Vector2f(100,100), Vector2f(150, 60));
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

void windowManager::takeOutTrash() {
    vector<bool> triggers(4, false);
    vector<RectangleImage> trash(8, creationManager::defineRectangleImage("trash", Vector2f(30,30), Vector2f(150, 120)));
    trash[0].rectangle.setPosition(Vector2f(20,220));
    trash[1].rectangle.setPosition(Vector2f(23,220));
    trash[2].rectangle.setPosition(Vector2f(77,220));
    trash[3].rectangle.setPosition(Vector2f(140, 120));
    trash[4].rectangle.setPosition(Vector2f(123, 120));
    trash[5].rectangle.setPosition(Vector2f(240, 220));
    trash[6].rectangle.setPosition(Vector2f(172, 220));
    int trashCount = 0, curTrash = -1;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Grab The Trash and Put it In the Bin");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleImage trashBin = creationManager::defineRectangleImage("trashBin", Vector2f(40,50), Vector2f(260,210));
    RectangleImage dumpster = creationManager::defineRectangleImage("openDumpster", Vector2f(100, 140), Vector2f(150, 145));
    RectangleImage lid = creationManager::defineRectangleImage("dumpsterLid", Vector2f(100, 50), Vector2f(150, 75));
    RectangleImage bag = creationManager::defineRectangleImage("trashBag", Vector2f(50,50), Vector2f(0,0));
    RectangleShape trashHitbox = creationManager::defineRectangle(20,10, 260, 180);
    RectangleImage table = creationManager::defineRectangleImage("table", Vector2f(100,100), Vector2f(150,180));
    while (window.isOpen()) {
        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));
        window.clear(Color(0,1,0));

        if (trashCount >= 8 && !triggers[0]) {
            triggers[0] = true;
            instructions.setString("Click The Bin");
            utilitiesManager::textRecenter(instructions, "middle");
        }

        window.draw(instructions);
        if (!triggers[1]) {
            window.draw(table.rectangle);
            window.draw(trashBin.rectangle);
            for (auto piece : trash)
                window.draw(piece.rectangle);
        } else {
            if (!triggers[3])
                window.draw(lid.rectangle);
            window.draw(dumpster.rectangle);
            if (!triggers[2]) {
                bag.rectangle.setPosition(mouseCoords);
                window.draw(bag.rectangle);
            }
        }
        
        window.display();

        if (triggers[3]) {
            sleep(chrono::seconds(1));
            break;
        }

        if (!triggers[0]) {
            for (int i = 0; i < trash.size(); i++) {
                Vector2f pos = trash[i].rectangle.getPosition();       
                if (trashHitbox.getGlobalBounds().contains(pos) && curTrash != i) {
                    trashCount++;
                    trash.erase(trash.begin() + i);
                    i--;
                    continue;
                }
                if (curTrash != i) {
                    trash[i].rectangle.setPosition(pos + (((pos.x < 100 || pos.x > 200) && pos.y < 220) || (pos.y < 120) ? Vector2f(0,3) : Vector2f(0,0)));
                } else {
                    trash[i].rectangle.setPosition(mouseCoords);
                }
            }
        }

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    if (!triggers[0]) {
                        if (curTrash != -1) {
                            //MessageBoxA(NULL, to_string(curTrash).c_str(), "DEBUG", MB_OK);
                            if (!table.rectangle.getGlobalBounds().contains(mousePos))
                                curTrash = -1;
                        } else {
                            for (int i = 0; i < trash.size(); i++) {
                                if (trash[i].rectangle.getGlobalBounds().contains(mousePos)) {
                                    curTrash = i;
                                    break; 
                                }
                            }
                        }
                    } else if (!triggers[1]) {
                        if (trashBin.rectangle.getGlobalBounds().contains(mousePos)) {
                            triggers[1] = true; 
                            instructions.setString("Put in the Dumpster");
                            utilitiesManager::textRecenter(instructions, "middle");
                        }
                    } else if (!triggers[2]) {
                        if (dumpster.rectangle.getGlobalBounds().contains(mousePos)) {
                            instructions.setString("Close The Lid");
                            utilitiesManager::textRecenter(instructions, "middle");
                            triggers[2] = true;
                        }
                    } else {
                        if (lid.rectangle.getGlobalBounds().contains(mousePos)) {
                            triggers[3] = true;
                            instructions.setString("Good Job!");
                            creationManager::defineTexture(dumpster.texture, "closeDumpster");
                            utilitiesManager::textRecenter(instructions, "middle");
                        }
                    }
                }
            }
        }
    }
}

void windowManager::cleanPet() {
    vector<bool> triggers(4,false);
    vector<pair<int,int>> bubbleCoords = {{102,68},{100,100},{103,112}, {133,72},{130,97},{127,128}, {190,50},{175,93},{210,143}, {100,170},{150,172},{173, 163}};
    vector<RectangleImage> bubbles;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Grab The Spray");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleImage cat = creationManager::defineRectangleImage(stats.type + "Dirty", Vector2f(200,200), Vector2f(150,125));
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
            creationManager::defineTexture(cat.texture, stats.type + "Normal");
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

void windowManager::carWash() {
    vector<bool> triggers(3, false);
    int carDir = 0, sprayDelay = 0, shineCount = 0, bubbleCount = 0;
    vector<int> sprayCount(4,0);
    vector<pair<int,int>> bubbleCoords = {{102,88},{100,122},{103,183}, {183,86},{180,132},{187,189}, {150,74},{150,105},{153, 182}};
    vector<pair<RectangleImage, vector<int>>> bubbles;
    for (auto coords : bubbleCoords) 
        bubbles.push_back({creationManager::defineRectangleImage("bubble", Vector2f(30,30), Vector2f(coords.first, coords.second)), {0,0,0,0}});
    RectangleImage carEven = creationManager::defineRectangleImage("carFront", Vector2f(100,150), Vector2f(150,150));
    RectangleImage carOdd = creationManager::defineRectangleImage("carSide", Vector2f(200,150), Vector2f(150,150));
    RectangleImage spray = creationManager::defineRectangleImage("spray", Vector2f(10,30), Vector2f(0,0));
    RectangleImage soap = creationManager::defineRectangleImage("soap", Vector2f(30,20), Vector2f(0,0));
    RectangleImage cloth = creationManager::defineRectangleImage("cloth", Vector2f(30,30), Vector2f(0,0));
    RectangleImage spraySFX = creationManager::defineRectangleImage("spraySFX", Vector2f(15,15), Vector2f(0,0));
    RectangleImage turnRight = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(250, 150));
    RectangleImage turnLeft = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(50,150));
    turnLeft.rectangle.rotate(degrees(180));
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Spray Each Side of The Car");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleShape buttonHitbox = creationManager::defineRectangle(20,20, 210, 60);
    buttonHitbox.setFillColor(Color(67,67,67));

    while (window.isOpen()) {
        bool valid = true;
        for (int i : sprayCount)
            valid = (i >= 2) && valid;
        triggers[0] = valid;

        if (bubbleCount >= bubbleCoords.size() * 3 && !triggers[1])
            triggers[1] = true;
        
        if (shineCount >= bubbleCount && triggers[1]) {
            soap.rectangle.setPosition(Vector2f(0,0));
            triggers[2] = true;
        }

        if (triggers[0] && !triggers[1]) {
            instructions.setString("Scrub Each Side of The Car");
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (triggers[1] && !triggers[2]) {
            instructions.setString("Wash Off Each Side of The Car");
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (triggers[2]) {
            instructions.setString("Good Job");
            utilitiesManager::textRecenter(instructions, "middle");
        }

        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));

        window.clear(Color(0,1,0));
        if (carDir % 2 == 0)
            window.draw(carEven.rectangle);
        else
            window.draw(carOdd.rectangle);
        for (auto bubble : bubbles) {
            if (bubble.second[carDir] == 1)
                creationManager::defineTexture(bubble.first.texture, "bubble");
            else if (bubble.second[carDir] == 2)
                creationManager::defineTexture(bubble.first.texture, "shine");
            if (bubble.second[carDir] >= 1)
                window.draw(bubble.first.rectangle);
        }
        window.draw(turnRight.rectangle);
        window.draw(turnLeft.rectangle);
        if (sprayDelay - totals.tick > 0)
            window.draw(spraySFX.rectangle);

         if (triggers[1]) {
            cloth.rectangle.setPosition(mouseCoords);
            window.draw(cloth.rectangle);
        } else if (triggers[0]) {
            soap.rectangle.setPosition(mouseCoords);
            window.draw(soap.rectangle);
        } else {
            spray.rectangle.setPosition(mouseCoords);
            window.draw(spray.rectangle);
        }
        window.draw(instructions);

        window.display();

        if (triggers[2]) {
            sleep(chrono::seconds(2));
            break;
        }

        for (int i = 0; i < bubbleCoords.size(); i++) {
            if (soap.rectangle.getGlobalBounds().contains(Vector2f(bubbleCoords[i].first, bubbleCoords[i].second))) {
                if (bubbles[i].second[carDir] == 0) {
                    bubbleCount++;
                    bubbles[i].second[carDir] = 1;
                }
            }
            if (cloth.rectangle.getGlobalBounds().contains(bubbles[i].first.rectangle.getPosition())) {
                if (bubbles[i].second[carDir] == 1) {
                    shineCount++;
                    bubbles[i].second[carDir] = 2;
                }
            }
        }

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    if (turnRight.rectangle.getGlobalBounds().contains(mousePos)) {
                        carDir = (carDir + 1) % 4;
                        if (carDir == 0)
                            creationManager::defineTexture(carEven.texture, "carFront");
                        else if (carDir == 2)
                            creationManager::defineTexture(carEven.texture, "carBack");
                    } else if (turnLeft.rectangle.getGlobalBounds().contains(mousePos)) {
                        carDir = (carDir - 1 + 4) % 4;
                        if (carDir == 0)
                            creationManager::defineTexture(carEven.texture, "carFront");
                        else if (carDir == 2)
                            creationManager::defineTexture(carEven.texture, "carBack");
                    } else if (!triggers[0]) {
                        sprayCount[carDir]++;
                        sprayDelay = totals.tick + 1;
                        spraySFX.rectangle.setPosition(mousePos - Vector2f(15, 15));
                    }
                }
            }
        }
    }
}

void windowManager::dishWash() {
    vector<bool> triggers(4, false);
    vector<int> activePlates;
    int plateCount = 0, curPlate = -1;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Open the door");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleImage dishWasher = creationManager::defineRectangleImage("closeWasher", Vector2f(150,200), Vector2f(150,150));
    RectangleImage plateStack = creationManager::defineRectangleImage("plateStack", Vector2f(40,80), Vector2f(150,50));
    vector<RectangleImage> plates(8, creationManager::defineRectangleImage("plate", Vector2f(40,20), Vector2f(150,50)));
    vector<RectangleShape> slots(8, creationManager::defineRectangle(20, 40, 0, 0));
    for (int i = 0; i < 8; i++) {
        slots[i].setFillColor(DARK_GREEN);
        slots[i].setPosition(Vector2f((i % 4) * 28 + 105, (int)i / 4 * 100 + 90));
    }
    RectangleShape buttonHitbox = creationManager::defineRectangle(20,20, 210, 60);
    buttonHitbox.setFillColor(Color(67,67,67));

    while (window.isOpen()) {
        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));
        window.clear(Color(0,1,0));
        if (plateCount >= plates.size() && !triggers[1]) {
            instructions.setString("Close the Door");
            utilitiesManager::textRecenter(instructions, "middle");
            triggers[1] = true;
        }
        
        if (triggers[3]) {
            instructions.setString("Good Job!");
            utilitiesManager::textRecenter(instructions, "middle");
        }

        if (!triggers[1])
            window.draw(plateStack.rectangle);
        window.draw(dishWasher.rectangle);
        if (triggers[0] && !triggers[2]) {
            for (int i = 0; i < plates.size(); i++) {
                if (find(activePlates.begin(), activePlates.end(), i) != activePlates.end())
                    window.draw(plates[i].rectangle);
            }
            if (curPlate != -1) {
                plates[curPlate].rectangle.setPosition(mouseCoords);
                window.draw(plates[curPlate].rectangle);
            }
        }
        window.draw(instructions);

        window.display();

        if (triggers[3]) {
            sleep(chrono::seconds(3));
            break;
        }

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    if (triggers[2])
                        if (buttonHitbox.getGlobalBounds().contains(mousePos))
                            triggers[3] = true;

                    if (!triggers[0] || triggers[1])
                        if (dishWasher.rectangle.getGlobalBounds().contains(mousePos))
                            if (!triggers[0]) {
                                triggers[0] = true;
                                creationManager::defineTexture(dishWasher.texture, "openWasher");
                                instructions.setString("put in plates from the stack");
                                utilitiesManager::textRecenter(instructions, "middle");
                            } else {
                                triggers[2] = true;
                                creationManager::defineTexture(dishWasher.texture, "closeWasher");
                                instructions.setString("Press the on button");
                                utilitiesManager::textRecenter(instructions, "middle");
                            }
                    if (triggers[0]) {
                        if (curPlate != -1) {
                            for (int i = 0; i < slots.size(); i++) {
                                if (slots[i].getGlobalBounds().contains(mousePos)) {
                                    plates[curPlate].rectangle.setPosition(slots[i].getPosition());
                                    activePlates.push_back(curPlate);
                                    curPlate = -1;
                                    slots.erase(slots.begin() + i);
                                    plateCount++;
                                    break;
                                }
                            }
                        } else {
                            if (plateStack.rectangle.getGlobalBounds().contains(mousePos)) {
                                curPlate = activePlates.size();
                                plates[curPlate].rectangle.setRotation(degrees(90));
                            }
                        }
                    }
                }
            }
        }
    }
}

void windowManager::cheerUp() {
    vector<bool> triggers(4, false);
    int pettingStart = -1, pettingTime = -1, petCount = 0;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Wipe Tears");
    utilitiesManager::textRecenter(instructions, "middle");
    vector<RectangleImage> tears;
    for (int i = 0; i < 6; i++)
        tears.push_back(creationManager::defineRectangleImage("tear", Vector2f(30,30), Vector2f((i / 3 < 1) ? 120 : 180, ((int)i / 2) * 40 + 100)));
    RectangleImage cat = creationManager::defineRectangleImage(stats.type + "Sad", Vector2f(250,250), Vector2f(150,125));
    RectangleImage hand = creationManager::defineRectangleImage("Hand", Vector2f(45,45),Vector2f(150,70));
    RectangleImage cloth = creationManager::defineRectangleImage("cloth", Vector2f(60,60), Vector2f(0,0));
    RectangleShape noseHitbox = creationManager::defineRectangle(20,20, 150, 130);

    while (window.isOpen()) {

        if (tears.size() == 0 && !triggers[0]) {
            triggers[0] = true;
            instructions.setString("Pet " + stats.name);
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (petCount >= 3 && pettingTime < 0 && !triggers[1]) {
            triggers[1] = true;
           instructions.setString("Boop The Nose");
            utilitiesManager::textRecenter(instructions, "middle");
        }

        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));
        window.clear(Color(0,1,0));
        cloth.rectangle.setPosition(mouseCoords);
        window.draw(cat.rectangle);
        for (auto tear : tears)
            window.draw(tear.rectangle);
        if (!triggers[0])
            window.draw(cloth.rectangle);
        window.draw(instructions);

        if (pettingTime >= 0) {
            pettingTime = pettingStart - totals.tick + pettingTime;
            if (pettingTime >= 24)
                hand.rectangle.rotate(degrees(2));
            else if (pettingTime >= 9)
                hand.rectangle.rotate(degrees(-2));
            else
                hand.rectangle.rotate(degrees(2));
            window.draw(hand.rectangle);
            if (pettingTime <= 0) {
                hand.rectangle.setRotation(degrees(0));
                pettingStart = 0;
                pettingTime = -1;
            }
        }

        window.display();

        if (triggers[2]) {
            sleep(chrono::seconds(2));
            break;
        }

        for (int i = 0; i < tears.size(); i++)
            if (cloth.rectangle.getGlobalBounds().contains(tears[i].rectangle.getPosition()))
                tears.erase(tears.begin() + i);


        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    if (triggers[0] && !triggers[1]) {
                        pettingStart = totals.tick, pettingTime = 27;
                        petCount++;
                    } else if (triggers[1] && noseHitbox.getGlobalBounds().contains(mousePos)) {
                        triggers[2] = true;
                        instructions.setString("Good Job!");
                        utilitiesManager::textRecenter(instructions, "middle");
                        creationManager::defineTexture(cat.texture, stats.type + "Normal");
                    }
                }
            }
        }
    }
}

void windowManager::brushPet() {
    vector<bool> triggers(4,false);
    bool paceForward = true;
    int timeCorrect = 0, lastTick = totals.tick, pace = 1;
    Text instructions = creationManager::defineText(15, 150, 30, DEFAULT_GREEN, "Follow The Sparkles");
    utilitiesManager::textRecenter(instructions, "middle");
    RectangleImage cat = creationManager::defineRectangleImage(stats.type + "Normal", Vector2f(250,250), Vector2f(150,125));
    RectangleImage brush = creationManager::defineRectangleImage("brush", Vector2f(90,90), Vector2f(0,110));
    RectangleImage paceSetter = creationManager::defineRectangleImage("sparkles", Vector2f(30,150), Vector2f(80,110));

    while (window.isOpen()) {
        if (timeCorrect >= 10 && !triggers[0]) {
            pace += 2;
            triggers[0] = true;
            instructions.setString("Faster");
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (timeCorrect >= 35 && !triggers[1]) {
            pace++;
            triggers[1] = true;
            instructions.setString("Faster!!!");
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (timeCorrect >= 60 && !triggers[2]) {
            pace += 3;
            triggers[2] = true;
            instructions.setString("EVEN FASTER!!!!!");
            utilitiesManager::textRecenter(instructions, "middle");
        } else if (timeCorrect >= 85) {
            triggers[3] = true;
            instructions.setString("Good Job!");
            utilitiesManager::textRecenter(instructions, "middle");
        }

        Vector2f mouseCoords = Vector2f(Mouse::getPosition(window));
        window.clear(Color(0,1,0));
        if (paceForward)
            if (paceSetter.rectangle.getPosition().x < 220)
                paceSetter.rectangle.setPosition(paceSetter.rectangle.getPosition() + Vector2f(pace,0));
            else
                paceForward = false;
        else
            if (paceSetter.rectangle.getPosition().x > 80)
                paceSetter.rectangle.setPosition(paceSetter.rectangle.getPosition() - Vector2f(pace,0));
            else
                paceForward = true;
        brush.rectangle.setPosition(Vector2f(mouseCoords.x, brush.rectangle.getPosition().y));
        window.draw(cat.rectangle);
        window.draw(brush.rectangle);
        window.draw(paceSetter.rectangle);
        window.draw(instructions);

        window.display();
        
        if (paceSetter.rectangle.getGlobalBounds().contains(mouseCoords)) {
            if (lastTick != totals.tick) {
                lastTick = totals.tick;
                timeCorrect++;
            }
        }

        if (triggers[3]) {
            sleep(chrono::seconds(2));
            break;
        }
        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                utilitiesManager::close(window);
            }

            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = Vector2f(mousePressed->position);
                    
                }
            }
        }
    }
}

void windowManager::taskMenu() {
    RectangleImage background = creationManager::defineRectangleImage("shopWindow", Vector2f(300,200), Vector2f(150,100));
    RectangleImage close = creationManager::defineRectangleImage("close", Vector2f(30,27.5), Vector2f(280,20));
    RectangleImage arrowForward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(280,180));
    RectangleImage arrowBackward = creationManager::defineRectangleImage("cornerGo", Vector2f(30,30), Vector2f(20,180));
    arrowBackward.rectangle.rotate(degrees(180));

    int pageNum = 0;
    vector<listItem> taskList;
    taskList.push_back(creationManager::defineListItem("foodBag", "FEED", "feed him", -1, 0));
    taskList.push_back(creationManager::defineListItem(stats.type + "Sad", "Cheer Up", "Make Happy", -1, 1));
    taskList.push_back(creationManager::defineListItem("soapBottle", "Clean Bro Up", "He Dirty", -1, 2));
    taskList.push_back(creationManager::defineListItem("medKit", "Heal", "help ur\nsick pet", -1, 3));
    taskList.push_back(creationManager::defineListItem("trash", "Take Out Trash", "Clean Up for some Moolah", -1, 4));
    taskList.push_back(creationManager::defineListItem("plateStack", "Wash Dishes", "Make that money", -1, 5));
    taskList.push_back(creationManager::defineListItem("soapBottle", "Wash The Car", "Make some moolah", -1, 6));
    taskList.push_back(creationManager::defineListItem("frisbee", "Play", "Play with bro", -1, 7));
    taskList.push_back(creationManager::defineListItem("brush", "Groom Pet", "Brush That Dude", -1, 8));
    taskList.push_back(creationManager::defineListItem("statsImage", "stats", "see your stats", -1, 9));
    vector<int> taskOrder;
    
    utilitiesManager::taskListCheck(taskList, taskOrder);
    taskOrder.push_back(taskList.size() - 1);
    
    while (window.isOpen()) {
        if (totals.tick % 10 == 0) {
            utilitiesManager::taskListCheck(taskList, taskOrder);
            sort(taskOrder.begin(), taskOrder.end());
        }
        window.clear(Color(0,1,0));
        window.draw(background.rectangle);
        window.draw(close.rectangle);
        if ((int)pageNum + 1 <= taskOrder.size()/3 && (float)taskOrder.size()/3 != pageNum + 1)
            window.draw(arrowForward.rectangle);
        if ((int)pageNum > 0)
            window.draw(arrowBackward.rectangle);
        for (int i = pageNum * 3; i <= pageNum * 3 + 2 && i < taskOrder.size(); i++) {
            listItem& item = taskList[taskOrder[i]];
            utilitiesManager::rollListItem(item, i);
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
                            switch (item.id) {
                                case 0:
                                    stats.hunger += foodMini();
                                    stats.record[0] = '0';
                                    totals.foodEaten++;
                                    break;
                                case 1:
                                    cheerUp();
                                    stats.happiness += 10;
                                    break;
                                case 2:
                                    stats.mood = "Normal";
                                    cleanPet();
                                    break;
                                case 3:
                                    stats.mood = "Normal";
                                    vetVisit();
                                    stats.record[1] = '0';
                                    break;
                                case 4:
                                    takeOutTrash();
                                    trashDelay = totals.time + 60;
                                    break;
                                    stats.money += 20;
                                case 5:
                                    dishWash();
                                    dishDelay = totals.time + 90;
                                    stats.money += 30;
                                    break;
                                case 6:
                                    carWash();
                                    carWashDelay = totals.time + 120;
                                    break;
                                    stats.money += 30;
                                case 7:
                                    stats.happiness += playFrisbee();
                                    frisbeeDelay = 60 + totals.time;
                                    break;
                                case 8:
                                    brushPet();
                                    stats.happiness += 25;
                                    brushDelay = 120 + totals.time;
                                    break;
                                case 9:
                                    statDisplay();
                                    break;
                            }
                            if (item.id != 9)
                                taskOrder.erase(taskOrder.begin() + i);
                            if (rand() % 3 == 0 && item.id != taskOrder.size() - 1 && item.id != 2)
                                stats.mood = "Dirty";
                            break;
                        }
                    }
                    if (arrowBackward.rectangle.getGlobalBounds().contains(mousePos) && pageNum > 0)
                        pageNum--;
                    else if (arrowForward.rectangle.getGlobalBounds().contains(mousePos) && (int)pageNum + 1 <= taskOrder.size()/3 && (float)taskOrder.size()/3 != pageNum + 1)
                        pageNum++;
                    else if (close.rectangle.getGlobalBounds().contains(mousePos))
                        return;
                }
            }
        }
    }
}