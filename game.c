#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"

/* Set up the game. */
static void setup(game_t* game) {
    point_t player_point = {COLS / 2 - 1, ROWS / 2 - 1};
    game->running = 1;
    game->score = 0;
    game->lives = PLAYER_LIVES;
    game->spawn_timer = FPS;
    game->player = (player_t) {player_point, 0, 0};
    game->first_enemy = NULL;
    game->first_bullet = NULL;
}

/* Spawn an enemy in the game. Return a pointer to the enemy. */
static enemy_t* spawn_enemy(game_t* game) {
    enemy_t* enemy = malloc(sizeof(enemy_t));
    point_t point = {COLS / 2 - 1, 3};
    *enemy = (enemy_t) {point, 1, game->first_enemy};
    game->first_enemy = enemy;
    return enemy;
}

/* Spawn a bullet in the game. Return a pointer to the bullet. */
static bullet_t* spawn_bullet(game_t* game, int x, int y, int fired_by_player) {
    bullet_t* bullet = malloc(sizeof(bullet_t));
    point_t point = {x, y};
    *bullet = (bullet_t) {point, fired_by_player, game->first_bullet};
    game->first_bullet = bullet;
    return bullet;
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

/* Do game logic involving the player. */
static void do_player_logic(game_t* game) {
    static int vertical_radius = 0, horiz_radius = 0;
    player_t* player = &game->player;

    if (!vertical_radius) {
        vertical_radius = get_sprite(PLAYER)->height / 2;
        horiz_radius = get_sprite(PLAYER)->width / 2;
    }
    if (player->cooldown)  // Bullet cooldown timer
        player->cooldown--;
    if (player->vertical_accel) {
        player->point.y += player->vertical_accel * PLAYER_Y_VELOCITY;
        if (player->vertical_accel < 0) {  // Boundary check
            if (player->point.y < vertical_radius) {
                player->point.y = vertical_radius;
                player->vertical_accel = 0;
            }
        }
        else if (player->point.y > ROWS - vertical_radius - 1) {
            player->point.y = ROWS - vertical_radius - 1;
            player->vertical_accel = 0;
        }
    }
    if (player->horiz_accel) {
        player->point.x += player->horiz_accel * PLAYER_X_VELOCITY;
        if (player->horiz_accel < 0) {  // Boundary check
            if (player->point.x < horiz_radius) {
                player->point.x = horiz_radius;
                player->horiz_accel = 0;
            }
        }
        else if (player->point.x > COLS - horiz_radius - 1) {
            player->point.x = COLS - horiz_radius - 1;
            player->horiz_accel = 0;
        }
    }
}

/* Do game logic involving bullets. */
static void do_bullet_logic(game_t* game) {
    bullet_t* bullet = game->first_bullet;
    bullet_t* prev = NULL;

    while (bullet) {
        // bullet impact logic goes here
        if (bullet->fired_by_player)
            bullet->point.y -= BULLET_VELOCITY;
        else
            bullet->point.y += BULLET_VELOCITY;
        if (bullet->point.y < 0 || bullet->point.y >= ROWS)
            bullet = despawn_bullet(game, bullet, prev);
        else {
            prev = bullet;
            bullet = bullet->next;
        }
    }
}

/* Do game logic involving the enemy. */
static void do_enemy_logic(game_t*  game) {
    static int vertical_radius = 0, horiz_radius = 0;
    enemy_t* enemy = game->first_enemy;

    if (!vertical_radius) {
        vertical_radius = get_sprite(ENEMY)->height / 2;
        horiz_radius = get_sprite(ENEMY)->width / 2;
    }
    while (enemy) {
        enemy->point.x += enemy->velocity;
        if (enemy->velocity < 0) {  // Boundary check
            if (enemy->point.x < horiz_radius) {
                enemy->point.x = horiz_radius;
                enemy->velocity = -enemy->velocity;
            }
        }
        else if (enemy->point.x > COLS - horiz_radius - 1) {
            enemy->point.x = COLS - horiz_radius - 1;
            enemy->velocity = -enemy->velocity;
        }
        enemy = enemy->next;
    }
    if (!game->spawn_timer) {
        spawn_enemy(game);
        game->spawn_timer = FPS;
    }
    else
        game->spawn_timer--;
}

/* Do game logic, mainly involving bullets and enemy spawning/movement. */
static void do_logic(game_t* game) {
    do_player_logic(game);
    do_bullet_logic(game);
    do_enemy_logic(game);
}

/* Handle user keyboard input during the game. */
static void handle_input(game_t* game) {
    int key;
    while ((key = getkey()) != KEY_NOTHING) {
        switch (key) {
            case 'q':
                game->running = 0;
                break;
            case KEY_UP:
            case 'w':
                if (game->player.vertical_accel > 0)
                    game->player.vertical_accel = 0;
                else
                    game->player.vertical_accel = -1;
                break;
            case KEY_DOWN:
            case 's':
                if (game->player.vertical_accel < 0)
                    game->player.vertical_accel = 0;
                else
                    game->player.vertical_accel = 1;
                break;
            case KEY_LEFT:
            case 'a':
                if (game->player.horiz_accel > 0)
                    game->player.horiz_accel = 0;
                else
                    game->player.horiz_accel = -1;
                break;
            case KEY_RIGHT:
            case 'd':
                if (game->player.horiz_accel < 0)
                    game->player.horiz_accel = 0;
                else
                    game->player.horiz_accel = 1;
                break;
            case ' ':
                if (!game->player.cooldown)
                    player_shoot(game);
                break;
        }
    }
}

/* Draw the heads-up display to the screen. */
static void draw_hud(game_t* game) {
    int i;
    SETPOS(1, 1);
    printf("%sScore:%s %s%d%s", XT_CH_BOLD, XT_CH_NORMAL, XT_CH_YELLOW, game->score, XT_CH_NORMAL);
    SETPOS(ROWS, 1);
    printf("%sShips:%s", XT_CH_BOLD, XT_CH_NORMAL);
    if (game->lives >= 5)
        printf(" %s^%s x %s%d%s", XT_CH_GREEN, XT_CH_NORMAL, XT_CH_YELLOW, game->lives, XT_CH_NORMAL);
    else if (game->lives) {
        for (i = 0; i < game->lives; i++)
            printf(" %s^%s", XT_CH_GREEN, XT_CH_NORMAL);
    }
    else
        printf(" %s0%s", XT_CH_YELLOW, XT_CH_NORMAL);
    SETPOS(ROWS, COLS);
}

/* Render the current game data on-screen. */
static void render(game_t* game) {
    enemy_t* enemy = game->first_enemy;
    bullet_t* bullet = game->first_bullet;

    xt_par0(XT_CLEAR_SCREEN);
    while (enemy) {
        draw(&(enemy->point), get_sprite(ENEMY));
        enemy = enemy->next;
    }
    draw(&(game->player.point), get_sprite(PLAYER));
    while (bullet) {
        draw(&(bullet->point), get_sprite(BULLET));
        bullet = bullet->next;
    }
    draw_hud(game);
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
