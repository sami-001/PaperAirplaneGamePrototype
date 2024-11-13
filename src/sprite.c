#include "sprite.h"

void freeSprite(Sprite *sprite) {
    if (sprite->spritesheet != NULL) {
        SDL_DestroyTexture(sprite->spritesheet);
        sprite->spritesheet = NULL;
    }
}

int initSprite(SDL_Renderer *renderer, Sprite *sprite, char *texture_path, SDL_Rect rect) {

    sprite->currentSrcRect = 0;
    sprite->lastFrameTick = 0;
    char buffer[100];
    SDL_snprintf(buffer, sizeof(buffer), "%s", texture_path);
    sprite->spritesheet = IMG_LoadTexture(renderer, buffer);
    if (sprite->spritesheet != NULL) {
        sprite->rect = rect;
        sprite->loaded = 1;
        return sprite->loaded; //Returns 1 on success
    }
    else {
        sprite->loaded = 0;
        return sprite->loaded; //Returns 0 on failure
    }
}

SDL_Rect *initSrcRect(Sprite *sprite, int xFrames, int rowFrame) {

    SDL_Rect *frames = malloc(xFrames * sizeof(SDL_Rect));

    for (int i = 0; i < xFrames; i++) {
        frames[i].x = sprite->rect.w * (i);
        frames[i].y = sprite->rect.h * (rowFrame - 1);
        frames[i].w = sprite->rect.w;
        frames[i].h = sprite->rect.h;
    } 
    return frames;
}

void renderSprite(SDL_Renderer *renderer, Sprite *sprite, int startFrame, int xFrames, int rowFrames, double angle, SDL_Point *centre, SDL_RendererFlip flip_flag) {
    SDL_Rect *srcRect = initSrcRect(sprite, xFrames, rowFrames);
    Uint32 startTick = SDL_GetTicks();
        if (startTick - sprite->lastFrameTick >= FrameTime) {
            sprite->currentSrcRect = (sprite->currentSrcRect + 1) % xFrames;
            if (sprite->currentSrcRect == 0) {
                sprite->currentSrcRect = startFrame - 1;
            }

            sprite->lastFrameTick = startTick;
        }
        SDL_RenderCopyEx(renderer, sprite->spritesheet, &srcRect[sprite->currentSrcRect], &sprite->rect, angle, centre, flip_flag); 
}
