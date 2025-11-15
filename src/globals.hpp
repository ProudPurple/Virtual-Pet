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
extern atomic<bool> running;