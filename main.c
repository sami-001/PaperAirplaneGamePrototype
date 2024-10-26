#include "main.h"

int debug_visuals = 0;

int main(int argc, char *argv[]) {
    Game game;

    init(&game);

    int is_running = 1;
    while (is_running) {
        is_running = processEvents(&game);
        doPhysics(&game);
        doRender(&game);   
    }

    quit(&game);
}

void quit(Game *game) {
    SDL_DestroyWindow(game->window);
    SDL_DestroyRenderer(game->renderer);
    game->window = NULL;
    game->renderer = NULL;

    SDL_DestroyTexture(game->paper_plane.texture);
    game->paper_plane.texture = NULL;

    IMG_Quit();
    SDL_Quit();
    exit(0);
}

void init(Game *game) {

    game->window = NULL;
    game->renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    
    //                                   TODO: Enhance error detection                              //

    game->window = SDL_CreateWindow("Paper Airplane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (game->window == NULL) {
        printf("Could not create game.window. Error: %s", SDL_GetError()); 
        exit(-1);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (game->renderer == NULL) {
        printf("Could not create renderer. Error: %s", SDL_GetError()); 
        exit(-1);
    }

    ////Init Paper AirPlane////
    game->paper_plane.is_picked = -1;

    //Init airplane position
    game->paper_plane.x = 66;
    game->paper_plane.y = 141;

    game->paper_plane.dx = 0;
    game->paper_plane.dy = 0;

    //Init airplane Friction
    game->paper_plane.friction = 0.1;

    //Init airplane texture
    game->paper_plane.texture = IMG_LoadTexture(game->renderer, "assets/textures/paper_airplane.png");
    game->paper_plane.texture_w = 50;
    game->paper_plane.texture_h = 50;
}


void doPhysics(Game *game) {
 
    //Moves Airplane
    game->paper_plane.x += game->paper_plane.dx;
    game->paper_plane.y += game->paper_plane.dy;

    //Airplane Collisions with Window edges
    float friction_after_bounce = 0.1;
    if (game->paper_plane.x-game->paper_plane.texture_w/2 < 0) { //Left edge
        game->paper_plane.x = 0 + game->paper_plane.texture_w/2;
        game->paper_plane.dx *= -1;
        game->paper_plane.friction = friction_after_bounce;
        //game->paper_plane.dy *= -1;
    }
    else if (game->paper_plane.x+game->paper_plane.texture_w/2 > WINDOW_WIDTH) { //Right edge
        game->paper_plane.x = WINDOW_WIDTH - game->paper_plane.texture_w/2;
        game->paper_plane.dx *= -1;
        game->paper_plane.friction = friction_after_bounce;
        //game->paper_plane.dy *= -1;
    }
    if (game->paper_plane.y-game->paper_plane.texture_h/2 < 0) { //Top edge
        game->paper_plane.y = 0 + game->paper_plane.texture_h/2;
        game->paper_plane.dy *= -1;
        game->paper_plane.friction = friction_after_bounce;
        //game->paper_plane.dx *= -1;
    }
    else if (game->paper_plane.y+game->paper_plane.texture_h/2 > WINDOW_HEIGHT) { //Bottom edge
        game->paper_plane.y = WINDOW_HEIGHT - game->paper_plane.texture_h/2;
        game->paper_plane.dy *= -1;
        game->paper_plane.friction = friction_after_bounce;
        //game->paper_plane.dx *= -1;
    }
    printf("%d, %d\n", game->paper_plane.x, game->paper_plane.y);

    //Friction
    if (game->paper_plane.dx > 1 || game->paper_plane.dx < -1) {
        game->paper_plane.dx *= (1.0 - game->paper_plane.friction);
    }
    else if (game->paper_plane.dx < 1 && game->paper_plane.dx > -1) {
        game->paper_plane.dx = 0;
    }

    if (game->paper_plane.dy > 1 || game->paper_plane.dy < -1) {
        game->paper_plane.dy *= (1.0 - game->paper_plane.friction);
    }
    else if (game->paper_plane.dy < 1 && game->paper_plane.dy > -1) {
        game->paper_plane.dy = 0;
    }

    //Distance between Plane and Mouse
    plane_mouse_distance_x = (mouse.x - game->paper_plane.x);
    plane_mouse_distance_y = (mouse.y - game->paper_plane.y);

    //AirPlane Movement Calculations when picked
    if (game->paper_plane.is_picked == 1) {

        game->paper_plane.dx = plane_mouse_distance_x / 2;
        game->paper_plane.dy = plane_mouse_distance_y / 2;
    }

    //Airplane Movement calculations when thrown
    if (game->paper_plane.is_thrown == 1) {        

        game->paper_plane.dx = plane_mouse_distance_x / 1;
        game->paper_plane.dy = plane_mouse_distance_y / 1;

        game->paper_plane.is_thrown = -1;
    }

}

void doRender(Game *game) {

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 225);
    SDL_RenderClear(game->renderer);

    //Render AirPlane
    SDL_Rect rect = {game->paper_plane.x - (game->paper_plane.texture_w / 2), game->paper_plane.y - (game->paper_plane.texture_h / 2), game->paper_plane.texture_w, game->paper_plane.texture_h};
    game->paper_plane.rect = rect;
    SDL_RenderCopy(game->renderer, game->paper_plane.texture, NULL, &game->paper_plane.rect);

    //AirPlane & Mouse Distance Line
    if (debug_visuals == 1) {
        SDL_SetRenderDrawColor(game->renderer, 225, 0, 0, 225);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, mouse.x, mouse.y);
    }

    SDL_RenderPresent(game->renderer);
}

int processEvents(Game *game) {
    int is_running = 1;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT) {
            is_running = 0;
        }

        if (e.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouse.x, &mouse.y);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (SDL_PointInRect(&mouse, &game->paper_plane.rect)) {
                game->paper_plane.is_picked = 1;  
                game->paper_plane.friction = 0.1;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (game->paper_plane.is_picked == 1) {

                if (fabsf(plane_mouse_distance_x) >= 15 && fabsf(plane_mouse_distance_y) >= 15) {
                    game->paper_plane.is_thrown = 1;
                }
                game->paper_plane.is_picked = -1;
                
            }
        }
    }
    return is_running;
}
