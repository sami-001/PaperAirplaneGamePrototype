#include "main.h"

int processEvents(SDL_Window *window);
void doRender(SDL_Renderer *renderer);
int doPhysics();
void quit(Game game);

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

    int running = 1;
    while (running) {
        running = processEvents(game.window);
        doRender(game.renderer);   
    }

    quit(game);
}

void quit(Game game) {
    SDL_DestroyWindow(game.window);
    SDL_DestroyRenderer(game.renderer);

    SDL_Quit();
    exit(0);
}

void doRender(SDL_Renderer *renderer) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 225);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

int processEvents(SDL_Window *window) {
    int running = 1;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) 
        case SDL_QUIT:
            running = 0;
            break;
    }
    return running;

}
