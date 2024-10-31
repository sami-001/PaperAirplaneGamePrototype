#include "main.h"

int debug_mode = 0;

int main(int argc, char *argv[]) {
    Game game;

    if (init(&game) == 1) {

        initStats(&game);
        while (processEvents(&game) == 1) {
            doRender(&game);   
            doPhysics(&game);
            }
        cleanUp(&game);
    }
}

void cleanUp(Game *game) {
    int debug_cleanUp = 0;
    //Free Textures
    if (game->paper_plane.texture != NULL) {
        SDL_DestroyTexture(game->paper_plane.texture);
        game->paper_plane.texture = NULL;
        if (debug_cleanUp == 1) {
            SDL_Log("paper_plane texture freed");
        }
    }
    //Free Renderer
    if (game->renderer != NULL) {
        SDL_DestroyRenderer(game->renderer);
        game->renderer = NULL;
        if (debug_cleanUp == 1) {
            SDL_Log("renderer freed");
        }
    }
    //Free Window
    if (game->window != NULL) {
        SDL_DestroyWindow(game->window);
        game->window = NULL;
        if (debug_cleanUp == 1) {
            SDL_Log("window freed");
        }
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
        SDL_Log("could not initialize SDL. Error : %s", SDL_GetError());
        init_success = 0;
        return init_success;
    }
    else { 
        game->window = SDL_CreateWindow("Paper Airplane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (game->window == NULL) {
            SDL_Log("Could not create game.window. Error: %s", SDL_GetError()); 
            init_success = 0;
            return init_success;
        }
        else {

            game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (game->renderer == NULL) {
                SDL_Log("Could not create renderer. Error: %s", SDL_GetError()); 
                init_success = 0;
                return init_success;
            }
            else {
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
    game->paper_plane.x = 66;
    game->paper_plane.y = 141;

    game->paper_plane.dx = 0;
    game->paper_plane.dy = 0;

    //Init airplane direction
    game->paper_plane.dir_x = 0;
    game->paper_plane.dir_y = 0;
    game->paper_plane.dir_angle = 0;

    //Init airplane Friction
    game->paper_plane.friction = 0.1;

    //Init airplane texture
    
    game->paper_plane.texture = IMG_LoadTexture(game->renderer, "../res/textures/paper_airplane.png");
    game->paper_plane.shadow_texture = IMG_LoadTexture(game->renderer, "../res/textures/paper_airplane_shadow.png");
    SDL_QueryTexture(game->paper_plane.texture, NULL, NULL, &game->paper_plane.texture_w, &game->paper_plane.texture_h);

 
      //Init Frames
    if (game->paper_plane.texture != NULL) { 
        game->paper_plane.n_frames = 2;

        game->paper_plane.frames = malloc( game->paper_plane.n_frames * sizeof(SDL_Rect));
        for (int i = 0; i < game->paper_plane.n_frames; i++) {
            game->paper_plane.frames[i].x = game->paper_plane.texture_w / game->paper_plane.n_frames * i;
            game->paper_plane.frames[i].y = 0; 
            game->paper_plane.frames[i].w = game->paper_plane.texture_w / game->paper_plane.n_frames;
            game->paper_plane.frames[i].h = game->paper_plane.texture_h;
        } 
    }
    else {
        SDL_Log("could not load texture. ERROR: %s", SDL_GetError());
        cleanUp(game);
    }
}


void doRender(Game *game) {

    SDL_SetRenderDrawColor(game->renderer, 101, 101, 101, 225);
    SDL_RenderClear(game->renderer);
//                                       FIXME: NEEDS RESTUCTURING
    //Render AirPlane
      //Main Destination Rect
    game->paper_plane.rect.x = game->paper_plane.x - (game->paper_plane.texture_w / game->paper_plane.n_frames / 2);
    game->paper_plane.rect.y = game->paper_plane.y - (game->paper_plane.texture_h / 2);
    game->paper_plane.rect.w = game->paper_plane.texture_w / game->paper_plane.n_frames;
    game->paper_plane.rect.h = game->paper_plane.texture_h;

    if (fabsf(game->paper_plane.dx) < 4 && fabsf(game->paper_plane.dy) < 4) {
        game->paper_plane.current_frame = game->paper_plane.frames[0];

        game->paper_plane.shadow_rect.x = game->paper_plane.x - (game->paper_plane.texture_w / game->paper_plane.n_frames / 2) + 10;
        game->paper_plane.shadow_rect.y = game->paper_plane.y - (game->paper_plane.texture_h / 2) + 10;
        game->paper_plane.shadow_rect.w = game->paper_plane.texture_w / game->paper_plane.n_frames;
        game->paper_plane.shadow_rect.h = game->paper_plane.texture_h;
    }
    else {
        game->paper_plane.current_frame = game->paper_plane.frames[1];

        game->paper_plane.shadow_rect.x = game->paper_plane.x - (game->paper_plane.texture_w / game->paper_plane.n_frames / 2) + 30;
        game->paper_plane.shadow_rect.y = game->paper_plane.y - (game->paper_plane.texture_h / 2) + 30;
        game->paper_plane.shadow_rect.w = game->paper_plane.texture_w / game->paper_plane.n_frames;
        game->paper_plane.shadow_rect.h = game->paper_plane.texture_h;
    }
    //Plane Shadow Texture
    SDL_RenderCopyEx(game->renderer, game->paper_plane.shadow_texture, NULL, &game->paper_plane.shadow_rect, game->paper_plane.dir_angle, NULL, SDL_FLIP_NONE); 
    //Plane Texture
    SDL_RenderCopyEx(game->renderer, game->paper_plane.texture, &game->paper_plane.current_frame, &game->paper_plane.rect, game->paper_plane.dir_angle, NULL, SDL_FLIP_NONE); 
    SDL_ScaleMode(NEAR);

    //Visual Debug
    if (debug_mode == 1) {
        //Red line from plane to mouse
        SDL_SetRenderDrawColor(game->renderer, 225, 0, 0, 225);
        SDL_RenderDrawLine(game->renderer, game->paper_plane.x, game->paper_plane.y, mouse.x, mouse.y);

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

    //Moves Airplane
    game->paper_plane.x += game->paper_plane.dx;
    game->paper_plane.y += game->paper_plane.dy;

    //Airplane Direction calculation
      //Direction Vectors
    if ( fabs(game->paper_plane.dx) > 1) {
        game->paper_plane.dir_x = (game->paper_plane.x + game->paper_plane.dx) - game->paper_plane.x;
        //SDL_Log("%f", game->paper_plane.dx);
    }
    if ( fabs(game->paper_plane.dy) > 1) {
        game->paper_plane.dir_y = (game->paper_plane.y + game->paper_plane.dy) - game->paper_plane.y;
        //SDL_Log("%f", game->paper_plane.dy);
    }
    game->paper_plane.dir_angle = (atan2(game->paper_plane.dir_y, game->paper_plane.dir_x) * (180.0 / PI));
    //SDL_Log("%f", game->paper_plane.dir_angle);
    

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
                game->paper_plane.is_picked = 0;
                
            }
        }
    }
    return is_running;
}
