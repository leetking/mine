#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

#include "../src/timer.h"
#include "../src/model.h"

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

/* 相等测试 */
#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do { \
        ++test_count; \
        if (equality) ++test_pass; \
        else { \
            fprintf(stderr, "%s: %d: expect: "format"\tactual: "format"\n", \
                    __FILE__, __LINE__, expect, actual); \
            main_ret = 1; \
        } \
    } while (0)
/* 值近似测试 */
#define EXPECT_SIM_BASE(expect, deviation, actual, format) \
    do { \
        ++test_count; \
        if ((expect)-(deviation) <= (actual) && (actual) <= (expect)+(deviation)) { \
            ++test_pass; \
        } else { \
            fprintf(stderr, "%s: %d: expect: ["format ", "format"]\tactual: "format"\n",\
                    __FILE__, __LINE__, (expect)-(deviation), (expect)+(deviation), actual); \
            main_ret = 1; \
        } \
    } while (0)
#define TEST_EQ_INT(a, b)   EXPECT_EQ_BASE((a)==(b), (a), (b), "%d")
#define TEST_NEQ_INT(e, a)  EXPECT_EQ_BASE((e)!=(a), "NOT EQ", "EQ", "%s")
#define TEST_AS(asval, act) EXPECT_SIM_BASE(asval, 1, act, "%d")
#define TEST_TRUE(e)        EXPECT_EQ_BASE((e), 1, (e), "%d")
#define TEST_FALSE(e)       EXPECT_EQ_BASE(!(e), 0, (e), "%d")
#define TEST_NOTNULL(p)     EXPECT_EQ_BASE(p, "NOTNULL", "NULL", "%s")
#define TEST_ISNULL(p)      EXPECT_EQ_BASE(!p, "NULL", "NOTNULL", "%s")

static void test_timer(void)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Timer t = TIMER_INIT;
    int ticks;
    TEST_FALSE(Timer_isstart(&t));
    Timer_start(&t);
    TEST_TRUE(Timer_isstart(&t));
    TEST_FALSE(Timer_ispause(&t));
    SDL_Delay(100);
    ticks = Timer_getticks(&t);
    TEST_AS(100, ticks);
    TEST_FALSE(Timer_ispause(&t));
    Timer_pause(&t);
    TEST_TRUE(Timer_ispause(&t));
    SDL_Delay(10);
    ticks = Timer_getticks(&t);
    TEST_AS(100, ticks);
    Timer_unpause(&t);
    SDL_Delay(100);
    ticks = Timer_getticks(&t);
    TEST_AS(200, ticks);
    TEST_FALSE(Timer_ispause(&t));
    ticks = Timer_restart(&t);
    TEST_AS(200, ticks);
    TEST_TRUE(Timer_isstart(&t));
    ticks = Timer_getticks(&t);
    TEST_AS(0, ticks);
    TEST_FALSE(Timer_ispause(&t));
    Timer_stop(&t);
    TEST_FALSE(Timer_isstart(&t));
    TEST_FALSE(Timer_ispause(&t));
    SDL_Quit();
}

static void test_model(void)
{
    map_t *map;
    map = map_create(8, 8, 5);

    map_destroy(map);
}

int main()
{
    test_timer();
    test_model();
    printf("%d/%d (%3.2f%%) passed.\n",
            test_pass, test_count, test_pass*100.0/test_count);
    return main_ret;
}


