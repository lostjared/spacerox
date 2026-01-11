/* coded by Jared Bruni */
#include "sdl.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
TTF_Font *font = NULL;

const char *getPath(const char *src) {
    static char path[4096];
#ifndef __EMSCRIPTEN__
    snprintf(path, 4095, "%s%s", "./assets/", src);
#else
    snprintf(path, 4095, "/assets/%s", src);
#endif
    return path;
}

int initSDL(const char *app, int w, int h, int sx, int sy) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error could not open SDL: %s", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow(app, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sx, sy, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(!window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        fprintf(stderr, "Error render device failed to init: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    SDL_Surface *ico = SDL_LoadBMP(getPath("icon.bmp"));
    if(!ico) {
        fprintf(stderr, "Could not open icon.bmp: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    SDL_SetWindowIcon(window, ico);
    SDL_FreeSurface(ico);
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!texture) {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    if(TTF_Init() < 0) {
        fprintf(stderr, "Could not initalize fonts.\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    font = TTF_OpenFont(getPath("font.ttf"), 16);
    if(!font) {
        fprintf(stderr, "Error opening font: %s Reason: %s\n", "font.ttf", TTF_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 0;
    }
    return 1;
}

void releaseSDL(void) {
    if(font) TTF_CloseFont(font);
    TTF_Quit();
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Color textColor;

void settextcolor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    textColor.r = r;
    textColor.g = g;
    textColor.b = b;
    textColor.a = a;
}

void printtext(const char *src, int x, int y) {
    if (!font || !renderer) return;
    SDL_Surface *surface = TTF_RenderText_Blended(font, src, textColor);
    if (!surface) return;
    int text_w = surface->w;
    int text_h = surface->h;
    SDL_Texture *textTex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); 
    if (!textTex) return;
    SDL_Rect dst = {x, y, text_w, text_h};
    SDL_RenderCopy(renderer, textTex, NULL, &dst);
    SDL_DestroyTexture(textTex);
}
