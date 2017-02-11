#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "timer.h"
#include "dot.h"
#include "setting.h"

#define DOTS_MAX    3

static SDL_Window *win;
static SDL_Renderer *renderer;
static Dot *dots[DOTS_MAX];

static int main_init(void);
static void main_quit(void);
static void main_render(SDL_Renderer *renderer);
static void main_run();
static SDL_Texture *load_texture(char const *path);
static void main_collide(void);

static int main_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("lesson 4 - move dot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        printf("Create win: %s\n", SDL_GetError());
        return 1;
    }
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Create renderer: %s\n", SDL_GetError());
        return 1;
    }
    /* 设置默认render颜色 */
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Init image: %s\n", IMG_GetError());
        return 1;
    }

    return 0;
}
static void main_quit(void)
{
    for (int i = 0; i < DOTS_MAX; ++i) {
        dot_free(dots[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}
static void main_render(SDL_Renderer *renderer)
{
    SDL_RenderClear(renderer);
    for (int i = 0; i < DOTS_MAX; ++i) {
        dot_show(dots[i], renderer);
    }
    SDL_RenderPresent(renderer);
}
static void main_run()
{
    for (int i = 0; i < DOTS_MAX; ++i) {
        dot_move(dots[i]);
    }
    main_collide();
}
static SDL_Texture *load_texture(char const *path)
{
    SDL_Surface *pic;
    pic = SDL_LoadBMP(path);
    if (!pic) {
        printf("Load Bmp(%s): %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_SetColorKey(pic, SDL_TRUE, SDL_MapRGB(pic->format, 0xff, 0xff, 0xff));
    SDL_Texture *ret;
    ret = SDL_CreateTextureFromSurface(renderer, pic);
    if (!ret) {
        printf("Create texutre(%s): %s\n", path, SDL_GetError());
    }
    return ret;
}
static void main_collide(void)
{
    /* TODO 碰撞检测有问题! */
#define POW(x)  ((x)*(x))
    for (int i = 0; i < DOTS_MAX; ++i) {
        for (int j = i+1; j < DOTS_MAX; ++j) {
            if ( (POW(dots[i]->x - dots[j]->x) + POW(dots[i]->y - dots[j]->y))
                    < POW(dots[i]->w)) {
                double dvi = sqrt(POW(dots[i]->vx) + POW(dots[i]->vy));
                double dvj = sqrt(POW(dots[j]->vx) + POW(dots[j]->vy));
                double vjx_1 = dvi/dvj*dots[j]->vx;
                double vjy_1 = dvi/dvj*dots[j]->vy;
                double vx = (dots[i]->vx/2 + vjx_1/2);
                double vy = (dots[i]->vy/2 + vjy_1/2);
                double dv2 = vx*vx + vy*vy;
                double sinx2 = vx*vx/dv2;
                double cosx2 = vy*vy/dv2;
                double sinxcosx = vx*vy/dv2;
                double a = dots[i]->vx;
                double b = dots[i]->vy;
                dots[i]->vx =  a*sinx2 - 2*b*sinxcosx - a*cosx2;
                dots[i]->vy = -b*sinx2 - 2*a*sinxcosx + b*cosx2;
                a = dots[j]->vx;
                b = dots[j]->vy;
                dots[j]->vx =  a*sinx2 - 2*b*sinxcosx - a*cosx2;
                dots[j]->vy = -b*sinx2 - 2*a*sinxcosx + b*cosx2;
            }
        }
    }
#undef pow
}

int main(int argc, char **argv)
{
    main_init();
    Timer fps, update;
    Timer spf;
    long frame = 0;

    srand((unsigned int)(time(NULL)+clock()));
    for (int i = 0; i < DOTS_MAX; ++i) {
        /* init dot */
        dots[i] = dot_new(load_texture("imgs/dot.bmp"));
        dots[i]->x = rand()%SCREEN_WIDTH;
        dots[i]->y = rand()%SCREEN_HEIGHT;
        dots[i]->vx = ((rand()%(2*MAX_V)) - MAX_V);
        dots[i]->vy = ((rand()%(2*MAX_V)) - MAX_V);
    }

    Timer_start(&fps);
    Timer_start(&update);

    int isquit = 0;
    int ispause = 0;
    while (!isquit) {
        Timer_restart(&spf);
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) isquit = 1;
            if ( (e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_SPACE)) {
                ispause = !ispause;
            }
        }

        if (!ispause) {
            main_run();
            main_render(renderer);
        }

        if (Timer_getticks(&update) > 1000) {
            char title[512];
            snprintf(title, sizeof(title), "fps: %.2f", frame / (Timer_getticks(&fps)/1000.0));
            SDL_SetWindowTitle(win, title);
            Timer_restart(&update);
        }
        ++frame;
        int sleeptime = (1000.0/FPS - Timer_getticks(&spf));
        if (sleeptime > 0) {
            SDL_Delay(sleeptime);
        }
    }

    main_quit();
    return 0;
}


