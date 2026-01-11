/* coded by Jared Bruni */
#ifndef __SDL_H__
#define __SDL_H__

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

int initSDL(const char *app, int w, int h, int sx, int sy);
void printtext(const char *src, int x, int y);
void settextcolor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void releaseSDL(void);

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern TTF_Font *font;

#endif