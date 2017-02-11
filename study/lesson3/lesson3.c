#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Timer.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

static SDL_Window *win;
static SDL_Surface *win_surf;
static SDL_Renderer *renderer;

static SDL_Texture *bg;

static int main_init(void);
static SDL_Texture *load_texture(char const *path);
static SDL_Surface *load_img(char const *path);
static int main_load_media(void);
static void main_quit(void);

static int main_init(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("lesson 3 - optimized surface", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) return 1;
    win_surf = SDL_GetWindowSurface(win);
    /*
     * 当使用集成显卡(intel)时，需要采用创建的方式
     * SDL_RENDERER_PRESENTVSYNC SDL_RenderPresent按照显示器(父表面?)刷新频率更新
     * 渲染: 3710 fps
     */
    //renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    /*
     * 当时用cpu渲染（而且不是intel的显卡），使用SDL_RENDERER_SOFTWARE
     * 渲染达到: 1890 fps
     */
    //renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    /*
     * 当我使用nvidia的显卡时，不能创建renderer器，只能获取？
     * 并且渲染达到: 8680 fps
     */
    //renderer = SDL_GetRenderer(win);
    if (!renderer) {
        printf("Create render: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        return 1;
    }
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Init image: %s\n", IMG_GetError());
        return 1;
    }
    return 0;
}
static SDL_Surface *load_img(char const *path)
{
    SDL_Surface *ret, *tmp;
    tmp = IMG_Load(path);
    if (!tmp) {
        printf("load image(%s): %s\n", path, IMG_GetError());
        return NULL;
    }
    ret = SDL_ConvertSurface(tmp, win_surf->format, 0);
    if (!ret) {
        printf("Optimie image(%s): %s\n", path, SDL_GetError());
        return tmp;
    }
    SDL_FreeSurface(tmp);
    return ret;
}
static SDL_Texture *load_texture(char const *path)
{
    SDL_Surface *surf = load_img(path);
    SDL_Texture *ret = SDL_CreateTextureFromSurface(renderer, surf);
    if (!ret) {
        printf("Create texture(%s): %s\n", path, SDL_GetError());
    }
    SDL_FreeSurface(surf);
    return ret;
}
static int main_load_media(void)
{
    int ret = 0;
    bg = load_texture("imgs/testing.png");
    return ret;
}
static void main_quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char **argv)
{
    if (0 != main_init()) return 1;
    long frame = 0;
    Timer fps;
    Timer update;

    /*
     * Apply the image stretched
     * SDL_Rect stretchRect;
     * stretchRect.x = offsetx;
     * stretchRect.y = offsety;
     * stretchRect.w = width;
     * stretchRect.h = height;
     * SDL_BlitScaled(stretchedSurface, NULL, screenSurface, &stretchRect);
     */
    main_load_media();
    Timer_start(&update);
    Timer_start(&fps);

    char isquit = 0;
    while (!isquit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            /* We only care event of quit */
            if (e.type == SDL_QUIT) isquit = 1;
        }

        /* render */
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bg, NULL, NULL);
        SDL_RenderPresent(renderer);

        ++frame;
        if (Timer_getticks(&update) > 1000) {
            char buf[512];
            snprintf(buf, sizeof(buf), "f: %ld\tfps: %.2f", frame, frame/(Timer_getticks(&fps)/1000.0));
            SDL_SetWindowTitle(win, buf);
            Timer_restart(&update);
        }
    }
    
    main_quit();
    return 0;
}

