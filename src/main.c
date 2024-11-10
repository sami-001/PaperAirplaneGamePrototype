#include "main.h"

int debug_mode = 0;

void freeSprite(Sprite *sprite) {
    if (sprite->spritesheet != NULL) {
        SDL_DestroyTexture(sprite->spritesheet);
        sprite->spritesheet = NULL;
    }
}

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

void initSprite(Game *game, Sprite *sprite, char *texture_path, SDL_Rect rect) {

    sprite->loaded = 0;
    sprite->currentSrcRect = 0;
    sprite->lastFrameTick = 0;
    char buffer[100];
    SDL_snprintf(buffer, sizeof(buffer), "%s", texture_path);
    sprite->spritesheet = IMG_LoadTexture(game->renderer, buffer);
    if (sprite->spritesheet != NULL) {
        sprite->loaded = 1;
    }
    else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), game->window);
        cleanUp(game);
    }
    sprite->rect = rect;
}

void initStats(Game *game) {
    ////Init Paper AirPlane////
    game->paper_plane.is_picked = 0;

    //Init airplane position
    game->paper_plane.x = 23;
    game->paper_plane.y = 23;

    game->paper_plane.dx = 0;
    game->paper_plane.dy = 0;

    //Init airplane direction
    game->paper_plane.dir_x = 0;
    game->paper_plane.dir_y = 0;
    game->paper_plane.dir_angle = 0;

    //Init airplane Friction
    game->paper_plane.friction = 0.1;
    game->paper_plane.friction_after_bounce = 0.2;
    game->paper_plane.picked_drag = 2.0; //Divides distance to mouse which will equal delta position
    game->paper_plane.thrown_drag = 1.0;
    game->paper_plane.throw_threshold = 15.0; //Minimum distance from mouse while picked to throw when released  
    game->paper_plane.min_delta_pos = 2.0; //If delta position is less that this, set it to 0. If not apply friction

    //Init airplane texture
    initSprite(game, &game->paper_plane.sprite, "../res/textures/paper_airplane.png", (SDL_Rect){0, 0, 16, 16});
    initSprite(game, &game->dot, "../res/textures/Sprite-0001.png", (SDL_Rect){0, 0, 16, 16});
    game->dot.rect = (SDL_Rect){100, 123, 16, 16};
}

SDL_Rect *initSrcRect(Game *game, Sprite *sprite, int xFrames, int rowFrame) {

    SDL_Rect *frames = malloc(xFrames * sizeof(SDL_Rect));
    if (sprite->loaded != 0) { 

        for (int i = 0; i < xFrames; i++) {
            frames[i].x = sprite->rect.w * (i);
            frames[i].y = sprite->rect.h * (rowFrame - 1);
            frames[i].w = sprite->rect.w;
            frames[i].h = sprite->rect.h;
            SDL_Log("%d", i);
        } 
    }
    else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", SDL_GetError(), game->window);
        cleanUp(game);
    }
    return frames;
}

void renderSprite(Game *game, Sprite *sprite, int startFrame, int xFrames, int rowFrames, double angle, SDL_Point *centre, SDL_RendererFlip flip_flag) {
    SDL_Rect *srcRect = initSrcRect(game, sprite, xFrames, rowFrames);
    Uint32 startTick = SDL_GetTicks();
        if (startTick - sprite->lastFrameTick >= FrameTime) {
            sprite->currentSrcRect = (sprite->currentSrcRect + 1) % xFrames;
            if (sprite->currentSrcRect == 0) {
                sprite->currentSrcRect = startFrame - 1;
            }

            sprite->lastFrameTick = startTick;
        }
        SDL_RenderCopyEx(game->renderer, sprite->spritesheet, &srcRect[sprite->currentSrcRect], &sprite->rect, angle, centre, flip_flag); 
}

void doRender(Game *game) {

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetRenderDrawColor(game->renderer, 1, 1, 1, 225);
    SDL_RenderClear(game->renderer);

    renderSprite(game, &game->paper_plane.sprite, 1, 2, 1, game->paper_plane.dir_angle, NULL, SDL_FLIP_NONE);

    //Render Dot
    if (game->paper_plane.is_picked) {
        renderSprite(game, &game->dot, 1, 4, 1, 0, NULL, 0);
    }
    else {
        renderSprite(game, &game->dot, 3, 3, 1, 0, NULL, 0);
    }

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
    mouse.x = (mouse.x / (int)scaleX);
    mouse.y = (mouse.y / (int)scaleY);

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
    game->paper_plane.dir_angle = (atan2(game->paper_plane.dir_y, game->paper_plane.dir_x) * (180.0 / PI));
    

    //Airplane Collisions with Window edges
    if (game->paper_plane.x-game->paper_plane.sprite.rect.w/2 < 0) { //Left edge
        game->paper_plane.x = 0 + game->paper_plane.sprite.rect.w/2;
        game->paper_plane.dx *= -1;
        game->paper_plane.friction = game->paper_plane.friction_after_bounce;
    }
    else if (game->paper_plane.x+game->paper_plane.sprite.rect.w/2 > LOGICAL_W_WIDTH) { //Right edge
        game->paper_plane.x = LOGICAL_W_WIDTH - game->paper_plane.sprite.rect.w/2;
        game->paper_plane.dx *= -1;
        game->paper_plane.friction = game->paper_plane.friction_after_bounce;
    }
    if (game->paper_plane.y-game->paper_plane.sprite.rect.h/2 < 0) { //Top edge
        game->paper_plane.y = 0 + game->paper_plane.sprite.rect.h/2;
        game->paper_plane.dy *= -1;
        game->paper_plane.friction = game->paper_plane.friction_after_bounce;
    }
    else if (game->paper_plane.y+game->paper_plane.sprite.rect.h/2 > LOGICAL_W_HEIGHT) { //Bottom edge
        game->paper_plane.y = LOGICAL_W_HEIGHT - game->paper_plane.sprite.rect.h/2;
        game->paper_plane.dy *= -1;
        game->paper_plane.friction = game->paper_plane.friction_after_bounce;
    
    }

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

    //Distance between Plane and Mouse
    plane_mouse_distance_x = (mouse.x - game->paper_plane.x);
    plane_mouse_distance_y = (mouse.y - game->paper_plane.y);

    //AirPlane Movement Calculations when picked
    if (game->paper_plane.is_picked == 1) {

        game->paper_plane.dx = plane_mouse_distance_x / game->paper_plane.picked_drag;
        game->paper_plane.dy = plane_mouse_distance_y / game->paper_plane.picked_drag;

    }

    //Airplane Movement calculations when thrown
    if (game->paper_plane.is_thrown == 1) {        

        game->paper_plane.dx = plane_mouse_distance_x / game->paper_plane.thrown_drag;
        game->paper_plane.dy = plane_mouse_distance_y / game->paper_plane.thrown_drag;;

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
                game->paper_plane.friction = 0.1;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            if (game->paper_plane.is_picked == 1) {

                if (fabsf(plane_mouse_distance_x) >= game->paper_plane.throw_threshold || fabsf(plane_mouse_distance_y) >= game->paper_plane.throw_threshold) {
                    game->paper_plane.is_thrown = 1;
                }
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
