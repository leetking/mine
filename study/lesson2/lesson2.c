#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

enum {
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

static SDL_Window *win;
static SDL_Surface *press_surfaces[KEY_PRESS_SURFACE_TOTAL];

static int main_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("按键测试", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) return 1;
    return 0;
}
static void main_quit()
{
    SDL_DestroyWindow(win);
    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
        SDL_FreeSurface(press_surfaces[i]);
    }
    SDL_Quit();
}
static SDL_Surface *load_surface(char const *path)
{
    SDL_Surface *s = SDL_LoadBMP(path);
    if (!s) printf("load file(%s): %s\n", path, SDL_GetError());
    return s;
}
static int main_load_media(void)
{
    int ret = 0;
    char const *paths[] = {
        "imgs/press.bmp",
        "imgs/up.bmp", "imgs/down.bmp", "imgs/left.bmp", "imgs/right.bmp",
    };
    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
        press_surfaces[i] = load_surface(paths[i]);
        if (!press_surfaces[i]) ret = 1;
    }

    return ret;
}

int main(int argc, char **argv)
{
    main_init();
    main_load_media();
    int isquit = 0;
    SDL_Event e;
    SDL_Surface *current_surface = press_surfaces[KEY_PRESS_SURFACE_DEFAULT];
    while (!isquit) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) isquit = 1;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_UP:
                current_surface = press_surfaces[KEY_PRESS_SURFACE_UP];
                break;
            case SDLK_DOWN:
                current_surface = press_surfaces[KEY_PRESS_SURFACE_DOWN];
                break;
            case SDLK_LEFT:
                current_surface = press_surfaces[KEY_PRESS_SURFACE_LEFT];
                break;
            case SDLK_RIGHT:
                current_surface = press_surfaces[KEY_PRESS_SURFACE_RIGHT];
                break;
            default:
                current_surface = press_surfaces[KEY_PRESS_SURFACE_DEFAULT];
                break;
            }
        }
        SDL_BlitSurface(current_surface, NULL, SDL_GetWindowSurface(win), NULL);
        SDL_UpdateWindowSurface(win);
    }
    main_quit();
    return 0;
}


