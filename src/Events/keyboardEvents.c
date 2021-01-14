#include "keyboardEvents.h"

#define key(sym, name) case sym:return name

const char *keySymToString(SDL_Keycode sym) {
    switch(sym) {
        key(SDLK_LEFT, "left_arrow");
        key(SDLK_RIGHT, "right_arrow");
        key(SDLK_SPACE, "space_bar");
        default: return "";
    }
}

void destroyKeymap(td_keymap keymap) {
    free(keymap -> up);
    free(keymap -> down);
}
