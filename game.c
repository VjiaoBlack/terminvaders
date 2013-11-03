#include <stdio.h>
#include <unistd.h>
#include "game.h"

/* Set up the game. */
static void setup(game_t* game) {
    game->running = 1;
    game->score = 0;
    game->lives = 3;
    game->player.point.x = COLS / 2 - 1;
    game->player.point.y = ROWS / 2 - 1;
    game->num_enemies = 0;
}

/* Render the current game data. */
static void render(game_t* game) {
    int i;
    clear();
    draw(&(game->player.point), get_sprite(PLAYER));
    for (i = 0; i < game->num_enemies; i++)
        draw(&(game->enemies[i].point), get_sprite(ENEMY));
    SETPOS(1, 1);
    printf("Score: %s%d%s", XT_CH_YELLOW, game->score, XT_CH_NORMAL);
    SETPOS(2, 1);
    printf("Lives: %s%d%s", XT_CH_YELLOW, game->lives, XT_CH_NORMAL);
    SETPOS(ROWS, COLS);
}

/* Handle user keyboard input during the game. */
static void handle_input(game_t* game) {
    int key, playerx, playery;
    while ((key = getkey()) != KEY_NOTHING) {
        playerx = game->player.point.x;
        playery = game->player.point.y;
        switch (key) {
            case 'q':
                game->running = 0;
                break;
            case KEY_UP:
            case 'w':
                if (playery > get_sprite(PLAYER)->height / 2)
                    game->player.point.y--;
                break;
            case KEY_DOWN:
            case 's':
                if (playery < ROWS - get_sprite(PLAYER)->height / 2 - 1)
                    game->player.point.y++;
                break;
            case KEY_LEFT:
            case 'a':
                if (playerx > get_sprite(PLAYER)->width / 2)
                    game->player.point.x--;
                break;
            case KEY_RIGHT:
            case 'd':
                if (playerx < COLS - get_sprite(PLAYER)->width / 2 - 1)
                    game->player.point.x++;
                break;
        }
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
