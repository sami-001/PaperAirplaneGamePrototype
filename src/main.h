#ifndef MAIN_H
#define MAIN_H 

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>

#include "sprite.h"

#define PI 3.14159265358979323846

const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 720;

const int LOGICAL_W_WIDTH = 320;
const int LOGICAL_W_HEIGHT = 240;

SDL_Rect windowRect = {0+10, 0+10, LOGICAL_W_WIDTH-20, LOGICAL_W_HEIGHT-20};

float scaleX = (float)WINDOW_WIDTH / (float)LOGICAL_W_WIDTH;
float scaleY = (float)WINDOW_HEIGHT / (float)LOGICAL_W_HEIGHT;

SDL_Point mouse;

float plane_mouse_distance_x; 
float  plane_mouse_distance_y;

typedef struct {
    int x;
    int y;

    SDL_Point target_position;
    SDL_Point redirect_point;

    float movement_progress;

    //Saves position of last frame
    int savedX;
    int savedY;

    float dx;
    float dy;

    float speed;
    
    float friction;
    float thrown_drag;
    float min_delta_pos; //If delta position is less that this, set it to 0. If not apply friction
    float friction_after_bounce;

    //Texture
    Sprite sprite;

    //States
    int is_picked;
    int is_aiming;
    int is_redirecting;
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
    Uint32 lastTick;
    float deltaTime;

    PaperAirPlane paper_plane;
    
    Sprite dot;

} Game;

#endif
