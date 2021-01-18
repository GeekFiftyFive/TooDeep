#include <stdbool.h>
#include "animation.h"
#include "../IO/logger.h"

#define ASSET_DIR "/assets/"

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

struct addFrameCallbackData {
    int index;
    int *frames;
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

static void addFrameCallback(td_json json, void *callbackData) {
    struct addFrameCallbackData *dataCast = (struct addFrameCallbackData*) callbackData;
    int index = dataCast -> index;
    int frameNumber = getJSONInt(json, NULL, NULL);
    dataCast -> frames[index] = frameNumber;
    dataCast -> index++;
}

td_animation createAnimationFromJson(td_json json, td_game game) {
    char *source = getJSONString(json, "source", NULL);
    int width = getJSONInt(json, "tile_dimensions.w", NULL);
    int height = getJSONInt(json, "tile_dimensions.h", NULL);
    float worldWidth = getJSONDouble(json, "world_dimensions.w", NULL);
    float worldHeight = getJSONDouble(json, "world_dimensions.h", NULL);
    int frameCount = getJSONArrayLength(json, "frames");
    int *frames = malloc(sizeof(int) * frameCount);
    bool loop = getJSONBool(json, "loop", NULL);
    int frameRate = getJSONInt(json, "frame_rate", NULL);
    char *fullAssetName = malloc(strlen(source) + strlen(ASSET_DIR) + 1);
    sprintf(fullAssetName, "%s%s", ASSET_DIR, source);
    // Load asset using asset name and create renderable
    SDL_Surface *surface =  loadSurfaceResource(getResourceLoader(game), fullAssetName);
    SDL_Texture *texture = surfaceToTexture(getRenderer(game), surface);
    struct addFrameCallbackData callbackData = { 0, frames };
    jsonArrayForEach(json, "frames", addFrameCallback, &callbackData);
    td_animation animation = createAnimation(
        getRenderer(game),
        (SDL_Rect) { 0, 0, width, height },
        (td_tuple) { worldWidth, worldHeight },
        texture,
        frames,
        frameCount,
        frameRate,
        loop
    );

    return animation;
}

void destroyAnimation(td_animation animation) {
    free(animation -> frames);
    free(animation);
}
