#ifndef DOT_H__
#define DOT_H__

#include <SDL2/SDL.h>

typedef struct Dot Dot;

Dot *dot_new(SDL_Texture *pic);
int dot_getX(Dot const *dot);
void dot_setX(Dot *dot, int x);
int dot_getY(Dot const *dot);
void dot_setY(Dot *dot, int y);
int dot_getVx(Dot const *dot);
void dot_setVx(Dot *dot, int vx);
int dot_getVy(Dot const *dot);
void dot_setVy(Dot *dot, int vy);

void dot_move(Dot *dot);
void dot_show(Dot *dot, SDL_Renderer *renderer);
void dot_free(Dot *dot);

struct Dot {
    double x, y;
    /* vx, vy单位 pxiel per second */
    double vx, vy;
    SDL_Texture *pic;
    int w, h;
};

#endif
