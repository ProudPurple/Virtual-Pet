#include "globals.hpp"
#include "dataTypes.hpp"

Totals totals;
Stats stats;
RenderWindow window(VideoMode(Vector2u(300, 275)), "Virtual Pet", Style::None, State::Windowed);
Font FONT("RasterForgeRegular-JpBgm.ttf");
Color DEFAULT_GREEN = Color(34,177,76);
Color LIGHT_GREEN = Color(18, 219, 18);
Color DARK_GREEN = Color(5, 51, 6);
thread backgroundThread;
int frisbeeDelay = 0;
int carWashDelay = 0;
int brushDelay = 0;
int trashDelay = 0;
int dishDelay = 0;
int lastClick = 0;
list<SoundBuffer> buffers;
list<Sound> sounds;
bool sleepy = false;
atomic<bool> running = true;