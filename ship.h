/* coded by Jared Bruni */
#ifndef SHIP_H
#define SHIP_H

#include <math.h>
#include <stdbool.h>

#define SCALE_W 640
#define SCALE_H 360
#define PI 3.14159265358979323846
#define MAX_PARTICLES 40
#define ASTEROID_VERTICES 8  

typedef struct {
    float x, y;
    float vx, vy;
    float angle;  
    int lives;
    int fire_cooldown;
    int burst_count;
    bool exploding;           
    int explosion_timer;      
    int score;  
    int continuous_fire_timer;
    int overheated;
    int overheat_cooldown;
} Ship;

extern Ship the_ship;
extern bool keyLeft, keyRight, keyThrust;

void init_ship(void);
void draw_ship(void);
void update_ship(void);
bool can_fire(void);
void update_fire_timer(void);

typedef struct {
    float x, y;
    float vx, vy;
    float angle;
    bool active;
} Projectile;

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    bool active;
    float vertices[ASTEROID_VERTICES][2];  
    float rotation_angle;
    float rotation_speed;
} Asteroid;

typedef struct {
    float x, y;
    float vx, vy;
    int lifetime;
    bool active;
} Particle;

void init_projectiles(void);
void update_projectiles(void);
void draw_projectiles(void);
void fire_projectile(float x, float y, float angle);

void init_asteroids(void);
void update_asteroids(void);
void draw_asteroids(void);
void spawn_asteroid(float x, float y, float vx, float vy, float radius);

void check_and_spawn_asteroids(void);
void check_asteroid_collisions(void);
void check_ship_asteroid_collision(void);
void start_ship_explosion(void);
void update_explosion(void);
void draw_explosion(void);
void create_asteroid_explosion(float x, float y);

void check_projectile_asteroid_collisions(void);
void split_asteroid(int index);
void check_and_spawn_asteroids(void);
void update_ship(void);
void draw_score(void);


#endif