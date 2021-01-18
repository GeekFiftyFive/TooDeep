#ifndef TD_ANIMATION
#define ANIMATION

#include "renderer.h"
#include "../IO/gameLoader.h"
#include "../JSON/jsonParser.h"

typedef struct td_animation *td_animation;

td_animation createAnimation(td_renderer, SDL_Rect, td_tuple, SDL_Texture *, int *, int, int, bool);

void setAnimationPlaying(td_animation, bool);

td_renderable getRenderableFromAnimation(td_animation);

void iterateAnimation(td_animation);

td_animation createAnimationFromJson(td_json, td_game);

void destroyAnimation(td_animation);

#endif