#include "globals.hpp"
#include "dataTypes.hpp"

Totals totals;
Stats stats;
Font FONT("RasterForgeRegular-JpBgm.ttf");
Color DEFAULT_GREEN = Color(34,177,76);
Color LIGHT_GREEN = Color(18, 219, 18);
Color DARK_GREEN = Color(5, 51, 6);
thread backgroundThread;
int frisbeeDelay = 0;
atomic<bool> running = true;