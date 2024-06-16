#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "constant.h"
 
SDL_Window *window = NULL; 
SDL_Renderer *renderer = NULL;
bool game_is_running = false;
int amount_particle;

struct particle{
    float position[2];
    float velocity[2];
    float radius;
};

bool intialize_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) != false ){
        fprintf(stderr, "Error Initalizing SDL. \n");
        return false;
    }
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);

    if (!window){
        fprintf(stderr, "Error creating SDL window. \n");
        return false;
    }    
    renderer = SDL_CreateRenderer(window,-1,0);

    if (!renderer){
        fprintf(stderr, "Error creating SDL Renderer. \n");
        return false;
    }
    return true;
}

void setup(){

}

void process_input(){
    SDL_Event event; 
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT:
            game_is_running = false; 
            break;
        case SDL_KEYDOWN:
            if ( event.key.keysym.sym == SDLK_ESCAPE){ 
                game_is_running= false;
                break;
            }      
    }
}
void update(){

}
void render(){

}

void clean(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}

int main ( int argc, char*argv[]){
    game_is_running = intialize_window();

    setup();

    while(game_is_running){
        void process_input();
        void update();
        void render();  
    }
    clean();

    return 0;
}




