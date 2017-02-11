#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "model.h"
#include "timer.h"

#define WNGRIDS     10
#define HNGRIDS     10
#define NMINES      20
#define AGRID_EDGE  40
#define PADDING     10
#define WIDTH       (2*PADDING+WNGRIDS*(AGRID_EDGE+1))
#define HEIGHT      (2*PADDING+HNGRIDS*(AGRID_EDGE+1))
#define FPS         (60)

#define FORMAT_MAX  (512)

#define TITLE   "扫雷 SDL2 "VERSION

#define MAX(x, y)   ((x)>(y)?(x):(y))

static SDL_Window *win;
static SDL_Renderer *renderer;
static map_t *map;
static int isquit = 0;
static SDL_Color bg        = {0xee, 0xee, 0xee, 0xff},
                 closegrid = {0x9f, 0xa2, 0x9c, 0xff},
                 opengrid  = {0xd3, 0xd7, 0xcf, 0xff};
static SDL_Texture *sprites;

static void Error(char const *format, ...);
static void Warn(char const *format, ...);
static int main_init(void);
static void main_render(SDL_Renderer *renderer);
static void render_map(SDL_Renderer *renderer);
static void main_processevents(void);
static int main_quit(void);
static SDL_Renderer *win_getrenderer(SDL_Window *win);
static int pixelpos2gridpos(int pxielx, int pxiely, int *gridx, int *gridy);
static void render_num(SDL_Renderer *renderer, int num, int x, int y);

int main(int argc, char **argv)
{
    if ( (2 == argc) && !strcmp("-h", argv[1])) {
        printf("%s: v%s (GPL v3) leetking <li_Tking@163.com>\n", argv[0], VERSION);
        return 0;
    }
    main_init();

    Timer update;
    Timer fps;
    long frames = 0;
    Timer_start(&update);
    Timer_start(&fps);
    /* main loop */
    while (!isquit) {
        /* process events */
        main_processevents();
        /* render */
        main_render(renderer);
        ++frames;

        int sleeptime = (int)(1000.0/FPS - Timer_getticks(&update));
        if (sleeptime >= 0) SDL_Delay(sleeptime);
        Timer_restart(&update);
    }

    return  main_quit();
}



static void Error(char const *format, ...)
{
    char tmp[FORMAT_MAX];
    snprintf(tmp, FORMAT_MAX, "Error: %s", format);
    va_list vars;
    va_start(vars, format);
    vprintf(tmp, vars);
    va_end(vars);
}
static void Warn(char const *format, ...)
{
    char tmp[FORMAT_MAX];
    snprintf(tmp, FORMAT_MAX, "Warn: %s", format);
    va_list vars;
    va_start(vars, format);
    vprintf(tmp, vars);
    va_end(vars);
}
static int main_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    if ( !(IMG_INIT_PNG & IMG_Init(IMG_INIT_PNG)) ) {
        Error("Init IMG: %s\n", IMG_GetError());
        return 1;
    }
    win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = win_getrenderer(win);
    map = map_create(WNGRIDS, HNGRIDS, NMINES);
    sprites = IMG_LoadTexture(renderer, "resource/imgs/sprites.png");

    return 0;
}
static int main_quit(void)
{
    map_destroy(map);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

static SDL_Renderer *win_getrenderer(SDL_Window *win)
{
    SDL_Renderer *ret;
    ret = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ret) return ret;

    Warn("Create Renderer: %s\n", SDL_GetError());
    /*
     * 对于非集成显卡貌似只能获取renderer
     * 对于Ubuntu 16.04LTS、GTX 850(nvidia-367)是这样的
     */
    ret = SDL_GetRenderer(win);
    if (ret) return ret;

    Warn("Get Renderer: %s\n", SDL_GetError());
    /* 最后尝试软renderer */
    ret = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!ret) Error("Create Soft Renderer: %s\n", SDL_GetError());

    return ret;
}
static int pixelpos2gridpos(int pxielx, int pxiely, int *row, int *col)
{
    if (pxielx < PADDING || pxiely < PADDING) return 1;
    pxielx -= PADDING;
    pxiely -= PADDING;
    *row = pxiely/AGRID_EDGE;
    *col = pxielx/AGRID_EDGE;
    if (*col >= WNGRIDS || *row >= HNGRIDS) return 1;
    return 0;
}

static void render_map(SDL_Renderer *renderer)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    /*
    SDL_SetRenderDrawColor(renderer, gridr, gridg, gridb, 0xff);
    SDL_Rect colormap;
    colormap.x = PADDING; colormap.y = PADDING;
    colormap.w = WNGRIDS*AGRID_EDGE; colormap.h = HNGRIDS*AGRID_EDGE;
    SDL_RenderFillRect(renderer, &colormap);

    SDL_SetRenderDrawColor(renderer, bgr, bgg, bgb, 0xff);
    int i, j;
    for (i = 0; i <= WNGRIDS; ++i) {
        SDL_RenderDrawLine(renderer, PADDING+i*AGRID_EDGE, PADDING,
                PADDING+i*AGRID_EDGE, HEIGHT-PADDING);
    }
    for (i = 0; i <= WNGRIDS; ++i) {
        SDL_RenderDrawLine(renderer, PADDING, PADDING+i*AGRID_EDGE,
                WIDTH-PADDING, PADDING+i*AGRID_EDGE);
    }
    */

    int i, j;
    SDL_Rect grid;
    SDL_Color color;
    for (i = 0; i < map->h; ++i) {
        for (j = 0; j < map->w; ++j) {
            grid.x = PADDING+j*(AGRID_EDGE+1);
            grid.y = PADDING+i*(AGRID_EDGE+1);
            grid.w = grid.h = AGRID_EDGE;
            color = map->grids[i][j].is_open ? opengrid : closegrid;
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &grid);
            if (map->grids[i][j].is_open && (0 != map->grids[i][j].nmine)) {
                render_num(renderer, map->grids[i][j].nmine, grid.x, grid.y);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
static void main_render(SDL_Renderer *renderer)
{
    render_map(renderer);

    SDL_RenderPresent(renderer);
}
static void main_processevents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            isquit = 1;
            break;
        case SDL_MOUSEBUTTONDOWN:
            {
                int row, col;
                int status;
                if (0 != pixelpos2gridpos(e.motion.x, e.motion.y, &row, &col))
                    break;
                status = map_open(map, row, col);
                if (-1 == status) {
                    printf("You touched a mine!\n");
                    isquit = 1;
                }
                if (1 == status) {
                    printf("You are winner!\n");
                    isquit = 1;
                }
            }
            break;
        }
    }
}
static void render_num(SDL_Renderer *renderer, int num, int x, int y)
{
    SDL_Rect numtextures[] = {
        { 0,  0,  0,  0},   /* not use */
        { 0,  0, 30, 50},   /* 1 */
        {30,  0, 30, 50},
        {60,  0, 30, 50},
        {90,  0, 30, 50},
        { 0, 50, 30, 50},
        {30, 50, 30, 50},
        {60, 50, 30, 50},
        {90, 50, 30, 50},   /* 8 */
    };
    SDL_Rect num_ = numtextures[num];
    double scale = 0.8*(double)AGRID_EDGE/MAX(num_.w, num_.h);
    SDL_Rect dst = {
        x+(AGRID_EDGE - scale*num_.w)/2,
        y+(AGRID_EDGE - scale*num_.h)/2,
        scale * num_.w,
        scale * num_.h,
    };
    SDL_RenderCopy(renderer, sprites, &numtextures[num], &dst);
}
