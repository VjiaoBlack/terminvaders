#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

/* Set up the game. */
void setup(game_t* game) {
    game->running = 1;
    game->score = 0;
    game->player.point.x = 10;
    game->player.point.y = 10;
}

/* Clear the screen. */
void clear(void) {
    int r, c;
    for (r = 1; r <= ROWS; r++) {
        xt_par2(XT_SET_ROW_COL_POS, r, 1);
        for (c = 1; c <= COLS; c++)
            putchar(' ');
    }
}

/* Render the current game data. */
void render(game_t* game) {
    clear();
    draw(&(game->player.point), get_sprite(PLAYER));
}

/* Handle user keyboard input during the game. */
void handle_input(game_t* game) {
    int key;
    switch ((key = getkey())) {
        case KEY_NOTHING:
            break;
        case 'q':
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
