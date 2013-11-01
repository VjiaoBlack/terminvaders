#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "graphics.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

void setup(game_t* game) {
    int r, c;

    game->running = 1;
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++)
            game->screen[r][c] = ' ';
    }
}

void render(game_t* game) {
    int r, c;

    xt_par0(XT_CLEAR_SCREEN);
    for (r = 1; r <= ROWS; r++) {
        xt_par2(XT_SET_ROW_COL_POS, r, 1);
        for (c = 0; c < COLS; c++)
            putchar(game->screen[r - 1][c]);
    }
}

void handle_input(game_t* game) {
    int key;

    switch ((key = getkey())) {
        case KEY_NOTHING:
            break;
    }
}

void update(game_t* game) {
    handle_input(game);
    render(game);
}

void play() {
    game_t game;
    setup(&game);
    while (game.running) {
        update(&game);
        // usleep(US_PER_UPDATE);
    }
}
