#include <stdlib.h>
#include <SDL2/SDL.h>
#include "dot.h"
#include "setting.h"


Dot *dot_new(SDL_Texture *pic)
{
    Dot *ret = malloc(sizeof(Dot));
    ret->pic = pic;
    ret->r = 0;
    ret->vx = ret->vy = 0;
    ret->x = ret->y = 0;
    if (pic) {
        SDL_QueryTexture(pic, NULL, NULL, &ret->r, NULL);
    }
    return ret;
}


void dot_move(Dot *dot, double ms)
{
    dot->x += dot->vx * ms/1000.0;
    dot->y += dot->vy * ms/1000.0;
}


void dot_next_pos(Dot *dot, double ms, double *x, double *y)
{
    *x = dot->x + dot->vx * ms/1000.0;
    *y = dot->y + dot->vy * ms/1000.0;
}


void dot_render(Dot const *dot, SDL_Renderer *renderer)
{
    SDL_Rect pos;
    pos.x = (dot->x - dot->r/2) + 0.5;
    pos.y = (dot->y - dot->r/2) + 0.5;
    pos.w = pos.h = dot->r;
    SDL_RenderCopy(renderer, dot->pic, NULL, &pos);
}


void dot_free(Dot *dot)
{
    if (dot->pic) SDL_DestroyTexture(dot->pic);
    free(dot);
}
