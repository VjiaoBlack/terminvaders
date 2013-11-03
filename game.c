#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"

/* Set up the game. */
static void setup(game_t* game) {
    game->running = 1;
    game->score = 0;
    game->lives = 3;
    game->player.point.x = COLS / 2 - 1;
    game->player.point.y = ROWS / 2 - 1;
    game->player.cooldown = 0;
    game->first_bullet = NULL;
    game->num_enemies = 0;
}

/* Spawn a bullet in the game. */
static void spawn_bullet(game_t* game, int x, int y, int fired_by_player) {
    bullet_t* bullet = malloc(sizeof(bullet_t));
    point_t point = {x, y};
    *bullet = (bullet_t) {point, fired_by_player, game->first_bullet};
    game->first_bullet = bullet;
}

/* Despawn a bullet in the game. Returns next bullet in linked list. */
static bullet_t* despawn_bullet(game_t* game, bullet_t* bullet, bullet_t* prev) {
    bullet_t* next = bullet->next;
    if (prev)
        prev->next = next;
    else
        game->first_bullet = next;
    free(bullet);
    return next;
}

/* Make the player shoot a bullet. */
static void player_shoot(game_t* game) {
    game->player.cooldown = PLAYER_COOLDOWN;
    spawn_bullet(game, game->player.point.x, game->player.point.y - 2, 1);
}

/* Do game logic involving bullets. */
static void do_bullet_logic(game_t* game) {
    bullet_t* bullet = game->first_bullet;
    bullet_t* prev = NULL;

    while (bullet) {
        // bullet impact logic goes here
        if (bullet->fired_by_player)
            bullet->point.y--;
        else
            bullet->point.y++;
        if (bullet->point.y < 0 || bullet->point.y >= ROWS)
            bullet = despawn_bullet(game, bullet, prev);
        else {
            prev = bullet;
            bullet = bullet->next;
        }
    }
}

/* Do game logic, mainly involving bullets and enemy spawning/movement. */
static void do_logic(game_t* game) {
    if (game->player.cooldown)
        game->player.cooldown--;
    do_bullet_logic(game);
    if (!game->num_enemies) {
        game->num_enemies = 1;
        game->enemies[0] = (enemy_t) {(point_t) {COLS / 2 - 1, 3}};  // ghetto spawn function
    }
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
            case ' ':
                if (!game->player.cooldown)
                    player_shoot(game);
                break;
        }
    }
}

/* Render the current game data. */
static void render(game_t* game) {
    bullet_t* bullet = game->first_bullet;
    int i;

    clear();
    draw(&(game->player.point), get_sprite(PLAYER));
    for (i = 0; i < game->num_enemies; i++)
        draw(&(game->enemies[i].point), get_sprite(ENEMY));
    while (bullet) {
        draw(&(bullet->point), get_sprite(BULLET));
        bullet = bullet->next;
    }
    SETPOS(1, 1);
    printf("Score: %s%d%s", XT_CH_YELLOW, game->score, XT_CH_NORMAL);
    SETPOS(2, 1);
    printf("Lives: %s%d%s", XT_CH_YELLOW, game->lives, XT_CH_NORMAL);
    SETPOS(ROWS, COLS);
}

/* Do a single cycle of game logic: render and handle input. */
static void update(game_t* game) {
    do_logic(game);
    handle_input(game);
    render(game);
}

/* Play a game. */
void play(void) {
    game_t game;
    setup(&game);
    while (game.running) {
        update(&game);
        usleep(1000000 / FPS);
    }
}
