#include <stdio.h> 
#include <SDL2/SDL.h>


int main(int argc, char *argv[]) {

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    
    window = SDL_CreateWindow("Paper Airplane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (window == NULL) {
        printf("Could not create window. Error: %s", SDL_GetError()); 
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Could not create renderer. Error: %s", SDL_GetError()); 
        return -1;
    }
    
    int running = 1;

    SDL_Event e;
    while (running) {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 225);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            switch (e.type) 
                case SDL_QUIT:
                    running = 0;
                break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    return 0;
}
