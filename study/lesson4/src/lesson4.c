#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "dot.h"
#include "setting.h"


#define randf()         (rand()/(double)RAND_MAX)
#define ABS(x)          ((x)>0? (x): -(x))
#define MIN(x, y)       ((x)>(y)? (y): (x))
#define MAX(x, y)       ((x)>(y)? (x): (y))


#define DOTS_MAX    7


typedef struct transfer_t {
    double a, b, c, d;
} transfer_t;

typedef struct line_t {
    double x1, y1;
    double x2, y2;
} line_t;

typedef struct game_state_t {
    signed char quit, pause;
} game_state_t;

static SDL_Window *win;
static SDL_Renderer *renderer;
static Dot *dots[DOTS_MAX];

static int main_init(void);
static void main_quit(void);
static void main_update(int ms);
static void main_render(void);
static SDL_Texture *load_texture(char const *path);
static void main_collide(int ms);


static int main_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("lesson 4 - move dot",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        printf("Create win: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Create renderer: %s\n", SDL_GetError());
        return 1;
    }

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


static void main_render(void)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);
    for (int i = 0; i < DOTS_MAX; ++i) {
        dot_render(dots[i], renderer);
    }
    SDL_RenderPresent(renderer);
}


static void main_update(int ms)
{
    main_collide(ms);
    for (int i = 0; i < DOTS_MAX; ++i) {
        dot_move(dots[i], ms);
    }
}


static SDL_Texture *load_texture(char const *path)
{
    return IMG_LoadTexture(renderer, path);
}


static void transfer(transfer_t const *T,
        double x1, double y1, double *x2, double *y2)
{
    *x2 = T->a * x1 + T->b * y1;
    *y2 = T->c * x1 + T->d * y1;
}


static void main_collide(int _ms)
{
    struct {
        double x, y;
        double vx, vy;
    } nexts[DOTS_MAX];

    /* 解决穿墙的问题：
     * 一个物体很好解决
     * 多个物体呢，碰撞边界是直角型如何解决？
     */
    for (double ms = _ms, t = _ms; ms > 0; ms -= t) {
        for (int i = 0; i < DOTS_MAX; ++i) {
            nexts[i].x = dots[i]->x;
            nexts[i].y = dots[i]->y;
            nexts[i].vx = dots[i]->vx;
            nexts[i].vy = dots[i]->vy;
        }

        t = ms;
        for (int i = 0; i < DOTS_MAX; ++i) {
            double x0, y0, x1, y1;
            x0 = dots[i]->x;
            y0 = dots[i]->y;
            dot_next_pos(dots[i], ms, &x1, &y1);
            if (!(dots[i]->r <= x1 && x1 <= WINDOW_WIDTH-dots[i]->r)) {
                double dx = MIN(x0, WINDOW_WIDTH - x0) - dots[i]->r;
                ms = dx/ABS(dots[i]->vx);
                nexts[i].x = (dots[i]->vx <= 0)?
                    dots[i]->r: WINDOW_WIDTH-dots[i]->r;
                nexts[i].y += dots[i]->vy * ms;
                nexts[i].vx = -dots[i]->vx;
                nexts[i].vy = -dots[i]->vy;
            }
            if (!(dots[i]->r <= y1 && y1 <= WINDOW_HEIGHT-dots[i]->r)) {
            }
        }

        for (int i = 0; i < DOTS_MAX; ++i) {
            dots[i]->x = nexts[i].x;
            dots[i]->y = nexts[i].y;
            dots[i]->vx = nexts[i].vx;
            dots[i]->vy = nexts[i].vy;
        }
    }

    /* TODO 解决小球碰撞问题 */
}


static int process_event(game_state_t *s)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            s->quit = 1;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_SPACE)
                s->pause = !s->pause;
            break;
        }
    }

    return s->quit;
}


int main(int argc, char **argv)
{
    game_state_t state;
    main_init();

    srand((unsigned int)(time(NULL)+clock()));
    /* initialize dots */
    for (int i = 0; i < DOTS_MAX; ++i) {
        dots[i] = dot_new(load_texture("imgs/dot.bmp"));
        dots[i]->x = randf() * (WINDOW_WIDTH - dots[i]->r) + dots[i]->r/2.0;
        dots[i]->y = randf() * (WINDOW_HEIGHT - dots[i]->r) + dots[i]->r/2.0;
        dots[i]->vx = ((rand()%(2*MAX_V)) - MAX_V);
        dots[i]->vy = ((rand()%(2*MAX_V)) - MAX_V);
    }

    int fps = 0;
    int last = SDL_GetTicks();
    state.quit = state.pause = 0;
    main_render();
    while (!state.quit) {
        int t0 = SDL_GetTicks();
        process_event(&state);
        if (!state.pause) {
            main_update(1000.0/FPS);
        }
        main_render();
        ++fps;
        int t1 = SDL_GetTicks();

        /* 1 second */
        if (t1-last >= 1000) {
            char fps_str[512];
            sprintf(fps_str, "fps: %d", fps);
            SDL_SetWindowTitle(win, fps_str);
            SDL_Log("fps: %d\n", fps);

            fps = 0;
            last = t1;
        }

        if (1000.0/FPS > t1-t0)
            SDL_Delay(1000.0/FPS - (t1-t0));
    }
    /* Another game loop: https://gpp.tkchu.me/game-loop.html */

    main_quit();
    return 0;
}
