#include "keyboardEvents.h"

const char *keySymToString(SDL_Keycode sym) {
    switch(sym) {
        case SDLK_LEFT:
            return "left_arrow";
        case SDLK_RIGHT:
            return "right_arrow";
        default:
            return "";
    }
}

void destroyKeymap(td_keymap keymap) {
    free(keymap -> up);
    free(keymap -> down);
}
