#include "dataTypes.hpp"
#include "globals.hpp"

listItem::listItem()
    : title(FONT, "N/A", 10),
    description(FONT, "N/A", 10),
    cost(FONT, "0", 10)
{}

Totals::Totals()
        : moneySpent(0),
        moneyGained(0),
        foodEaten(0),
        timesSick(0),
        tick(0),
        time(0)
    {}

Stats::Stats()
    : hunger(0),
    money(0),
    happiness(0),
    hash(0),
    name("!"),
    mood("normal"),
    record("0000000")
{}