#ifndef TD_KEYBOARD_EVENTS
#define TD_KEYBOARD_EVENTS

#include <SDL2/SDL.h>

typedef struct td_keymap {
    char *up;
    char *down;
} *td_keymap;

const char *keySymToString(SDL_Keycode);

void destroyKeymap(td_keymap);

#endif