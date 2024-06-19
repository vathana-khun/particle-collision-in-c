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
float last_frame_time = 0;
struct particle particles[AMOUNT_PARTICLE];
float gravity;
float collision_damping;
int numParticles = sizeof(particles) / sizeof(particles[0]);


void circle(SDL_Renderer*renderer, int x_center, int y_center, int r);
void create_particle(struct particle p[], int r, int numParticles);
void render_particle(SDL_Renderer *render,struct particle p[],  int numParticles);
void update_particle(struct particle p[],  int numParticles,float dt);
void collision_detection(struct particle *P, int numParticles, float dt);
void solve_collision(struct particle *p1, struct particle *p2, float dt);
bool collide(struct particle *p1, struct particle *p2);
void boundary_collision(struct particle *p, float dt);


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
    gravity = 4*60;
    collision_damping = 0.7f;
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
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    float dt = (SDL_GetTicks() - last_frame_time) /1000.0f;
    last_frame_time  = SDL_GetTicks();

    // printf("Delta Time is: %f\n",dt);
    update_particle(particles, numParticles,dt);
    collision_detection(particles, numParticles, dt);
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

        // p[i].velocity[0] = (rand() % (3 - 1 + 1) + 1) * 60;
        // p[i].velocity[1] = (rand() % (3 - 1 + 1) + 1 )* 60;

        p[i].velocity[0] = 0;
        p[i].velocity[1] = 100;

        // printf("creating velocity of particle:%d (%f , %f)\n", i , p[i].velocity[0],p[i].velocity[1]);
    }
}

void update_particle(struct particle p[], int numParticles,float dt){
    for(int i = 0; i < numParticles; i++){
        // printf(" before update velocity of particle:%d (%f , %f) dt is %f\n", i , p[i].velocity[0],p[i].velocity[1], dt);
        p[i].position[0] += p[i].velocity[0] * dt ;
        p[i].position[1] += p[i].velocity[1] * dt;

        p[i].velocity[1] += gravity * dt;
        
        boundary_collision(&p[i], dt);

        // printf(" after update velocity of particle:%d (%f , %f) dt is %f\n", i , p[i].velocity[0],p[i].velocity[1],dt);
    }
}

void collision_detection(struct particle p[], int numParticles, float dt){
    for (int i = 0; i < numParticles; i++) {
       for (int j = i + 1 ; j<numParticles; j++){
            if (collide(&p[i] , &p[j])){
                solve_collision(&p[i] , &p[j],dt);
            }
       }
    }
}
void boundary_collision(struct particle *p, float dt){
    float x = p -> position[0];
    float y = p -> position[1];
    float r = p -> radius;
        // Check boundaries and handle collision
        // printf("Before boundary collision - Position (%f, %f), Velocity (%f, %f)\n", x, y, p->velocity[0], p->velocity[1]);
        if (x > WIDTH - r) {
            p->position[0] = 2 * (WIDTH - r) - x;
            p->velocity[0] *= -1 * collision_damping ;
        } else if (x < r) {
            p->position[0] = 2 * r - x;
            p->velocity[0] *= -1 * collision_damping ;
        }
        if (y > HEIGHT - r) {
            p->position[1] = 2 * (HEIGHT - r) - y;
            p->velocity[1] *= -1 * collision_damping ;
        } else if (y < r) {
            p->position[1] = 2 * r - y;
            p->velocity[1] *= -1 * collision_damping ;
        }
        //  printf("After boundary collision - Position (%f, %f), Velocity (%f, %f)\n", p->position[0], p->position[1], p->velocity[0], p->velocity[1]);
}
bool collide(struct particle *p1, struct particle *p2){
        double d = sqrt((pow (p1->position[0] - p2->position[0],2)) + (pow (p1->position[1] - p2->position[1],2)));
        double min_dist = p1->radius + p2->radius; 
        if(d < min_dist){ 
            return true;
        }
    return false;
}
void solve_collision(struct particle *p1, struct particle *p2, float dt){
    float x1 = p1->position[0];
    float y1 = p1->position[1];
    float v1_x = p1->velocity[0];
    float v1_y = p1->velocity[1];

    double d = sqrt((pow (p1->position[0] - p2->position[0],2)) + (pow (p1->position[1] - p2->position[1],2)));
    double min_dist = p1->radius + p2->radius;
    float m1 = p1->radius*0.1;    
    float m2 = p2->radius*0.1; 

    float x2 = p2->position[0];
    float y2 = p2->position[1];
    float v2_x = p2->velocity[0];    float v2_y = p2->velocity[1];

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

    p1->velocity[0] = vf1_x;
    p1->velocity[1] = vf1_y;
    p2->velocity[0] = vf2_x;
    p2->velocity[1] = vf2_y;

    p1->velocity[0] *= collision_damping ;
    p1->velocity[1] *= collision_damping ;
    
    p1->position[0] = x1;
    p1->position[1] = y1;
    p2->position[0] = x2;
    p2->position[1] = y2;
}
void render_particle(SDL_Renderer *render,struct particle p[], int numParticles){
   
    for(int i = 0; i < numParticles; i++){
            circle(renderer, p[i].position[0], p[i].position[1], p[i].radius);
        }
}


