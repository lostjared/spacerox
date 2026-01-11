/* coded by Jared Bruni */
#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

#include"sdl.h"
#include"ship.h"
#include<math.h> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_W 640
#define WINDOW_H 360
#define WINDOW_SX 1280
#define WINDOW_SY 720
#define STAR_COUNT 300

int initialized = 0;
struct { float x, y, speed; } stars[STAR_COUNT];

int countdown_sequence = 1; 
int countdown_timer = 0;
int countdown_duration = 60; 
int countdown_number = 3;
int launch_sequence = 0; 
int launch_timer = 0;
int launch_duration = 60; 
int ship_launch_y = 0;

int game_over = 0;

void update(void);
void render(void);
void draw_launch_sequence(void);
void draw_countdown_sequence(void);
void trigger_respawn_sequence(void);
void draw_score(void);
void draw_game_over(void);
void restart_game(void);

SDL_Event e;
int active = 1;

int main(int argc, char **argv) {
    if(!initSDL("[space]", WINDOW_W, WINDOW_H, WINDOW_SX, WINDOW_SY)) {
        fprintf(stderr, "Failed to init..\n");
        return 1;
    }

    init_ship();
    init_projectiles();
    init_asteroids();
    
    countdown_sequence = 1;
    countdown_timer = 0;
    countdown_number = 3;
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update, 0, 1);
#else
    while(active) {
        update();
    }
#endif
    releaseSDL();
    return 0;
}   

void restart_game(void) {
    init_ship();
    init_projectiles();
    init_asteroids();
    game_over = 0;
    countdown_sequence = 1;
    countdown_timer = 0;
    countdown_number = 3;
    launch_sequence = 0;
}

void trigger_respawn_sequence(void) {
    the_ship.x = SCALE_W / 2.0f;
    the_ship.y = SCALE_H / 2.0f;
    the_ship.vx = 0.0f;
    the_ship.vy = 0.0f;
    the_ship.angle = 0.0f;
    the_ship.exploding = false;
    the_ship.explosion_timer = 0;
    
    countdown_sequence = 1;
    countdown_timer = 0;
    countdown_number = 3;
    launch_sequence = 0;
}

void update(void) {
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_QUIT:
                active = 0;
                continue;
                
            case SDL_KEYDOWN:
                if(e.key.keysym.sym == SDLK_ESCAPE) 
                    active = 0;
                
                if(e.key.keysym.sym == SDLK_SPACE && game_over) {
                    restart_game();
                    return;
                }
                
                if (!launch_sequence && !countdown_sequence && !game_over) {
                    if(e.key.keysym.sym == SDLK_LEFT)
                        keyLeft = true;
                    if(e.key.keysym.sym == SDLK_RIGHT)
                        keyRight = true;
                    if(e.key.keysym.sym == SDLK_UP)
                        keyThrust = true;
                }
                break;
                
            case SDL_KEYUP:
                if (!launch_sequence && !countdown_sequence && !game_over) {
                    if(e.key.keysym.sym == SDLK_LEFT)
                        keyLeft = false;
                    if(e.key.keysym.sym == SDLK_RIGHT)
                        keyRight = false;
                    if(e.key.keysym.sym == SDLK_UP)
                        keyThrust = false;
                }
                break;                          
        }
    }
    
    if (game_over) {
        render();
        return;
    }
    
    
    if (!launch_sequence && !countdown_sequence) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_SPACE] && can_fire()) {
            fire_projectile(the_ship.x, the_ship.y, the_ship.angle);
        }
    }
    
    if (countdown_sequence) {
        countdown_timer++;
        
        if (countdown_timer >= countdown_duration) {
            countdown_timer = 0;
            countdown_number--;
            
            if (countdown_number < 0) {
                countdown_sequence = 0;
                launch_sequence = 1;
                launch_timer = 0;
                ship_launch_y = WINDOW_H;
            }
        }
        
        render();
        return;
    }
    
    if (launch_sequence) {
        launch_timer++;
        
        if (launch_timer < launch_duration / 2) {
            ship_launch_y = WINDOW_H - (launch_timer * (WINDOW_H - the_ship.y) / (launch_duration / 2));
        } else {
            ship_launch_y = the_ship.y; 
        }
        
        if (launch_timer >= launch_duration) {
            launch_sequence = 0;
            launch_timer = 0;
        }
        
        render();
        return;
    }
    
    update_ship();
    update_fire_timer();
    update_projectiles();
    update_asteroids();
    update_explosion();
    
    check_asteroid_collisions();        
    check_ship_asteroid_collision();    
    check_projectile_asteroid_collisions(); 
    
    check_and_spawn_asteroids();
    
    static bool was_exploding = false;
    if (was_exploding && !the_ship.exploding) {
        if (the_ship.lives > 0) {
            trigger_respawn_sequence(); 
        } else {
            game_over = 1;
        }
        was_exploding = false;
    } else if (the_ship.exploding) {
        was_exploding = true;
    }
    
    render();
}

void draw_countdown_sequence(void) {
    if (!initialized) {
        for (int i = 0; i < STAR_COUNT; ++i) {
            stars[i].x = rand() % WINDOW_W;
            stars[i].y = rand() % WINDOW_H;
            stars[i].speed = 0.5f + (rand() % 100) / 50.0f; 
        }
        initialized = 1;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < STAR_COUNT; ++i) {
        stars[i].y += stars[i].speed * 0.3f; 
        if (stars[i].y >= WINDOW_H) {
            stars[i].x = rand() % WINDOW_W;
            stars[i].y = 0;
            stars[i].speed = 0.5f + (rand() % 100) / 50.0f;
        }
        SDL_RenderDrawPoint(renderer, (int)stars[i].x, (int)stars[i].y);
    }
    char countdown_text[16];
    if (countdown_number > 0) {
        snprintf(countdown_text, 16, "%d", countdown_number);
        settextcolor(255, 255, 0, 255); 
        printtext(countdown_text, 315, 170);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);
        int flash = (countdown_timer / 10) % 2;
        if (flash) {
            SDL_Rect highlight = {300, 160, 40, 40};
            SDL_RenderDrawRect(renderer, &highlight);
        }
    } else {
        settextcolor(0, 255, 0, 255); 
        printtext("LAUNCH!", 280, 170);
    }
    settextcolor(255, 255, 255, 255);
    printtext("PREPARE FOR MISSION", 240, 200);
    
    char lives_text[32];
    snprintf(lives_text, 32, "Lives: %d", the_ship.lives);
    printtext(lives_text, 270, 220);
    
    char score_text[32];
    snprintf(score_text, 32, "Score: %d", the_ship.score);
    printtext(score_text, 270, 240);
}

void draw_launch_sequence(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < STAR_COUNT; ++i) {
        stars[i].y += stars[i].speed * 0.5f; 
        if (stars[i].y >= WINDOW_H) {
            stars[i].x = rand() % WINDOW_W;
            stars[i].y = 0;
            stars[i].speed = 0.5f + (rand() % 100) / 50.0f;
        }
        SDL_RenderDrawPoint(renderer, (int)stars[i].x, (int)stars[i].y);
    }
    
    if (launch_timer > launch_duration / 4) {
        draw_asteroids();
    }
    
    float temp_y = the_ship.y;
    the_ship.y = ship_launch_y;
    draw_ship();
    the_ship.y = temp_y; 
    
    if (launch_timer < launch_duration / 2) {
        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); 
        for (int i = 0; i < 12; i++) {
            int thrust_x = the_ship.x + (rand() % 8) - 4;
            int thrust_y = ship_launch_y + 15 + i * 2;
            if (thrust_y < WINDOW_H) {
                SDL_RenderDrawPoint(renderer, thrust_x, thrust_y);
            }
        }
    }
    
    if (launch_timer >= launch_duration / 2) {
        settextcolor(0, 255, 255, 255); 
        printtext("MISSION START!", 250, 180);
    }
}

void draw_game_over(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < STAR_COUNT; ++i) {
        stars[i].y += stars[i].speed * 0.2f; 
        if (stars[i].y >= WINDOW_H) {
            stars[i].x = rand() % WINDOW_W;
            stars[i].y = 0;
            stars[i].speed = 0.5f + (rand() % 100) / 50.0f;
        }
        SDL_RenderDrawPoint(renderer, (int)stars[i].x, (int)stars[i].y);
    }
    
    settextcolor(255, 0, 0, 255); 
    printtext("GAME OVER", 270, 140);
    
    char final_score[64];
    snprintf(final_score, 64, "Final Score: %d", the_ship.score);
    settextcolor(255, 255, 255, 255); 
    printtext(final_score, 250, 170);
    
    settextcolor(255, 255, 0, 255); 
    printtext("Press SPACE to begin", 230, 200);
}

void render(void) {
    static Uint32 last_time = 0;
    Uint32 current_time = SDL_GetTicks();
    
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    if (game_over) {
        draw_game_over();
    } else if (countdown_sequence) {
        draw_countdown_sequence();
    } else if (launch_sequence) {
        draw_launch_sequence();
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < STAR_COUNT; ++i) {
            stars[i].y += stars[i].speed;
            if (stars[i].y >= WINDOW_H) {
                stars[i].x = rand() % WINDOW_W;
                stars[i].y = 0;
                stars[i].speed = 0.5f + (rand() % 100) / 50.0f;
            }
            SDL_RenderDrawPoint(renderer, (int)stars[i].x, (int)stars[i].y);
        }

        draw_ship();
        draw_projectiles();
        draw_asteroids();
        draw_explosion();
        draw_score(); 
    }
    SDL_SetRenderTarget(renderer, NULL);
    int window_w, window_h;
    SDL_GetRendererOutputSize(renderer, &window_w, &window_h);
    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);
    float window_aspect = (float)window_w / (float)window_h;
    float texture_aspect = (float)tex_w / (float)tex_h;
    SDL_Rect dst;
    if (texture_aspect > window_aspect) {
        dst.w = window_w;
        dst.h = (int)(window_w / texture_aspect);
        dst.x = 0;
        dst.y = (window_h - dst.h) / 2;
    } else {
        dst.h = window_h;
        dst.w = (int)(window_h * texture_aspect);
        dst.y = 0;
        dst.x = (window_w - dst.w) / 2;
    }
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);
    const Uint32 target_frame_time = 1000 / 60; 
    Uint32 frame_time = current_time - last_time;
    if (frame_time < target_frame_time) {
        SDL_Delay(target_frame_time - frame_time);
    }
    last_time = SDL_GetTicks();
}

void draw_score(void) {
    char score_text[32];
    snprintf(score_text, 32, "Score: %d", the_ship.score);
    settextcolor(255, 255, 255, 255); 
    printtext(score_text, 10, 10);    
    char lives_text[32];
    snprintf(lives_text, 32, "Lives: %d", the_ship.lives);
    printtext(lives_text, 10, 30);  
}