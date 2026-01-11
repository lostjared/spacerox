#include "ship.h"
#include "sdl.h"

#define MAX_PROJECTILES 50
#define MAX_ASTEROIDS 30
#define PROJECTILE_SPEED 5.0f
#define PROJECTILE_LIFETIME 60  
#define FIRE_COOLDOWN 5        
#define SHOTS_PER_BURST 5      
#define FIRE_DELAY 3           

#define EXPLOSION_DURATION 90     
#define RESPAWN_DELAY 120         
#define INVULNERABLE_TIME 180     

#define MIN_ASTEROID_RADIUS 8.0f  
#define LARGE_ASTEROID_POINTS 20
#define MEDIUM_ASTEROID_POINTS 50
#define SMALL_ASTEROID_POINTS 100

#define ASTEROID_VERTICES 8  

Ship the_ship;
bool keyLeft = false, keyRight = false, keyThrust = false;

Projectile projectiles[MAX_PROJECTILES];
Asteroid asteroids[MAX_ASTEROIDS];
Particle explosion_particles[MAX_PARTICLES];


void init_projectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }
}

void update_projectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            
            projectiles[i].x += projectiles[i].vx;
            projectiles[i].y += projectiles[i].vy;
            
            
            if (projectiles[i].x < 0 || projectiles[i].x >= SCALE_W || 
                projectiles[i].y < 0 || projectiles[i].y >= SCALE_H) {
                projectiles[i].active = false;
                continue;
            }
            
            
            projectiles[i].angle--;
            if (projectiles[i].angle <= 0) {
                projectiles[i].active = false;
            }
        }
    }
}

void draw_projectiles(void) {
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
    
            float endX = projectiles[i].x - projectiles[i].vx * 0.5;
            float endY = projectiles[i].y - projectiles[i].vy * 0.5;
            SDL_RenderDrawLine(renderer, 
                              (int)projectiles[i].x, (int)projectiles[i].y,
                              (int)endX, (int)endY);
        }
    }
}

void fire_projectile(float x, float y, float angle) {
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            
            float c = cosf(angle);
            float s = sinf(angle);
            float lx1 = 0, ly1 = -12;  
            
            
            projectiles[i].x = x + (lx1 * c - ly1 * s);
            projectiles[i].y = y + (lx1 * s + ly1 * c);
            
            projectiles[i].vx = sinf(angle) * PROJECTILE_SPEED;
            projectiles[i].vy = -cosf(angle) * PROJECTILE_SPEED; 
            projectiles[i].angle = PROJECTILE_LIFETIME;  
            projectiles[i].active = true;
            break;
        }
    }
}


void init_asteroids(void) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].active = false;
    }

    for (int i = 0; i < 4; i++) {
        float x, y;
        
        do {
            x = rand() % SCALE_W;
            y = rand() % SCALE_H;
        } while (hypotf(x - the_ship.x, y - the_ship.y) < 100);
        
        float vx = ((rand() % 100) - 50) / 50.0f;  
        float vy = ((rand() % 100) - 50) / 50.0f;  
        float radius = 20.0f + (rand() % 20);      
        
        spawn_asteroid(x, y, vx, vy, radius);
    }
}

void update_asteroids(void) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].x += asteroids[i].vx;
            asteroids[i].y += asteroids[i].vy;
            asteroids[i].rotation_angle += asteroids[i].rotation_speed;
            if (asteroids[i].x < 0) asteroids[i].x += SCALE_W;
            if (asteroids[i].x >= SCALE_W) asteroids[i].x -= SCALE_W;
            if (asteroids[i].y < 0) asteroids[i].y += SCALE_H;
            if (asteroids[i].y >= SCALE_H) asteroids[i].y -= SCALE_H;
        }
    }
}

void draw_asteroids(void) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            
            float cos_rot = cosf(asteroids[i].rotation_angle);
            float sin_rot = sinf(asteroids[i].rotation_angle);
            
            
            float rotated_vertices[ASTEROID_VERTICES][2];
            for (int j = 0; j < ASTEROID_VERTICES; j++) {
                float orig_x = asteroids[i].vertices[j][0];
                float orig_y = asteroids[i].vertices[j][1];
                
                rotated_vertices[j][0] = orig_x * cos_rot - orig_y * sin_rot;
                rotated_vertices[j][1] = orig_x * sin_rot + orig_y * cos_rot;
            }
            
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            
            
            for (int j = 0; j < ASTEROID_VERTICES; j++) {
                int next = (j + 1) % ASTEROID_VERTICES;
                
                int x1 = (int)(asteroids[i].x + rotated_vertices[j][0]);
                int y1 = (int)(asteroids[i].y + rotated_vertices[j][1]);
                int x2 = (int)(asteroids[i].x + rotated_vertices[next][0]);
                int y2 = (int)(asteroids[i].y + rotated_vertices[next][1]);
                
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            
            
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            
            float inner_vertices[ASTEROID_VERTICES][2];
            for (int j = 0; j < ASTEROID_VERTICES; j++) {
                inner_vertices[j][0] = rotated_vertices[j][0] * 0.6f;
                inner_vertices[j][1] = rotated_vertices[j][1] * 0.6f;
            }
            
            for (int j = 0; j < ASTEROID_VERTICES; j++) {
                int next = (j + 1) % ASTEROID_VERTICES;
                
                int x1 = (int)(asteroids[i].x + inner_vertices[j][0]);
                int y1 = (int)(asteroids[i].y + inner_vertices[j][1]);
                int x2 = (int)(asteroids[i].x + inner_vertices[next][0]);
                int y2 = (int)(asteroids[i].y + inner_vertices[next][1]);
                
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            
            for (int j = 0; j < ASTEROID_VERTICES; j += 2) {
                int outer_x = (int)(asteroids[i].x + rotated_vertices[j][0]);
                int outer_y = (int)(asteroids[i].y + rotated_vertices[j][1]);
                int inner_x = (int)(asteroids[i].x + inner_vertices[j][0]);
                int inner_y = (int)(asteroids[i].y + inner_vertices[j][1]);
                
                SDL_RenderDrawLine(renderer, outer_x, outer_y, inner_x, inner_y);
            }
            
            SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
            
            int center_x = (int)asteroids[i].x;
            int center_y = (int)asteroids[i].y;
            
            for (int j = 0; j < ASTEROID_VERTICES; j += 4) {
                int vertex_x = (int)(asteroids[i].x + inner_vertices[j][0]);
                int vertex_y = (int)(asteroids[i].y + inner_vertices[j][1]);
                
                SDL_RenderDrawLine(renderer, center_x, center_y, vertex_x, vertex_y);
            }
            
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
            
            for (int j = 0; j < ASTEROID_VERTICES; j += 2) {
                int next = (j + 1) % ASTEROID_VERTICES;
                
                int outer_x1 = (int)(asteroids[i].x + rotated_vertices[j][0]);
                int outer_y1 = (int)(asteroids[i].y + rotated_vertices[j][1]);
                int inner_x2 = (int)(asteroids[i].x + inner_vertices[next][0]);
                int inner_y2 = (int)(asteroids[i].y + inner_vertices[next][1]);
                
                SDL_RenderDrawLine(renderer, outer_x1, outer_y1, inner_x2, inner_y2);
            }
        }
    }
}

void spawn_asteroid(float x, float y, float vx, float vy, float radius) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active) {
            asteroids[i].x = x;
            asteroids[i].y = y;
            asteroids[i].vx = vx;
            asteroids[i].vy = vy;
            asteroids[i].radius = radius;
            asteroids[i].active = true;
            
            asteroids[i].rotation_angle = 0.0f;
            asteroids[i].rotation_speed = ((rand() % 100) - 50) / 1000.0f; 
         

            for (int j = 0; j < ASTEROID_VERTICES; j++) {
                float angle = (float)j / ASTEROID_VERTICES * 2 * PI;
                
                float vertex_radius = radius * (0.7f + ((rand() % 30) / 100.0f));
                
                asteroids[i].vertices[j][0] = cosf(angle) * vertex_radius;
                asteroids[i].vertices[j][1] = sinf(angle) * vertex_radius;
            }
            
            break;
        }
    }
}

void init_ship(void) {
    the_ship.x = SCALE_W / 2.0f;
    the_ship.y = SCALE_H / 2.0f;
    the_ship.vx = 0.0f;
    the_ship.vy = 0.0f;
    the_ship.angle = 0.0f;
    the_ship.lives = 3;
    the_ship.fire_cooldown = 0;
    the_ship.burst_count = 0;
    the_ship.exploding = false;
    the_ship.explosion_timer = 0;
    the_ship.score = 0;
    the_ship.continuous_fire_timer = 0;
    the_ship.overheated = false;
    the_ship.overheat_cooldown = 0;
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        explosion_particles[i].active = false;
    }
}

void draw_ship(void) {
    if (the_ship.exploding) {
        draw_explosion();
        return;
    }
    
    
    float nose_x = 0, nose_y = -15;
    float left_wing_x = -12, left_wing_y = 8;
    float right_wing_x = 12, right_wing_y = 8;
    
    float left_engine_x = -8, left_engine_y = 12;
    float right_engine_x = 8, right_engine_y = 12;
    float center_engine_x = 0, center_engine_y = 10;
    
    float left_detail_x = -6, left_detail_y = 2;
    float right_detail_x = 6, right_detail_y = 2;
    
    float cockpit_left_x = -3, cockpit_left_y = -8;
    float cockpit_right_x = 3, cockpit_right_y = -8;
    float cockpit_center_x = 0, cockpit_center_y = -5;
    
    float c = cosf(the_ship.angle);
    float s = sinf(the_ship.angle);
    
    int vertices[11][2];
    
    vertices[0][0] = (int)(the_ship.x + (nose_x * c - nose_y * s));
    vertices[0][1] = (int)(the_ship.y + (nose_x * s + nose_y * c));
    
    vertices[1][0] = (int)(the_ship.x + (left_wing_x * c - left_wing_y * s));
    vertices[1][1] = (int)(the_ship.y + (left_wing_x * s + left_wing_y * c));
    
    vertices[2][0] = (int)(the_ship.x + (right_wing_x * c - right_wing_y * s));
    vertices[2][1] = (int)(the_ship.y + (right_wing_x * s + right_wing_y * c));
    
    vertices[3][0] = (int)(the_ship.x + (left_engine_x * c - left_engine_y * s));
    vertices[3][1] = (int)(the_ship.y + (left_engine_x * s + left_engine_y * c));
    
    vertices[4][0] = (int)(the_ship.x + (right_engine_x * c - right_engine_y * s));
    vertices[4][1] = (int)(the_ship.y + (right_engine_x * s + right_engine_y * c));
    
    vertices[5][0] = (int)(the_ship.x + (center_engine_x * c - center_engine_y * s));
    vertices[5][1] = (int)(the_ship.y + (center_engine_x * s + center_engine_y * c));
    
    vertices[6][0] = (int)(the_ship.x + (left_detail_x * c - left_detail_y * s));
    vertices[6][1] = (int)(the_ship.y + (left_detail_x * s + left_detail_y * c));
    
    vertices[7][0] = (int)(the_ship.x + (right_detail_x * c - right_detail_y * s));
    vertices[7][1] = (int)(the_ship.y + (right_detail_x * s + right_detail_y * c));
    
    vertices[8][0] = (int)(the_ship.x + (cockpit_left_x * c - cockpit_left_y * s));
    vertices[8][1] = (int)(the_ship.y + (cockpit_left_x * s + cockpit_left_y * c));
    
    vertices[9][0] = (int)(the_ship.x + (cockpit_right_x * c - cockpit_right_y * s));
    vertices[9][1] = (int)(the_ship.y + (cockpit_right_x * s + cockpit_right_y * c));
    
    vertices[10][0] = (int)(the_ship.x + (cockpit_center_x * c - cockpit_center_y * s));
    vertices[10][1] = (int)(the_ship.y + (cockpit_center_x * s + cockpit_center_y * c));
    
    
    if (the_ship.overheated) {
    
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    } else if (the_ship.continuous_fire_timer > 120) {
    
        int heat_level = the_ship.continuous_fire_timer - 120;
        SDL_SetRenderDrawColor(renderer, 255, 255 - (heat_level * 4), 0, 255);
    } else {
    
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    
    
    SDL_RenderDrawLine(renderer, vertices[0][0], vertices[0][1], vertices[1][0], vertices[1][1]);
    SDL_RenderDrawLine(renderer, vertices[0][0], vertices[0][1], vertices[2][0], vertices[2][1]);
    SDL_RenderDrawLine(renderer, vertices[1][0], vertices[1][1], vertices[2][0], vertices[2][1]);
    
    SDL_RenderDrawLine(renderer, vertices[1][0], vertices[1][1], vertices[3][0], vertices[3][1]);
    SDL_RenderDrawLine(renderer, vertices[2][0], vertices[2][1], vertices[4][0], vertices[4][1]);
    SDL_RenderDrawLine(renderer, vertices[3][0], vertices[3][1], vertices[5][0], vertices[5][1]);
    SDL_RenderDrawLine(renderer, vertices[4][0], vertices[4][1], vertices[5][0], vertices[5][1]);
    
    
    if (the_ship.overheated) {
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    } else if (the_ship.continuous_fire_timer > 120) {
        int heat_level = the_ship.continuous_fire_timer - 120;
        SDL_SetRenderDrawColor(renderer, 200, 200 - (heat_level * 3), 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    }
    
    SDL_RenderDrawLine(renderer, vertices[0][0], vertices[0][1], vertices[6][0], vertices[6][1]);
    SDL_RenderDrawLine(renderer, vertices[0][0], vertices[0][1], vertices[7][0], vertices[7][1]);
    SDL_RenderDrawLine(renderer, vertices[6][0], vertices[6][1], vertices[7][0], vertices[7][1]);
    SDL_RenderDrawLine(renderer, vertices[6][0], vertices[6][1], vertices[1][0], vertices[1][1]);
    SDL_RenderDrawLine(renderer, vertices[7][0], vertices[7][1], vertices[2][0], vertices[2][1]);
    
    
    if (the_ship.overheated) {
        SDL_SetRenderDrawColor(renderer, 0, 128, 128, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    }
    SDL_RenderDrawLine(renderer, vertices[8][0], vertices[8][1], vertices[9][0], vertices[9][1]);
    SDL_RenderDrawLine(renderer, vertices[8][0], vertices[8][1], vertices[10][0], vertices[10][1]);
    SDL_RenderDrawLine(renderer, vertices[9][0], vertices[9][1], vertices[10][0], vertices[10][1]);
    
    
    if (the_ship.overheated) {
        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    }
    SDL_RenderDrawLine(renderer, vertices[10][0], vertices[10][1], vertices[5][0], vertices[5][1]);
    
    
    if (keyThrust) {
        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
        
        int flame1_x = (int)(the_ship.x + ((-6) * c - 18 * s));
        int flame1_y = (int)(the_ship.y + ((-6) * s + 18 * c));
        int flame2_x = (int)(the_ship.x + (0 * c - 22 * s));
        int flame2_y = (int)(the_ship.y + (0 * s + 22 * c));
        int flame3_x = (int)(the_ship.x + (6 * c - 18 * s));
        int flame3_y = (int)(the_ship.y + (6 * s + 18 * c));
        
        SDL_RenderDrawLine(renderer, vertices[3][0], vertices[3][1], flame1_x, flame1_y);
        SDL_RenderDrawLine(renderer, vertices[5][0], vertices[5][1], flame2_x, flame2_y);
        SDL_RenderDrawLine(renderer, vertices[4][0], vertices[4][1], flame3_x, flame3_y);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        int inner_flame_x = (int)(the_ship.x + (0 * c - 20 * s));
        int inner_flame_y = (int)(the_ship.y + (0 * s + 20 * c));
        SDL_RenderDrawLine(renderer, vertices[5][0], vertices[5][1], inner_flame_x, inner_flame_y);
    }
}


void check_asteroid_collisions(void) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!asteroids[i].active) continue;
        
        for (int j = i + 1; j < MAX_ASTEROIDS; j++) {
            if (!asteroids[j].active) continue;
            
            float dx = asteroids[i].x - asteroids[j].x;
            float dy = asteroids[i].y - asteroids[j].y;
            float distance = sqrtf(dx * dx + dy * dy);
            float min_distance = asteroids[i].radius + asteroids[j].radius;
            
            if (distance < min_distance && distance > 0) {
                float nx = dx / distance;
                float ny = dy / distance;
                
                float overlap = min_distance - distance;
                float separation = overlap * 0.5f;
                
                asteroids[i].x += nx * separation;
                asteroids[i].y += ny * separation;
                asteroids[j].x -= nx * separation;
                asteroids[j].y -= ny * separation;
                
                float rel_vx = asteroids[i].vx - asteroids[j].vx;
                float rel_vy = asteroids[i].vy - asteroids[j].vy;
                
                float vel_along_normal = rel_vx * nx + rel_vy * ny;
                
                if (vel_along_normal > 0) continue;
                
                float restitution = 0.8f;
                float impulse = -(1 + restitution) * vel_along_normal;
                
                float mass_ratio = 0.5f;
                
                asteroids[i].vx += impulse * mass_ratio * nx;
                asteroids[i].vy += impulse * mass_ratio * ny;
                asteroids[j].vx -= impulse * mass_ratio * nx;
                asteroids[j].vy -= impulse * mass_ratio * ny;
            }
        }
    }
}


void check_ship_asteroid_collision(void) {
    if (the_ship.exploding) return; 
    
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            float dx = the_ship.x - asteroids[i].x;
            float dy = the_ship.y - asteroids[i].y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            float ship_radius = 8.0f;
            
            if (distance < asteroids[i].radius + ship_radius) {
                start_ship_explosion();
                
                float explosion_x = the_ship.x + dx * 0.5f;
                float explosion_y = the_ship.y + dy * 0.5f;
                create_asteroid_explosion(explosion_x, explosion_y);
                
                float bounce_force = 2.0f;
                float norm_x = -dx / distance;
                float norm_y = -dy / distance;
                
                asteroids[i].vx += norm_x * bounce_force;
                asteroids[i].vy += norm_y * bounce_force;
                
                break;
            }
        }
    }
}

void start_ship_explosion(void) {
    if (!the_ship.exploding) {
        the_ship.exploding = true;
        the_ship.explosion_timer = EXPLOSION_DURATION;
        the_ship.lives--; 
        
        
        the_ship.overheated = false;
        the_ship.overheat_cooldown = 0;
        the_ship.continuous_fire_timer = 0;
        the_ship.burst_count = 0;
        
        for (int i = 0; i < MAX_PARTICLES; i++) {
            explosion_particles[i].x = the_ship.x + ((rand() % 20) - 10);
            explosion_particles[i].y = the_ship.y + ((rand() % 20) - 10);
            
            float angle = (rand() % 628) / 100.0f;
            float speed = 0.5f + (rand() % 20) / 10.0f;
            
            explosion_particles[i].vx = cosf(angle) * speed;
            explosion_particles[i].vy = sinf(angle) * speed;
            
            explosion_particles[i].lifetime = 30 + (rand() % 60);
            explosion_particles[i].active = true;
        }
    }
}

void create_asteroid_explosion(float x, float y) {
    int particles_to_create = 15 + (rand() % 10); 
    int created = 0;
    
    for (int i = 0; i < MAX_PARTICLES && created < particles_to_create; i++) {
        if (!explosion_particles[i].active) {
            explosion_particles[i].x = x + ((rand() % 16) - 8);
            explosion_particles[i].y = y + ((rand() % 16) - 8);
            
            float angle = (rand() % 628) / 100.0f;
            float speed = 1.0f + (rand() % 20) / 10.0f; 
            
            explosion_particles[i].vx = cosf(angle) * speed;
            explosion_particles[i].vy = sinf(angle) * speed;
            
            explosion_particles[i].lifetime = 15 + (rand() % 25); 
            explosion_particles[i].active = true;
            created++;
        }
    }
}

void update_explosion(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (explosion_particles[i].active) {
            explosion_particles[i].x += explosion_particles[i].vx;
            explosion_particles[i].y += explosion_particles[i].vy;
            
            explosion_particles[i].vx *= 0.97f;
            explosion_particles[i].vy *= 0.97f;
            
            explosion_particles[i].lifetime--;
            if (explosion_particles[i].lifetime <= 0) {
                explosion_particles[i].active = false;
            }
            
            if (explosion_particles[i].x < 0) explosion_particles[i].x += SCALE_W;
            if (explosion_particles[i].x >= SCALE_W) explosion_particles[i].x -= SCALE_W;
            if (explosion_particles[i].y < 0) explosion_particles[i].y += SCALE_H;
            if (explosion_particles[i].y >= SCALE_H) explosion_particles[i].y -= SCALE_H;
        }
    }
    
    if (the_ship.exploding) {
        the_ship.explosion_timer--;
        if (the_ship.explosion_timer <= 0) {
            the_ship.exploding = false;
        }
    }
}

void draw_explosion(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (explosion_particles[i].active) {
            float life_percent = (float)explosion_particles[i].lifetime / 50.0f;
            
            if (life_percent > 0.7f) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
            } else if (life_percent > 0.5f) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);   
            } else if (life_percent > 0.2f) {
                SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);   
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);     
            }
            
            SDL_Rect particle = {
                (int)explosion_particles[i].x, 
                (int)explosion_particles[i].y,
                2, 
                2
            };
            SDL_RenderFillRect(renderer, &particle);
        }
    }
}

bool can_fire(void) {
    
    if (the_ship.overheated) {
        return false;
    }
    
    if (the_ship.fire_cooldown <= 0) {
        if (the_ship.burst_count < SHOTS_PER_BURST) {
            the_ship.fire_cooldown = FIRE_DELAY;  
            the_ship.burst_count++;
            return true;
        } else {
            the_ship.fire_cooldown = FIRE_COOLDOWN;
            the_ship.burst_count = 0;
            return false;
        }
    }
    return false;
}

void check_projectile_asteroid_collisions(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;
        
        for (int j = 0; j < MAX_ASTEROIDS; j++) {
            if (!asteroids[j].active) continue;
            
            float dx = projectiles[i].x - asteroids[j].x;
            float dy = projectiles[i].y - asteroids[j].y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance < asteroids[j].radius) {
                projectiles[i].active = false;
                split_asteroid(j);
                if (asteroids[j].radius >= 20.0f) {
                    the_ship.score += LARGE_ASTEROID_POINTS;
                } else if (asteroids[j].radius >= MIN_ASTEROID_RADIUS) {
                    the_ship.score += MEDIUM_ASTEROID_POINTS;
                } else {
                    the_ship.score += SMALL_ASTEROID_POINTS;
                }
                
                break;
            }
        }
    }
}


void split_asteroid(int index) {
    float x = asteroids[index].x;
    float y = asteroids[index].y;
    float radius = asteroids[index].radius / 2.0f;
    
    if (radius < MIN_ASTEROID_RADIUS) {
        create_asteroid_explosion(x, y); 
        the_ship.score += SMALL_ASTEROID_POINTS;
        asteroids[index].active = false;
        return;
    }
    
    int num_splits = 2 + (rand() % 2); 
    for (int i = 0; i < num_splits; i++) {
        float angle = (rand() % 628) / 100.0f;
        float speed = 0.5f + (rand() % 15) / 10.0f;
        
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed;
        
        vx += asteroids[index].vx * 0.3f;
        vy += asteroids[index].vy * 0.3f;
        
        float offset = radius * 0.5f;
        float new_x = x + cosf(angle) * offset;
        float new_y = y + sinf(angle) * offset;
        
        spawn_asteroid(new_x, new_y, vx, vy, radius);
    }
    
    if (radius >= 25.0f) {
        the_ship.score += LARGE_ASTEROID_POINTS;
    } else {
        the_ship.score += MEDIUM_ASTEROID_POINTS;
    }
    
    create_asteroid_explosion(x, y);
    
    asteroids[index].active = false;
}

void check_and_spawn_asteroids(void) {
    int active_count = 0;
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            active_count++;
        }
    }
    
    
    int min_asteroids = 3; 
    if (active_count < min_asteroids) {
        int asteroids_to_spawn = min_asteroids - active_count;
        
        for (int spawn = 0; spawn < asteroids_to_spawn; spawn++) {
            float x, y;
            int attempts = 0;
            
            do {
                if (rand() % 2) {
                    x = (rand() % 2) ? -30.0f : SCALE_W + 30.0f;
                    y = rand() % SCALE_H;
                } else {
                    x = rand() % SCALE_W;
                    y = (rand() % 2) ? -30.0f : SCALE_H + 30.0f;
                }
                attempts++;
            } while (hypotf(x - the_ship.x, y - the_ship.y) < 100.0f && attempts < 10);
            
            float vx = ((rand() % 100) - 50) / 50.0f;
            float vy = ((rand() % 100) - 50) / 50.0f;
            
            float radius;
            int size_roll = rand() % 100;
            if (size_roll < 20) {
                radius = 30.0f + (rand() % 15); 
            } else if (size_roll < 60) {
                radius = 20.0f + (rand() % 10); 
            } else {
                radius = 12.0f + (rand() % 8);  
            }
            
            spawn_asteroid(x, y, vx, vy, radius);
        }
    }
}

void update_ship(void) {
    if (the_ship.exploding) return;
    
    if (keyLeft) {
        the_ship.angle -= 0.15f; 
    }
    if (keyRight) {
        the_ship.angle += 0.15f; 
    }
    
    if (keyThrust) {
        float thrust = 0.2f; 
        the_ship.vx += sinf(the_ship.angle) * thrust;
        the_ship.vy += -cosf(the_ship.angle) * thrust;
        
        float max_speed = 4.0f;
        float current_speed = sqrtf(the_ship.vx * the_ship.vx + the_ship.vy * the_ship.vy);
        if (current_speed > max_speed) {
            the_ship.vx = (the_ship.vx / current_speed) * max_speed;
            the_ship.vy = (the_ship.vy / current_speed) * max_speed;
        }
    }
    
    the_ship.vx *= 0.98f;
    the_ship.vy *= 0.98f;
    
    the_ship.x += the_ship.vx;
    the_ship.y += the_ship.vy;
    
    if (the_ship.x < 0) the_ship.x += SCALE_W;
    if (the_ship.x >= SCALE_W) the_ship.x -= SCALE_W;
    if (the_ship.y < 0) the_ship.y += SCALE_H;
    if (the_ship.y >= SCALE_H) the_ship.y -= SCALE_H;
    
    if (the_ship.fire_cooldown > 0) {
        the_ship.fire_cooldown--;
    }
}

void update_fire_timer(void) {
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_SPACE] && !the_ship.overheated) {
        the_ship.continuous_fire_timer++;
        the_ship.overheat_cooldown = 0;
        
        if (the_ship.continuous_fire_timer >= 180) {
            the_ship.overheated = true;
            the_ship.overheat_cooldown = 0; 
            the_ship.continuous_fire_timer = 0;
            the_ship.burst_count = 0;
        }
    } else if (keys[SDL_SCANCODE_SPACE] && the_ship.overheated) {
        the_ship.overheat_cooldown = 0;
    } else if (!keys[SDL_SCANCODE_SPACE]) {
        if (the_ship.overheated) {
            the_ship.overheat_cooldown++;
            
            if (the_ship.overheat_cooldown >= 180) {
                the_ship.overheated = false;
                the_ship.overheat_cooldown = 0;
                the_ship.continuous_fire_timer = 0;
            }
        } else {
            if (the_ship.continuous_fire_timer > 0) {
                the_ship.continuous_fire_timer--;
                if (the_ship.continuous_fire_timer < 0) {
                    the_ship.continuous_fire_timer = 0;
                }
            }
        }
    }
}