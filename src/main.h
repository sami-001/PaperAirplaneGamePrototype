#ifndef MAIN_H
#define MAIN_H 

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>

#define PI 3.14159265358979323846

#define FPS 60
#define FrameTime 4000 / FPS

const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 720;

const int LOGICAL_W_WIDTH = 320;
const int LOGICAL_W_HEIGHT = 240;

float scaleX = (float)WINDOW_WIDTH / (float)LOGICAL_W_WIDTH;
float scaleY = (float)WINDOW_HEIGHT / (float)LOGICAL_W_HEIGHT;

SDL_Point mouse;

float plane_mouse_distance_x; 
float  plane_mouse_distance_y;

typedef struct {
    SDL_Texture *spritesheet;
    SDL_Rect rect; 
    int currentSrcRect;
    int lastFrameTick;
    int loaded;
} Sprite;

typedef struct {
    int x;
    int y;

    float dx;
    float dy;
    
    float friction;
    float picked_drag; //Divides distance to mouse which will equal delta position
    float thrown_drag;
    float throw_threshold; //Minimum distance from mouse while picked to throw when released  
    float min_delta_pos; //If delta position is less that this, set it to 0. If not apply friction
    float friction_after_bounce;

    //Texture
    Sprite sprite;
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

    Uint32 startTick;

    PaperAirPlane paper_plane;
    
    Sprite dot;

} Game;

#endif
