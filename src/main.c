#include "main.h"

int debug_mode = 0;

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
    game->paper_plane.x = 100;
    game->paper_plane.y = 100;

    game->paper_plane.dx = 0;
    game->paper_plane.dy = 0;

    //Init airplane direction
    game->paper_plane.dir_x = 0;
    game->paper_plane.dir_y = 0;
    game->paper_plane.dir_angle = 0;

    //Init airplane Friction
    game->paper_plane.friction = 0.07;
    game->paper_plane.friction_after_bounce = 0.2;
    game->paper_plane.thrown_drag = 5.5;
    game->paper_plane.min_delta_pos = 1.0; //If delta position is less that this, set it to 0. If not apply friction

    //Init airplane texture
    initSprite(game->renderer, &game->paper_plane.sprite, "res/textures/paper_airplane.png", (SDL_Rect){0, 0, 16, 16});
    initSprite(game->renderer, &game->dot, "res/textures/Sprite-0001.png", (SDL_Rect){0, 0, 16, 16});
    game->dot.rect = (SDL_Rect){100, 123, 16, 16};
}



void doRender(Game *game) {

    SDL_SetRenderDrawColor(game->renderer, 1, 1, 1, 225);
    SDL_RenderClear(game->renderer);

    renderSprite(game->renderer, &game->paper_plane.sprite, 1, 2, 1, game->paper_plane.dir_angle, NULL, SDL_FLIP_NONE);

    //Render Dot
    if (game->paper_plane.is_picked) {
        renderSprite(game->renderer, &game->dot, 1, 4, 1, 0, NULL, 0);
    }
    else {
        renderSprite(game->renderer, &game->dot, 3, 3, 1, 0, NULL, 0);
    }

    if (game->paper_plane.is_picked == 1) {
        //Red line from plane to mouse
        SDL_SetRenderDrawColor(game->renderer, 225, 0, 0, 225);
        //SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, mouse.x, mouse.y);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, game->paper_plane.x - plane_mouse_distance_x , game->paper_plane.y - plane_mouse_distance_y);
    }

    //Visual Debug
    if (debug_mode == 1) {

        int length_mult = 3;
        //Yellow line from plane to (dx, dy)
        SDL_SetRenderDrawColor(game->renderer, 0, 225, 225, 225);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, (game->paper_plane.x+game->paper_plane.dx * length_mult), (game->paper_plane.y+game->paper_plane.dy * length_mult));

        //Green line from plane to mouse x
        SDL_SetRenderDrawColor(game->renderer, 0, 225, 0, 225);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, (game->paper_plane.x+game->paper_plane.dx * length_mult), game->paper_plane.y);

        //Blue line from plane to mouse y
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 225, 225);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, game->paper_plane.x, (game->paper_plane.y+game->paper_plane.dy * length_mult));
    }
    SDL_RenderPresent(game->renderer);
}

void doPhysics(Game *game) {
 
    //Get Mouse Position
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x = (mouse.x / (int)scaleX);
    mouse.y = (mouse.y / (int)scaleY);

    //Distance between Plane and Mouse
    plane_mouse_distance_x = (mouse.x - game->paper_plane.x);
    plane_mouse_distance_y = (mouse.y - game->paper_plane.y);

    //Collision with Edges
    if (SDL_PointInRect(&(SDL_Point){game->paper_plane.x, game->paper_plane.y}, &windowRect) == 1) {
        game->paper_plane.savedX = game->paper_plane.x;
        game->paper_plane.savedY = game->paper_plane.y;
    }
    else {
        game->paper_plane.x = game->paper_plane.savedX;
        game->paper_plane.dx = 0;
        game->paper_plane.y = game->paper_plane.savedY;
        game->paper_plane.dy = 0;
    }

    //Moves Airplane
    game->paper_plane.x += game->paper_plane.dx;
    game->paper_plane.y += game->paper_plane.dy;
    //Move Airplane Rect
    game->paper_plane.sprite.rect.x = game->paper_plane.x - (game->paper_plane.sprite.rect.w / 2);
    game->paper_plane.sprite.rect.y = game->paper_plane.y - (game->paper_plane.sprite.rect.h / 2);

    //Airplane Direction calculation
      //Direction Vectors
    if ( fabs(game->paper_plane.dx) > 0.3) {
        game->paper_plane.dir_x = (game->paper_plane.x + game->paper_plane.dx) - game->paper_plane.x;
    }
    if ( fabs(game->paper_plane.dy) > 0.3) {
        game->paper_plane.dir_y = (game->paper_plane.y + game->paper_plane.dy) - game->paper_plane.y;
    }
      //Direction Angle
    game->paper_plane.dir_angle = (atan2(game->paper_plane.dir_y, game->paper_plane.dir_x) * (180.0 / PI));
   
    //Friction
    if (fabs(game->paper_plane.dx) >= game->paper_plane.min_delta_pos) {
        game->paper_plane.dx *= (1.0 - game->paper_plane.friction);
    }
    if (fabs(game->paper_plane.dx) < game->paper_plane.min_delta_pos) {
        game->paper_plane.dx = 0;
        if (fabs(game->paper_plane.dy) < game->paper_plane.min_delta_pos * 2) {
            game->paper_plane.dy = 0;
        }
    }

    if (fabs(game->paper_plane.dy) >= game->paper_plane.min_delta_pos) {
        game->paper_plane.dy *= (1.0 - game->paper_plane.friction);
    }
    if (fabs(game->paper_plane.dy) < game->paper_plane.min_delta_pos) {
        game->paper_plane.dy = 0;
        if (fabs(game->paper_plane.dx) < game->paper_plane.min_delta_pos * 2) {
            game->paper_plane.dx = 0;
        }
    }

    //Airplane Movement calculations when thrown
    if (game->paper_plane.is_thrown == 1) {        

        game->paper_plane.dx = -plane_mouse_distance_x / game->paper_plane.thrown_drag;
        game->paper_plane.dy = -plane_mouse_distance_y / game->paper_plane.thrown_drag;;

        game->paper_plane.is_thrown = 0;
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
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (game->paper_plane.is_picked == 1) {

                game->paper_plane.is_thrown = 1;
                game->paper_plane.is_picked = 0;
                
            }
        }
    }
    return is_running;
}

int main(int argc, char *argv[]) {
    Game game;

    if (init(&game) == 1) {

        initStats(&game);
        while (processEvents(&game) == 1) {
            doPhysics(&game);
            doRender(&game);   
        }
        cleanUp(&game);
    }
}
