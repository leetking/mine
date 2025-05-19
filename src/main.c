#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "model.h"
#include "timer.h"

#define WNGRIDS     10
#define HNGRIDS     10
#define NMINES      10
#define FPS         (60)
/* 这些根据屏幕分辨率调整 */
static int AGRID_EDGE   = 40;
static int WIDTH;
static int HEIGHT;
static int LINE_WIDTH   = 5;
static int PADDING      = 10;

#define TITLE   "扫雷 "VERSION

#define FORMAT_MAX  (1024)

#define MAX(x, y)   ((x)>(y)?(x):(y))
#define MIN(x, y)   ((x)>(y)?(y):(x))

static SDL_Window *win;
static SDL_Renderer *renderer;
static map_t *map;
static SDL_Color const bg  = {0xee, 0xee, 0xee, 0xff},
                 closegrid = {0x9f, 0xa2, 0x9c, 0xff},
                 opengrid  = {0xd3, 0xd7, 0xcf, 0xff};
static SDL_Texture *sprites;
static TTF_Font *mainfont;
/* 游戏状态 */
enum {
    GAME_QUIT,
    GAME_RUNNING,
    GAME_OVER,
    GAME_WIN,
} game_status = GAME_RUNNING;

static void Error(char const *format, ...);
static void Warn(char const *format, ...);
static int main_init(void);
static void main_render(SDL_Renderer *renderer);
static void render_map(SDL_Renderer *renderer, map_t *map);
static void main_processevents(void);
static int main_quit(void);
static void main_status_machine(SDL_Event e);
static SDL_Renderer *win_getrenderer(SDL_Window *win);
static int pixelpos2gridpos(int pxielx, int pxiely, int *row, int *col);
static void render_num(SDL_Renderer *renderer, int num, int row, int col);
static void render_mark(SDL_Renderer *renderer, mark_t mark, int row, int col);
static void render_gamewin(SDL_Renderer *renderer);
static void render_gameover(SDL_Renderer *renderer);
static void render_agrid(SDL_Renderer *renderer, map_t *map, int row, int col);

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
    while (GAME_QUIT != game_status) {
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
    va_list vars;
    va_start(vars, format);
    char buf[FORMAT_MAX];
    vsprintf_s(buf, FORMAT_MAX, format, vars);
    SDL_Log("Error: %s", buf);
    va_end(vars);
}
static void Warn(char const *format, ...)
{
    va_list vars;
    va_start(vars, format);
    char buf[FORMAT_MAX];
    vsprintf_s(buf, FORMAT_MAX, format, vars);
    SDL_Log("Warn: %s", buf);
    va_end(vars);
}
static int main_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    if ( !(IMG_INIT_PNG & IMG_Init(IMG_INIT_PNG)) ) {
        Error("Init IMG: %s\n", IMG_GetError());
        return 1;
    }
    if (-1 == TTF_Init()) {
        Error("Init TTF: %s\n", TTF_GetError());
        return 1;
    }
    SDL_DisplayMode display_mode;
    if (0 == SDL_GetCurrentDisplayMode(0, &display_mode)) {
        WIDTH = display_mode.w;
        HEIGHT = display_mode.h;
        /* 如果这是电脑，简单判断吧:)，因为电脑的w>h */
        if (WIDTH > HEIGHT) {
            WIDTH = 2*PADDING+(WNGRIDS-1)*(AGRID_EDGE+LINE_WIDTH)+AGRID_EDGE;
            HEIGHT = 2*PADDING+(HNGRIDS-1)*(AGRID_EDGE+LINE_WIDTH)+AGRID_EDGE;
        } else {
            /* 根据分辨率调整相应数据 */
            int l1, l2;
            l1 = WIDTH / ((WNGRIDS-1)*9+12);
            l2 = HEIGHT / ((HNGRIDS-1)*9+12);
            LINE_WIDTH = MIN(l1, l2);
            PADDING = 2*LINE_WIDTH;
            int agrid_edge1, agrid_edge2;
            agrid_edge1 = (WIDTH-2*PADDING-(WNGRIDS-1)*LINE_WIDTH)/WNGRIDS;
            agrid_edge2 = (HEIGHT-2*PADDING-(HNGRIDS-1)*LINE_WIDTH)/HNGRIDS;
            AGRID_EDGE = MIN(agrid_edge1, agrid_edge2);
            WIDTH = 2*PADDING+(WNGRIDS-1)*(AGRID_EDGE+LINE_WIDTH)+AGRID_EDGE;
            HEIGHT = 2*PADDING+(HNGRIDS-1)*(AGRID_EDGE+LINE_WIDTH)+AGRID_EDGE;
        }
    } else {
        Warn("Get Current Display Mode: %s\n", SDL_GetError());
    }
    win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = win_getrenderer(win);
    map = map_create(WNGRIDS, HNGRIDS, NMINES);
    sprites = IMG_LoadTexture(renderer, "resource/imgs/sprites.png");
    if (!sprites) Error("Load Sprites(%s): %s\n", "resource/imgs/sprites.png", IMG_GetError());
    mainfont = TTF_OpenFont("resource/fonts/DroidSansFallback.ttf", 20);
    if (!mainfont) Error("Open Font(%s): %s\n", "resource/fonts/DroidSansFallback.ttf", TTF_GetError());

    return 0;
}
static int main_quit(void)
{
    TTF_CloseFont(mainfont);
    SDL_DestroyTexture(sprites);
    map_destroy(map);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    TTF_Quit();
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
    *row = pxiely/(AGRID_EDGE+LINE_WIDTH);
    *col = pxielx/(AGRID_EDGE+LINE_WIDTH);
    if (*col >= WNGRIDS || *row >= HNGRIDS) return 1;
    /* 在线上 */
    if (AGRID_EDGE < pxiely%(AGRID_EDGE+LINE_WIDTH)) return 1;
    if (AGRID_EDGE < pxielx%(AGRID_EDGE+LINE_WIDTH)) return 1;
    return 0;
}

static void render_map(SDL_Renderer *renderer, map_t *map)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    int row, col;
    for (row = 0; row < map->h; ++row) {
        for (col = 0; col < map->w; ++col) {
            render_agrid(renderer, map, row, col);
        }
    }
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
static void main_render(SDL_Renderer *renderer)
{
    render_map(renderer, map);
    switch (game_status) {
    case GAME_OVER:
        render_gameover(renderer);
    case GAME_WIN:
        render_gamewin(renderer);
        break;
    default:
        break;
    }

    SDL_RenderPresent(renderer);
}
static void main_processevents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            game_status = GAME_QUIT;
            break;
        case SDL_MOUSEBUTTONDOWN:
            main_status_machine(e);
            break;
        }
    }
}
static void render_num(SDL_Renderer *renderer, int num, int row, int col)
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
    int x, y;
    x = PADDING+col*(AGRID_EDGE+LINE_WIDTH);
    y = PADDING+row*(AGRID_EDGE+LINE_WIDTH);
    double scale = 0.8*(double)AGRID_EDGE/MAX(num_.w, num_.h);
    SDL_Rect dst = {
        (int)(x+(AGRID_EDGE - scale*num_.w)/2),
        (int)(y+(AGRID_EDGE - scale*num_.h)/2),
        (int)(scale * num_.w),
        (int)(scale * num_.h),
    };
    SDL_RenderCopy(renderer, sprites, &numtextures[num], &dst);
}
static void render_gamewin(SDL_Renderer *renderer)
{
    (void)renderer;
}
static void render_gameover(SDL_Renderer *renderer)
{
    (void)renderer;
}
static void render_mark(SDL_Renderer *renderer, mark_t mark, int row, int col)
{
    int x, y;
    x = PADDING+col*(AGRID_EDGE+LINE_WIDTH);
    y = PADDING+row*(AGRID_EDGE+LINE_WIDTH);
    SDL_Rect marks[] = {
        { 0,   0,  0,  0}, /* not use */
        {90, 100, 30, 50}, /* flag */
        {60, 100, 30, 50}, /* maybe */
        { 0, 100, 30, 50}, /* mine normal */
        {30, 100, 30, 50}, /* exploded */
        { 0, 150, 30, 50}, /* incorrect */
    };
    SDL_Rect src = marks[mark];
    double scale = (double)AGRID_EDGE/MAX(src.w, src.h);
    SDL_Rect dst = {
        (int)(x+(AGRID_EDGE - scale*src.w)/2),
        (int)(y+(AGRID_EDGE - scale*src.h)/2),
        (int)(scale * src.w),
        (int)(scale * src.h),
    };
    SDL_RenderCopy(renderer, sprites, &src, &dst);
}
static void render_agrid(SDL_Renderer *renderer, map_t *map, int row, int col)
{
    SDL_Rect grid = {
        PADDING+col*(AGRID_EDGE+LINE_WIDTH), PADDING+row*(AGRID_EDGE+LINE_WIDTH),
        AGRID_EDGE, AGRID_EDGE,
    };
    SDL_Color color = map->grids[row][col].is_open ? opengrid : closegrid;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &grid);

    if (map->grids[row][col].is_open && (0 != map->grids[row][col].nmine)) {
        render_num(renderer, map->grids[row][col].nmine, row, col);
    }

    if (!map->grids[row][col].is_open && (MARK_EMPTY != map->grids[row][col].mark)) {
        render_mark(renderer, map->grids[row][col].mark, row, col);
    }
}
static void main_status_machine(SDL_Event e)
{
    int row, col;
    int status;
    if (0 != pixelpos2gridpos(e.motion.x, e.motion.y, &row, &col))
        return;
    switch (game_status) {
    case GAME_RUNNING:
        if (e.button.button == SDL_BUTTON_LEFT) {
            status = map_open(map, row, col);
            if (-1 == status) {
                printf("You touched a mine!\n");
                game_status = GAME_OVER;
            }
            if (1 == status) {
                printf("You are winner!\n");
                game_status = GAME_WIN;
            }
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
            status = map_mark(map, row, col);
        }
        break;
    default: break;
    }
}
