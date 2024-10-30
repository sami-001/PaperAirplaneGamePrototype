#ifndef MAIN_H
#define MAIN_H 

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>

#define PI 3.14159265358979323846

const int WINDOW_HEIGHT = 960;
const int WINDOW_WIDTH = 1440;


SDL_Point mouse;

float plane_mouse_distance_x; 
float  plane_mouse_distance_y;

typedef struct {
    int x;
    int y;

    float dx;
    float dy;
    
    float friction;

    //Texture
    SDL_Texture *texture;
    int texture_w;
    int texture_h;
    SDL_Rect rect; 

    //States
    int is_picked;
    int is_thrown;

    //Direction
    double dir_x;
    double dir_y;
    double dir_angle;
    

} PaperAirPlane;


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    PaperAirPlane paper_plane;

} Game;

int processEvents(Game *game);
void doRender(Game *game);
int init(Game *game);
void initStats(Game *game); 
void doPhysics(Game *game);
void cleanUp(Game *game);

#endif
