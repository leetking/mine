#ifndef TIMER_H__
#define TIMER_H__

/*
 * 简单的计时器
 * NOTE 时间单位是微秒ms
 */

typedef struct Timer Timer;

void Timer_start(Timer *t);
void Timer_stop(Timer *t);
void Timer_pause(Timer *t);
void Timer_unpause(Timer *t);
/*
 * 重新开始计时器，并返回流逝的时间
 * @return: 流逝的时间
 */
int Timer_restart(Timer *t);
/*
 * 获取流逝的时间
 * @return: 流逝的时间
 */
int Timer_getticks(Timer const *t);
/*
 * 计时是否开始
 * @return: 0: 没有开始
 *         !0: 开始了
 */
int Timer_isstart(Timer const *t);
/*
 * 当前是否暂停
 * @return: 0: 没有暂停
 *         !0: 处于暂停中
 */
int Timer_ispause(Timer const *t);

struct Timer {
    char isstart, ispause;
    int startticks, pauseticks;
};

#endif
