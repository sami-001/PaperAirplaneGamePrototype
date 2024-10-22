#include "main.h"

int processEvents(Game *game);
void doRender(Game *game);
int loadGame();
void doPhysics(Game *game);
void quit(Game *game);

int main(int argc, char *argv[]) {
    Game game;
    game.window = NULL;
    game.renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    
    game.window = SDL_CreateWindow("Paper Airplane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (game.window == NULL) {
        printf("Could not create game.window. Error: %s", SDL_GetError()); 
        return -1;
    }

    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (game.renderer == NULL) {
        printf("Could not create renderer. Error: %s", SDL_GetError()); 
        return -1;
    }


    // Move to loadGame()
    game.paper_plane.x = 66;
    game.paper_plane.y = 141;


    int isRunning = 1;
    while (isRunning) {
        isRunning = processEvents(&game);
        doPhysics(&game);
        doRender(&game);   
    }

    quit(&game);
}

void quit(Game *game) {
    SDL_DestroyWindow(game->window);
    SDL_DestroyRenderer(game->renderer);

    SDL_Quit();
    exit(0);
}

void doPhysics(Game *game) {
    
    //AirPlane Movement Calculations
    game->paper_plane.dy = mouse_y - game->paper_plane.y;
    game->paper_plane.dx = mouse_x - game->paper_plane.x;

    if (game->paper_plane.dx != 0) {
        game->paper_plane.x += game->paper_plane.dx / 10;
    }
    else {
        game->paper_plane.dx = 0;
    }

    if (game->paper_plane.dy != 0) {
        game->paper_plane.y += game->paper_plane.dy / 10;
    }
    else {
        game->paper_plane.dy = 0;
    }
}

void doRender(Game *game) {

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 225);
    SDL_RenderClear(game->renderer);

    //Render AirPlane
    SDL_SetRenderDrawColor(game->renderer, 225, 225, 255, 225);
    SDL_RenderDrawPoint(game->renderer, game->paper_plane.x, game->paper_plane.y);

    SDL_RenderPresent(game->renderer);
}

int processEvents(Game *game) {
    int isRunning = 1;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT) {
            isRunning = 0;
        }

        if (e.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            printf("%d, %d\n", mouse_x, mouse_y);
        }

    }
    return isRunning;

}
