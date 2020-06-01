#ifndef DOT_H__
#define DOT_H__

#include <SDL2/SDL.h>

typedef struct Dot Dot;

Dot *dot_new(SDL_Texture *pic);
void dot_next_pos(Dot *dot, double ms, double *x, double *y);
void dot_move(Dot *dot, double ms);
void dot_render(Dot const *dot, SDL_Renderer *renderer);
void dot_free(Dot *dot);

struct Dot {
    double x, y;    /* 圆心坐标 */
    double vx, vy;  /* vx, vy单位 pxiel per second */
    double weight;

    SDL_Texture *pic;
    int r;
};

#endif
