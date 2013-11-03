#include <stdio.h>
#include "graphics.h"
#include "terminvaders.h"

static int sprite_init = 0;
static sprite_t sprite_table[NUM_SPRITES] = {0};

/* Initialize the sprites table. */
static void init_sprites(void) {
    sprite_init = 1;
    sprite_table[PLAYER] = (sprite_t) {
        {
            "  |  ",
            " /^\\ ",
            ":n^n:"
        }, 5, 3, XT_CH_GREEN
    };
    sprite_table[ENEMY] = (sprite_t) {
        {
            ":---:",
            " \\0/ ",
            "  n  "
        }, 5, 3, XT_CH_RED
    };
    sprite_table[BULLET] = (sprite_t) {{"."}, 1, 1};
    sprite_table[CANNON] = (sprite_t) {{"o"}, 1, 1};
    sprite_table[LASER]  = (sprite_t) {{"|"}, 1, 1};
}

/* Get a pointer to a sprite given its index. */
sprite_t* get_sprite(int name) {
    if (!sprite_init)
        init_sprites();
    return &sprite_table[name];
}

/* Draw a sprite on the screen at a given point. */
void draw(point_t* point, sprite_t* sprite) {
    int startx = point->x - sprite->width / 2,
        starty = point->y - sprite->height / 2,
        row, col;

    if (sprite->fg_color)
        xt_par0(sprite->fg_color);
    if (sprite->bg_color)
        xt_par0(sprite->bg_color);
    for (row = 0; row < sprite->height; row++) {
        SETPOS(row + starty + 1, startx + 1);
        for (col = 0; col < sprite->width; col++)
            putchar(sprite->graphic[row][col]);
    }
    if (sprite->fg_color)
        xt_par0(XT_CH_NORMAL);
    if (sprite->bg_color)
        xt_par0(XT_BG_DEFAULT);
}

/* Test whether two points of the given sprite types collide. */
int collides(point_t* point1, point_t* point2, int fuzzx, int fuzzy) {
    return (point1->x + fuzzx >= point2->x) && (point1->x - fuzzx <= point2->x) &&
           (point1->y + fuzzy >= point2->y) && (point1->y - fuzzy <= point2->y);
}
