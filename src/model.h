#ifndef MODEL_H_
#define MODEL_H_

typedef struct map_t map_t;
typedef struct agrid_t agrid_t;
typedef enum mark_t {
    MARK_EMPTY = 0,
    MARK_FLAG,
    MARK_MAYBE,
    MARK_MINE,
    MARK_EXPLOAED,
    MARK_INCORRECT,
} mark_t;

map_t *map_create(int w, int h, int nmine);
/*
 * 切换标记
 * @return: 返回切换后的标记
 */
mark_t map_mark(map_t *map, int row, int col);
/*
 * 打开这个没有开的格子，查看是否有雷
 * @reutrn: 0: 没有雷
 *          1: 游戏胜利
 *         -1: 有雷
 */
int map_open(map_t *map, int row, int col);
/*
 * 获取这个格子周围的地雷数
 * @return: 地雷数
 */
int map_getnmines(map_t *map, int row, int col);
void map_destroy(map_t *map);

struct agrid_t {
    char is_open;
    char has_mine;
    char nmine;     /* 周围有几个雷 */
    mark_t mark;
};
struct map_t {
    int w, h;
    int nmine;
    int closedgrids;
    agrid_t **grids;
};

#endif
