#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

/* Set up the game. */
static void setup(game_t* game) {
    game->running = 1;
    game->score = 0;
    game->player.point.x = COLS / 2 - 1;
    game->player.point.y = ROWS / 2 - 1;
}

/* Render the current game data. */
static void render(game_t* game) {
    clear();
    draw(&(game->player.point), get_sprite(PLAYER));
}

/* Handle user keyboard input during the game. */
static void handle_input(game_t* game) {
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
static void update(game_t* game) {
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
