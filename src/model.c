#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "model.h"

#define MIN(x, y)   ((x)<(y)?(x):(y))

map_t *map_create(int w, int h, int nmine)
{
    if (w <= 0 || h <= 0 || nmine <= 0) return NULL;
    int i, j;
    int nmine_;
    map_t *map = malloc(sizeof(map_t));
    map->w = w;
    map->h = h;
    map->closedgrids = w*h;
    map->nmine = MIN(w*h, nmine);
    map->grids = malloc(map->h * sizeof(agrid_t*));
    for (i = 0; i < map->h; ++i) {
        map->grids[i] = malloc(map->w * sizeof(agrid_t));
    }

    nmine_ = map->nmine;
    for (i = 0; i < map->h; ++i) {
        for (j = 0; j < map->w; ++j) {
            map->grids[i][j].mark = MARK_EMPTY;
            map->grids[i][j].is_open = 0;
            map->grids[i][j].has_mine = (0 < nmine_--);
            map->grids[i][j].nmine = 0;
        }
    }

    /* 随机打乱 */
    srand((unsigned int)(time(NULL)+clock()));
    for (i = 0; i < map->h; ++i) {
        for (j = 0; j < map->w; ++j) {
            agrid_t g;
            int row, col;
            g = map->grids[i][j];
            row = rand() % (map->w);
            col = rand() % (map->h);
            map->grids[i][j] = map->grids[row][col];
            map->grids[row][col] = g;
        }
    }

    /* 求出周围有几个雷 */
    for (i = 0; i < map->h; ++i) {
        for (j = 0; j < map->w; ++j) {
            int has_mine = (0 != map->grids[i][j].has_mine);
            if (i-1 >= 0 && j+1 < map->w) {
                map->grids[i][j].nmine += (0 != map->grids[i-1][j+1].has_mine);
                map->grids[i-1][j+1].nmine += has_mine;
            }
            if (j+1 < map->w) {
                map->grids[i][j].nmine += (0 != map->grids[i][j+1].has_mine);
                map->grids[i][j+1].nmine += has_mine;
            }
            if (i+1 < map->h && j+1 < map->w) {
                map->grids[i][j].nmine += (0 != map->grids[i+1][j+1].has_mine);
                map->grids[i+1][j+1].nmine += has_mine;
            }
            if (i+1 < map->h) {
                map->grids[i][j].nmine += (0 != map->grids[i+1][j].has_mine);
                map->grids[i+1][j].nmine += has_mine;
            }
        }
    }

    return map;
}
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
int map_open(map_t *map, int row, int col)
{
    if (map->grids[row][col].is_open) return 0;
    map->grids[row][col].is_open = 1;
    if (map->grids[row][col].has_mine) return -1;
    --map->closedgrids;
    if (map->nmine == map->closedgrids) return 1;

    if (0 != map->grids[row][col].nmine) return 0;

    /* 这个格子没有雷且周围没有雷，那么尝试展开地图 */
#define IS_IN_MAP(row, col)    (((0 <= (row)) && (row) < map->h) \
    && ((0 <= (col)) && (col) < map->w))
    char direction[8][2] = {
        {-1, -1}, {-1,  0}, {-1, +1}, { 0, -1},
        { 0, +1}, {+1, -1}, {+1,  0}, {+1, +1},
    };
    int i;
    for (i = 0; i < 8; ++i) {
        int row_, col_;
        row_ = row+direction[i][0];
        col_ = col+direction[i][1];
        if (IS_IN_MAP(row_, col_)) {
            map_open(map, row_, col_);
        }
    }
#undef IS_IN_MAP
    return 0;
}
/*
 * 获取这个格子周围的地雷数
 * @return: 地雷数
 */
int map_getnmines(map_t *map, int row, int col)
{
    return map->grids[row][col].nmine;
}
void map_destroy(map_t *map)
{
    if (!map) return;
    int i;
    for (i = 0; i < map->w; ++i) {
        free(map->grids[i]);
    }
    free(map->grids);
    free(map);
}
