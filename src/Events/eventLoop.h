#ifndef EVENT_LOOP
#define EVENT_LOOP

#include "../Renderer/renderer.h"
#include "../IO/gameLoader.h"

void startEventLoop(td_renderer, td_game);

void quit();

#endif