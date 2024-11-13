#ifndef SPRITE_H
#define SPRITE_H


#include <stdio.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>

#define FPS 60
#define FrameTime 4000 / FPS

typedef struct {
    SDL_Texture *spritesheet;
    SDL_Rect rect; 
    int currentSrcRect;
    int lastFrameTick;
    int loaded;
} Sprite;

void freeSprite(Sprite *sprite);
int initSprite(SDL_Renderer *renderer, Sprite *sprite, char *texture_path, SDL_Rect rect);
SDL_Rect *initSrcRect(Sprite *sprite, int xFrames, int rowFrame);
void renderSprite(SDL_Renderer *renderer, Sprite *sprite, int startFrame, int xFrames, int rowFrames, double angle, SDL_Point *centre, SDL_RendererFlip flip_flag);


#endif
