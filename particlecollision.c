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
float gravity;
float collision_damping;
int numParticles = sizeof(particles) / sizeof(particles[0]);


void circle(SDL_Renderer*renderer, int x_center, int y_center, int r);
void create_particle(struct particle p[], int r, int numParticles);
void render_particle(SDL_Renderer *render,struct particle p[],  int numParticles);
void update_particle(struct particle p[],  int numParticles);
void collision_detection(struct particle p[], int numParticles);
void solve_collision(struct particle p[], int numParticles);


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
    gravity = 0.5f;
    collision_damping = 0.9f;
    create_particle(particles, 10, numParticles);
    // particles[0].position[0] = WIDTH/4;
    // particles[0].position[1] = HEIGHT/2;
    // particles[0].velocity[0] = 1;
    // particles[0].velocity[1] = 0;
    // particles[0].radius = 40;

    // particles[1].position[0] = WIDTH * 0.75f;
    // particles[1].position[1] = HEIGHT/2;
    // particles[1].velocity[0] = -1;
    // particles[1].velocity[1] = 0;
    // particles[1].radius = 40;
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

    update_particle(particles, numParticles);
    collision_detection(particles, numParticles);
    solve_collision(particles, numParticles);
    

}
void render(){
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    render_particle(renderer, particles, numParticles);

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

void create_particle(struct particle p[], int r, int numParticles){
    

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

void update_particle(struct particle p[], int numParticles){
    for(int i = 0; i < numParticles; i++){
        float *x = &(p[i].position[0]);
        float *y = &(p[i].position[1]);
        float *xspeed = &(p[i].velocity[0]);
        float *yspeed = &(p[i].velocity[1]);

        *yspeed += gravity;

        *x += *xspeed;
        *y += *yspeed;
    }
}

void collision_detection(struct particle p[], int numParticles){
    
for (int i = 0; i < numParticles; i++) {
    int x = p[i].position[0];
    int y = p[i].position[1];
    int r = p[i].radius;

    // Check boundaries and handle collision
    if (x > WIDTH - r) {
        // Adjust position
        p[i].position[0] = 2 * (WIDTH - r) - x;
        // Reverse velocity
        p[i].velocity[0] *= -1;
    } else if (x < r) {
        // Adjust position
        p[i].position[0] = 2 * r - x;
        // Reverse velocity
        p[i].velocity[0] *= -1;
    }

    if (y > HEIGHT - r) {
        // Adjust position
        p[i].position[1] = 2 * (HEIGHT - r) - y;
        // Reverse velocity
        p[i].velocity[1] *= -1;
    } else if (y < r) {
        // Adjust position
        p[i].position[1] = 2 * r - y;
        // Reverse velocity
        p[i].velocity[1] *= -1;
    }
}
}

void solve_collision(struct particle p[], int numParticles){
     for(int i = 0; i < numParticles; i++){
        for(int j = i + 1 ; j< numParticles; j++){
            double d = sqrt((pow (p[i].position[0] - p[j].position[0],2)) + (pow (p[i].position[1] - p[j].position[1],2)));
            double min_dist = p[i].radius + p[j].radius; 
            float m1 = p[i].radius;
            float m2 = p[i].radius;       
            if(d < min_dist){  

                float x1 = p[i].position[0];
                float y1 = p[i].position[1];
                float v1_x = p[i].velocity[0];
                float v1_y = p[i].velocity[1];

                float x2 = p[j].position[0];
                float y2 = p[j].position[1];
                float v2_x = p[j].velocity[0];
                float v2_y = p[j].velocity[1];

                // calculated unit normal vector
                float un_x = (x2 - x1) / d;
                float un_y = (y2 - y1) / d;

                //calculated unit tagnent vector
                float ut_x = un_y * -1;
                float ut_y =  un_x;

                //Correct the first 
                float correction = (d - min_dist);
                float correction_x = un_x * correction / 2;
                float correction_y = un_y * correction / 2; 

                x1 += correction_x;
                y1 += correction_y;

                x2 -= correction_x;
                y2 -= correction_y;

                // Calculated the Scalar Part.
                float v1_n = (v1_x * un_x) + (v1_y * un_y);  
                float v2_n = (v2_x * un_x) + (v2_y * un_y); 
                float v1_t = (v1_x * ut_x) + (v1_y * ut_y);
                float v2_t = (v2_x * ut_x) + (v2_y * ut_y);

                float vf1_t = v1_t;
                float vf2_t = v2_t;

                float vf1_n = (( v1_n * (m1 - m2) + 2 * m2 * v2_n) / (m1+m2));
                float vf2_n = (( v2_n * (m2 - m1) + 2 * m1 * v1_n) / (m1+m2));

                //Converting to Vector
                float vf1_n_x = vf1_n * un_x;
                float vf1_n_y = vf1_n * un_y;
                float vf2_n_x = vf2_n * un_x;
                float vf2_n_y = vf2_n * un_y;    

                float vf1_t_x = vf1_t * ut_x;
                float vf1_t_y = vf1_t * ut_y;
                float vf2_t_x = vf2_t * ut_x;
                float vf2_t_y = vf2_t *ut_y;

                float vf1_x = vf1_n_x + vf1_t_x;
                float vf1_y = vf1_n_y + vf1_t_y;
                float vf2_x = vf2_n_x + vf2_t_x;
                float vf2_y = vf2_n_y + vf2_t_y;


                p[i].velocity[0] = vf1_x;
                p[i].velocity[1] = vf1_y;
                p[j].velocity[0] = vf2_x;
                p[j].velocity[1] = vf2_y;

                p[i].velocity[0] *= collision_damping;
                p[i].velocity[1] *= collision_damping;

                p[i].position[0] = x1;
                p[i].position[1] = y1;
                p[j].position[0] = x2;
                p[j].position[1] = y2;

            }

            }   
        }
     }



void render_particle(SDL_Renderer *render,struct particle p[], int numParticles){
   
    for(int i = 0; i < numParticles; i++){
            circle(renderer, p[i].position[0], p[i].position[1], p[i].radius);
        }
}


