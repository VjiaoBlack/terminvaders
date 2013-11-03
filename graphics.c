#include <stdio.h>
#include "graphics.h"
#include "xterm/xterm_control.h"

static sprite_t sprite_table[NUM_SPRITES] = {0};

/* Initialize the sprites table. */
static void init_sprites(void) {
    sprite_table[PLAYER] = (sprite_t) {
        {
            "  |  ",
            " /^\\ ",
            ":n^n:"
        }, 3, 3
    };
    sprite_table[ENEMY] = (sprite_t) {
        {
            ":---:",
            " \\0/ ",
            "  n  "
        }, 3, 3
    };
    sprite_table[BULLET] = (sprite_t) {{"."}, 1, 1};
    sprite_table[CANNON] = (sprite_t) {{"o"}, 1, 1};
    sprite_table[LASER]  = (sprite_t) {{"|"}, 1, 1};
}

/* Get a pointer to a sprite given its index. */
sprite_t* get_sprite(int name) {
    if (!sprite_table)
        init_sprites();
    return &sprite_table[name];
}

/* Draw a sprite on the screen at a given point. */
void draw(point_t* point, sprite_t* sprite) {
    int startx = point->x - sprite->width / 2,
        starty = point->y - sprite->height / 2,
        row, col;

    for (row = starty; row < sprite->height; row++) {
        xt_par2(XT_SET_ROW_COL_POS, row + 1, 1);
        for (col = startx; col < sprite->width; col++)
            putchar(sprite->graphic[row - starty][col - startx]);
    }
}
