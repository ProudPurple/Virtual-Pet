#pragma once
#include "dataTypes.hpp"
#include <thread>

extern Totals totals;
extern Stats stats;
extern Font FONT;
extern Color DEFAULT_GREEN;
extern Color LIGHT_GREEN;
extern Color DARK_GREEN;
extern thread backgroundThread;
extern int frisbeeDelay;
extern int trashDelay;
extern int carWashDelay;
extern int dishDelay;
extern int lastClick;
extern bool sleepy;
extern atomic<bool> running;