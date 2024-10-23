#ifndef MAIN_H
#define MAIN_H 

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>

typedef struct {
    int x;
    int y;

    int dx;
    int dy;

    SDL_Texture *textrue;
} PaperAirPlane;


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    PaperAirPlane paper_plane;
} Game;

int mouse_x;
int mouse_y;


#endif
