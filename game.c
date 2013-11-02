#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "graphics.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

/* Set up the game. */
void setup(game_t* game) {
    int r, c;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++)
            game->screen[r][c] = ' ';
    }
    game->running = 1;
}

/* Render the current game data. */
void render(game_t* game) {
    int r, c;

    for (r = 1; r <= ROWS; r++) {
        xt_par2(XT_SET_ROW_COL_POS, r, 1);
        for (c = 0; c < COLS; c++)
            putchar(game->screen[r - 1][c]);
    }
}

/* Handle user keyboard input during the game. */
void handle_input(game_t* game) {
    int key;

    switch ((key = getkey())) {
        case KEY_NOTHING:
            break;
        case 'Q':
            game->running = 0;
            break;
    }
}

/* Do a single cycle of game logic: render and handle input. */
void update(game_t* game) {
    handle_input(game);
    render(game);
}

/* Play a game. */
void play(void) {
    game_t game;
    setup(&game);
    while (game.running) {
        update(&game);
        usleep(US_PER_UPDATE);
    }
}
