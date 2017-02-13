#include "timer.h"
#include <SDL.h>

#undef NOTNULL
#define NOTNULL(x) do { if (!(x)) return; } while (0)

void Timer_start(Timer *t)
{
    NOTNULL(t);
    t->startticks = SDL_GetTicks();
    t->isstart = 1;
    t->ispause = 0;
    t->pauseticks = 0;
}
void Timer_stop(Timer *t)
{
    NOTNULL(t);
    t->isstart = 0;
}
void Timer_pause(Timer *t)
{
    NOTNULL(t);
    if (t->isstart && !t->ispause) {
        t->ispause = 1;
        t->pauseticks = SDL_GetTicks() - t->startticks;
    }
}
void Timer_unpause(Timer *t)
{
    NOTNULL(t);
    if (t && t->ispause) {
        t->ispause = 0;
        t->startticks = SDL_GetTicks() - t->pauseticks;
        t->pauseticks = 0;
    }
}
/*
 * 重新开始计时器，并返回流逝的时间
 * @return: 流逝的时间
 */
int Timer_restart(Timer *t)
{
    int elapse = Timer_getticks(t);
    Timer_start(t);
    return elapse;
}
/*
 * 获取流逝的时间
 * @return: 流逝的时间
 */
int Timer_getticks(Timer const *t)
{
    int ticks = 0;
    if (t && t->isstart) {
        ticks = t->ispause ? t->pauseticks : (SDL_GetTicks()-t->startticks);
    }
    return ticks;
}
/*
 * 计时是否开始
 * @return: 0: 没有开始
 *         !0: 开始了
 */
int Timer_isstart(Timer const *t)
{
    return (t && t->isstart);
}
/*
 * 当前是否暂停
 * @return: 0: 没有暂停
 *         !0: 处于暂停中
 */
int Timer_ispause(Timer const *t)
{
    return (t && t->isstart && t->ispause);
}
