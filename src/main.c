#include "main.h"

void cleanUp(Game *game) {
    //Free Sprites
    freeSprite(&game->paper_plane.sprite);
    freeSprite(&game->dot);

    //Free Renderer
    if (game->renderer != NULL) {
        SDL_DestroyRenderer(game->renderer);
        game->renderer = NULL;
    }
    //Free Window
    if (game->window != NULL) {
        SDL_DestroyWindow(game->window);
        game->window = NULL;
    }
    //Quit
    IMG_Quit();
    SDL_Quit();

    exit(0);
}

int init(Game *game) {

    int init_success;

    game->window = NULL;
    game->renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), game->window);
        init_success = 0;
        return init_success;
    }
    else { 
        game->window = SDL_CreateWindow("Paper Airplane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (game->window == NULL) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), game->window);
            init_success = 0;
            return init_success;
        }
        else {

            game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (game->renderer == NULL) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), game->window);
                init_success = 0;
                return init_success;
            }
            else {
                SDL_RenderSetLogicalSize(game->renderer, LOGICAL_W_WIDTH, LOGICAL_W_HEIGHT);
                //Initialized successfully 
                init_success = 1;
                return init_success;
            }

        }
    }

}


void initStats(Game *game) {
    ////Init Paper AirPlane////
    game->paper_plane.is_picked = 0;

    //Init airplane position
    game->paper_plane.x = 125;
    game->paper_plane.y = 125;

    game->paper_plane.target_position = (SDL_Point) {-1, -1};
    game->paper_plane.redirect_point = (SDL_Point) {-1, -1};

    //Init airplane direction
    game->paper_plane.dir_x = 0;
    game->paper_plane.dir_y = 0;
    game->paper_plane.dir_angle = 0;

    //Init airplane texture
    initSprite(game->renderer, &game->paper_plane.sprite, "res/textures/paper_airplane.png", (SDL_Rect){0, 0, 16, 16});
    initSprite(game->renderer, &game->dot, "res/textures/Sprite-0001.png", (SDL_Rect){0, 0, 16, 16});
    game->dot.rect = (SDL_Rect){100, 123, 16, 16};
}

void doRender(Game *game) {

    SDL_SetRenderDrawColor(game->renderer, 1, 1, 1, 225);
    SDL_RenderClear(game->renderer);

    renderSprite(game->renderer, &game->paper_plane.sprite, 1, 2, 1, game->paper_plane.dir_angle, NULL, SDL_FLIP_NONE);

    //Move Airplane Rect
    game->paper_plane.sprite.rect.x = game->paper_plane.x - (game->paper_plane.sprite.rect.w / 2);
    game->paper_plane.sprite.rect.y = game->paper_plane.y - (game->paper_plane.sprite.rect.h / 2);

    //Render Dot
    if (game->paper_plane.is_picked) {
        renderSprite(game->renderer, &game->dot, 1, 4, 1, 0, NULL, 0);
    }
    else {
        renderSprite(game->renderer, &game->dot, 3, 3, 1, 0, NULL, 0);
    }

    if (game->paper_plane.is_aiming == 1) {
        //Red line from plane to mouse
        SDL_SetRenderDrawColor(game->renderer, 225, 0, 0, 225);
        
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, game->paper_plane.target_position.x, game->paper_plane.target_position.y);
    }
    if (game->paper_plane.is_redirecting == 1 && game->paper_plane.redirect_point.x != -1) {
        //Red line from plane to mouse
        SDL_SetRenderDrawColor(game->renderer, 225, 0, 0, 225);

        for (float i = 0.0; i < 1; i += 0.01) {

            int bx1 = (game->paper_plane.x*(1 - i) + game->paper_plane.redirect_point.x * i)*(1 - i) + (game->paper_plane.redirect_point.x*(1 - i) + game->paper_plane.target_position.x * i) * i;
            int by1 = (game->paper_plane.y*(1 - i) + game->paper_plane.redirect_point.y * i)*(1 - i) + (game->paper_plane.redirect_point.y*(1 - i) + game->paper_plane.target_position.y * i) * i;

            SDL_RenderDrawPoint(game->renderer, bx1, by1);
        }
    }
    SDL_RenderPresent(game->renderer);
}

void doPhysics(Game *game) {
 
    //Get Mouse Position
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x = (mouse.x / (int)scaleX);
    mouse.y = (mouse.y / (int)scaleY);

    //Distance between Plane and Mouse
    float distance_to_mouse_x = (mouse.x - game->paper_plane.x);
    float distance_to_mouse_y = (mouse.y - game->paper_plane.y);


    //Aiming
    if (game->paper_plane.is_aiming == 1) {
        if (!game->paper_plane.is_thrown) {

            game->paper_plane.target_position.x = -distance_to_mouse_x + game->paper_plane.x;
            game->paper_plane.target_position.y = -distance_to_mouse_y + game->paper_plane.y;
        }
    }

    //Redirect point
    if (game->paper_plane.is_redirecting == 1) {
        if (!game->paper_plane.is_thrown) {
            game->paper_plane.redirect_point.x = -distance_to_mouse_x + game->paper_plane.x;
            game->paper_plane.redirect_point.y = -distance_to_mouse_y + game->paper_plane.y;
        }
    }

    //Airplane Movement calculations when thrown
    if (game->paper_plane.is_thrown == 1) {        
        game->paper_plane.is_aiming = 0;
        game->paper_plane.is_redirecting = 0;
        

        float i = game->paper_plane.movement_progress;
        float increment = 0.01;
        if (game->paper_plane.movement_progress < 1.0f) {
            float bx1 = (game->paper_plane.x*(1 - i) + game->paper_plane.redirect_point.x * i)*(1 - i) + (game->paper_plane.redirect_point.x*(1 - i) + game->paper_plane.target_position.x * i) * i;
            float by1 = (game->paper_plane.y*(1 - i) + game->paper_plane.redirect_point.y * i)*(1 - i) + (game->paper_plane.redirect_point.y*(1 - i) + game->paper_plane.target_position.y * i) * i;

            //Direction Angle
            if ( fabsf(bx1 - game->paper_plane.x) > 0.15) {
                game->paper_plane.dir_x = (bx1 - game->paper_plane.x);
            }
            if ( fabsf(by1 - game->paper_plane.y) > 0.15) {
                game->paper_plane.dir_y = (by1 - game->paper_plane.y);
            }
            game->paper_plane.dir_angle = (SDL_atan2((game->paper_plane.dir_y) , (game->paper_plane.dir_x)) * (180.0 / PI));

            game->paper_plane.x += (bx1 - game->paper_plane.x);
            game->paper_plane.y += (by1 - game->paper_plane.y);
            //Move Airplane Rect
            game->paper_plane.sprite.rect.x = game->paper_plane.x - (game->paper_plane.sprite.rect.w / 2);
            game->paper_plane.sprite.rect.y = game->paper_plane.y - (game->paper_plane.sprite.rect.h / 2);
            
            //Direction
            SDL_Log("x : %f, y : %f, r: %f\n", game->paper_plane.dir_x, game->paper_plane.dir_y, game->paper_plane.dir_angle);
            SDL_Log("bx : %f, by : %f, px: %i\n", bx1, by1, game->paper_plane.x);



            game->paper_plane.movement_progress += increment;
        }
        else {

            game->paper_plane.movement_progress = 0;
            game->paper_plane.is_thrown = 0;
            game->paper_plane.is_picked = 0;
        }
    }

}

int processEvents(Game *game) {
    int is_running = 1;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT) {
            is_running = 0;
        }

        if (e.type == SDL_WINDOWEVENT_CLOSE) {
            is_running = 0;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (SDL_PointInRect(&mouse, &game->paper_plane.sprite.rect)) {
                game->paper_plane.is_picked = 1;  
                game->paper_plane.is_aiming = 1;  
            }
            if (game->paper_plane.is_redirecting == 1) {
                game->paper_plane.is_thrown = 1;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (game->paper_plane.is_picked == 1) {
                game->paper_plane.is_aiming = 0;
                game->paper_plane.is_redirecting = 1;
                
            }
        }
    }
    return is_running;
}

int main(int argc, char *argv[]) {
    Game game;

    if (init(&game) == 1) {

        game.lastTick = SDL_GetTicks();
        game.deltaTime = 0;

        initStats(&game);
        while (processEvents(&game) == 1) {
            game.startTick = SDL_GetTicks();
            game.deltaTime = (game.startTick - game.lastTick) / 1000.0;
            game.lastTick = SDL_GetTicks();
            doPhysics(&game);
            doRender(&game);  
        }
        cleanUp(&game);
    }
}
