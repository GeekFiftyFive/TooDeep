#ifndef TD_ANIMATION
#define TD_ANIMATION

#include "renderer.h"
#include "../JSON/jsonParser.h"
#include "../IO/resourceLoader.h"
#include "../Renderer/renderer.h"

typedef struct td_animation *td_animation;

td_animation createAnimation(td_renderer, SDL_Rect, td_tuple, SDL_Texture *, int *, int, int, bool);

void setAnimationPlaying(td_animation, bool);

td_renderable getRenderableFromAnimation(td_animation);

void iterateAnimation(td_animation);

td_animation createAnimationFromJson(td_json, td_resourceLoader, td_renderer);

void destroyAnimation(td_animation);

#endif