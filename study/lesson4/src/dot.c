#include <stdlib.h>
#include <SDL2/SDL.h>
#include "dot.h"
#include "setting.h"

Dot *dot_new(SDL_Texture *pic)
{
    Dot *ret = malloc(sizeof(Dot));
    ret->pic = pic;
    if (pic) {
        SDL_QueryTexture(pic, NULL, NULL, &ret->w, &ret->h);
    }
    return ret;
}
int dot_getX(Dot const *dot)
{
    return (int)dot->x;
}
void dot_setX(Dot *dot, int x)
{
    dot->x = x;
}
int dot_getY(Dot const *dot)
{
    return (int)dot->y;
}
void dot_setY(Dot *dot, int y)
{
    dot->y = y;
}
void dot_setVx(Dot *dot, int vx)
{
    dot->vx = vx;
}
int dot_getVx(Dot const *dot)
{
    return dot->vx;
}
void dot_setVy(Dot *dot, int vy)
{
    dot->vy = vy;
}
int dot_getVy(Dot const *dot)
{
    return dot->vy;
}

void dot_move(Dot *dot)
{
    dot->x += dot->vx/FPS;
    dot->y += dot->vy/FPS;

    /* 碰到墙反弹 */
    if ((dot->x - dot->w/2 <= 0) || (dot->x + dot->w/2 >= SCREEN_WIDTH)) {
        dot->vx = -dot->vx;
    }
    if ((dot->y - dot->h/2 <= 0) || (dot->y + dot->h/2 >= SCREEN_HEIGHT)) {
        dot->vy = -dot->vy;
    }
}
void dot_show(Dot *dot, SDL_Renderer *renderer)
{
    SDL_Rect pos;
    pos.x = dot->x - dot->w/2;
    pos.y = dot->y - dot->h/2;
    pos.w = dot->w;
    pos.h = dot->h;
    SDL_RenderCopy(renderer, dot->pic, NULL, &pos);
}

void dot_free(Dot *dot)
{
    if (dot->pic) SDL_DestroyTexture(dot->pic);
    free(dot);
}
