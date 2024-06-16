#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include "constant.h"
 
struct particle{
    float position[2];
    float velocity[2];
    float radius;
};


SDL_Window *window = NULL; 
SDL_Renderer *renderer = NULL;
bool game_is_running = false;
int last_frame_time = 0;
struct particle particles[AMOUNT_PARTICLE];


void circle(SDL_Renderer*renderer, int x_center, int y_center, int r);
void create_particle(struct particle p[], int r);
void render_particle(SDL_Renderer *render,struct particle p[]);
void update_particle(struct particle p[]);
void collision_detection(struct particle p[]);


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
    create_particle(particles, 20);
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
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()- last_frame_time);

    if (time_to_wait >0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    last_frame_time = SDL_GetTicks();

    update_particle(particles);
    collision_detection(particles);

}
void render(){
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    render_particle(renderer, particles);

    SDL_RenderPresent(renderer);
}

void clean(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

int main ( int argc, char*argv[]){
    game_is_running = intialize_window();

    srand(time(0));

    setup();

    while(game_is_running){
        process_input();
        update();
        render();  
    }
    clean();

    return 0;
}

void circle(SDL_Renderer*renderer, int x_center, int y_center, int r){
    int x = r; 
    int y = 0;
    int p = 1-r;

    while(x>=y){
        SDL_RenderDrawPoint(renderer, x + x_center, y + y_center);
        SDL_RenderDrawPoint(renderer, y + x_center, x + y_center);
        SDL_RenderDrawPoint(renderer, -y + x_center, x + y_center);
        SDL_RenderDrawPoint(renderer, -x + x_center, y + y_center);
        SDL_RenderDrawPoint(renderer, -x + x_center, -y + y_center);
        SDL_RenderDrawPoint(renderer, -y + x_center, -x + y_center);
        SDL_RenderDrawPoint(renderer, y + x_center, -x + y_center);
        SDL_RenderDrawPoint(renderer, x + x_center, -y + y_center);

        y++;
        if (p <= 0){
            p += 2 * y  + 1;

        }else if (p > 0) {
            x--;
            p += 2*y - 2*x +1;  
        }
    }
}

void create_particle(struct particle p[], int r){
    int numParticles = sizeof(particles) / sizeof(particles[0]);

    for(int i = 0; i < numParticles; i++){
        //First Loop Set Radius
        p[i].radius = r;
        //Loop Twice for both position and velocity
        
        p[i].position[0] = rand() % ( (WIDTH - 20) - 20 + 1 ) + 20;
        p[i].position[1] = rand() % ( (HEIGHT - 20) - 20 + 1 ) + 20;

        p[i].velocity[0] = rand() % (5 - 1 + 1) + 1;
        p[i].velocity[1] = rand() % (5- 1 + 1) + 1;
    }
}

void update_particle(struct particle p[]){
    int numParticles = sizeof(particles) / sizeof(particles[0]);
    for(int i = 0; i < numParticles; i++){
        // int x = p[i].position[0];
        // int y = p[i].position[1];
        // int xspeed = p[i].velocity[0];
        // int yspeed = p[i].velocity[1];

        // x += xspeed;
        // y += yspeed;
        p[i].position[0] += p[i].velocity[0];
        p[i].position[1] += p[i].velocity[1];
    }
}

void collision_detection(struct particle p[]){
    int numParticles = sizeof(particles) / sizeof(particles[0]);
    
    for(int i = 0; i < numParticles; i++){
        int x = p[i].position[0];
        int y = p[i].position[1];
        int r = p[i].radius;

        if ( x > WIDTH - r|| x < r ) {
            p[i].velocity[0] *= -1;  
        }
        if ( y > HEIGHT - r  || y  < r ){
            p[i].velocity[1] *= -1;  
        }
    }

}

void render_particle(SDL_Renderer *render,struct particle p[]){
    int numParticles = sizeof(particles) / sizeof(particles[0]);
    for(int i = 0; i < numParticles; i++){
            circle(renderer, p[i].position[0], p[i].position[1], p[i].radius);
        }
}


