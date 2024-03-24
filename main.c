//
//  main.c
//  Pong
//
//  Created by nir shtam on 18/03/2024.
//

#include <SDL2/SDL.h>
#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

typedef struct game_object{
    int x;
    int y;
    int w;
    int h;
    int velocity_x;
    int velocity_y;
    int isBall;
} object;


// *GLOBAL VARIABLES* //
SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;
int game_is_running = 0;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS; // Milliseconds per frame
object objects_to_render[2];
//-----------------------------


int initialize_window(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            fprintf(stderr, "Error creating SDL Renderer.\n");
            return 0;
        }
    return 1;
}

struct game_object initialize_ball(void) {
    struct game_object ball;
    ball.h = BALL_HEIGHT;
    ball.w = BALL_WIDTH;
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.velocity_x = BALL_X_VELOCITY;
    ball.velocity_y = BALL_Y_VELOCITY;
    ball.isBall = 1;
    return ball;
}

struct game_object initialize_player(void) {
    struct game_object player;
    player.h = PADDLE_HEIGHT;
    player.w = PADDLE_WIDTH;
    player.x = PADDLE_INITIAL_POSX;
    player.y = PADDLE_INITIAL_POSY;
    player.velocity_x = PLAYER_X_VELOCITY;
    player.velocity_y = PLAYER_Y_VELOCITY;
    player.isBall = 0;
    return player;
}

struct game_object check_collision(struct game_object object){
    if(object.y + object.h > WINDOW_HEIGHT - 5) object.velocity_y *= -1;
    else if(object.y <  5) object.velocity_y *= -1;
    else if(object.x > WINDOW_WIDTH - 5) object.velocity_x *= -1;
    else if(object.x < 5) object.velocity_x *= -1;
    return object;
}

void render(object objects_to_render[]) {
    int i;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for(i = 0; i < sizeof(&objects_to_render); i++) {
        SDL_Rect object_to_render = {
            objects_to_render[i].x,
            objects_to_render[i].y,
            objects_to_render[i].w,
            objects_to_render[i].h
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if(objects_to_render[i].isBall == 0) {
            SDL_RenderFillRect(renderer, &object_to_render);
        } 
        else {
            filledCircleRGBA(renderer, objects_to_render[i].x, objects_to_render[i].y, BALL_RADIUS, 255, 255, 255, 255);
        }
    }
    SDL_RenderPresent(renderer);
}



void update(void){
    // Update the game objects
    int i;
    for(i = 0; i < sizeof(objects_to_render); i++) {
        struct game_object object = objects_to_render[i];
        object = check_collision(object);
        object.x += object.velocity_x;
        object.y += object.velocity_y;
        objects_to_render[i] = object;
    }
}

int handle_user_input(void) {
    SDL_Event e;
    // Poll for quit events
    while(SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            game_is_running = 0;
        }
    }
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_DOWN]) {
        objects_to_render[0].velocity_y = abs(objects_to_render[0].velocity_y); // Ensure positive velocity
    }
    if(state[SDL_SCANCODE_UP]) {
        objects_to_render[0].velocity_y = -abs(objects_to_render[0].velocity_y); // Ensure negative velocity
    }
    return 0;
}

int quit(void) {
    // Clear objects_to_render Array
    for (int i = 0; i < sizeof(objects_to_render); i++) {
        objects_to_render[i].x = 0;
        objects_to_render[i].y = 0;
        objects_to_render[i].w = 0;
        objects_to_render[i].h = 0;
        objects_to_render[i].velocity_x = 0;
        objects_to_render[i].velocity_y = 0;
        objects_to_render[i].isBall = 0;
    }
    // Destroy SDL
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = NULL;
    renderer = NULL;
    SDL_Quit();
    return 0;
}

int main(int argc, char* argv[]) {
    // INITIALIZE //
    game_is_running = initialize_window();
    struct game_object player = initialize_player();
    struct game_object ball = initialize_ball();
    objects_to_render[0] = player;
    objects_to_render[1] = ball;
    //-----------------------------------------------------//
    Uint32 last_frame_time = SDL_GetTicks();
   
    while (game_is_running) {
        while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_DELAY));
        last_frame_time = SDL_GetTicks();
        // Handle keyboard state for game logic
        handle_user_input();
        update();
        render(objects_to_render);
    }
    // quit the game
    quit();
    return 0;
}
