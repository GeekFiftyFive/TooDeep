#include <stdbool.h>
#include "animation.h"
#include "../IO/logger.h"

struct td_animation {
    int *frames;
    int frameCount;
    int index;
    int fps;
    bool loop;
    bool playing;
    uint lastUpdate;
    td_renderable renderable;
    SDL_Texture *texture;
    SDL_Rect dimensions;
};

td_animation createAnimation(
    td_renderer renderer,
    SDL_Rect dimensions,
    td_tuple worldDimensions,
    SDL_Texture *texture,
    int *frames,
    int frameCount,
    int fps,
    bool loop
) {
    td_animation animation = malloc(sizeof(struct td_animation));

    animation -> renderable = createRenderableFromTexture(renderer, texture);
    animation -> texture = texture;
    animation -> frames = frames;
    animation -> frameCount = frameCount;
    animation -> fps = fps;
    animation -> loop = loop;
    animation -> dimensions = dimensions;

    animation -> playing = true;
    animation -> lastUpdate = SDL_GetTicks();
    animation -> index = 0;
    SDL_Rect textureRegion = getTextureRegion(texture, dimensions, frames[0]);
    setRenderableTextureRegion(animation -> renderable, textureRegion);

    setRenderableSize(animation -> renderable, worldDimensions);

    return animation;
}

void setAnimationPlaying(td_animation animation, bool playing) {
    animation -> playing = playing;
}

td_renderable getRenderableFromAnimation(td_animation animation) {
    return animation -> renderable;
}

void iterateAnimation(td_animation animation) {
    if(
        (animation -> index == animation -> frameCount - 1 && !animation -> loop)
        || !animation -> playing
    ) {
        return;
    }

    int ticks = SDL_GetTicks();
    int delta = ticks - animation -> lastUpdate;

    if(delta < 1000 / animation -> fps) {
        return;
    }

    // 1 frame of animation has passed
    animation -> lastUpdate = ticks;
    animation -> index = (animation -> index + 1) % animation -> frameCount;

    SDL_Rect textureRegion = getTextureRegion(
        animation -> texture,
        animation -> dimensions,
        animation -> frames[animation -> index]
    );

    setRenderableTextureRegion(animation -> renderable, textureRegion);
}

void destroyAnimation(td_animation animation) {
    free(animation -> frames);
    free(animation);
}
