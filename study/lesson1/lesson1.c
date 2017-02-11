#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

int main(int argc, char **argv)
{
    SDL_Window *win;
    SDL_Surface *surface;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init: %s\n", SDL_GetError());
        return 1;
    }

    win = SDL_CreateWindow("你好SDL!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        printf("create window: %s\n", SDL_GetError());
        return 1;
    }

    surface = SDL_GetWindowSurface(win);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0xff));
    SDL_UpdateWindowSurface(win);
    
    int isquit = 0;
    while (!isquit) {
        SDL_Event e;
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) isquit = 1;
    }

    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}


