#ifndef MAIN_H
#define MAIN_H 

#include <stdio.h>
#include <SDL2/SDL.h> 

typedef struct {
    int x;
    int y;

    int dx;
    int dy;
} PaperAirPlane;


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    PaperAirPlane paper_plane;
} Game;

int mouse_x;
int mouse_y;


#endif
