#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"

/* Some function headers. */
static void spawn_player(game_t*);
static void despawn_player(game_t*);
static enemy_t* spawn_enemy(game_t*);
static enemy_t* despawn_enemy(game_t*, enemy_t*, enemy_t*);
static bullet_t* spawn_bullet(game_t*, int, int, double, int);
static bullet_t* despawn_bullet(game_t*, bullet_t*, bullet_t*);
static explosion_t* spawn_explosion(game_t*, int, int, int);
static explosion_t* despawn_explosion(game_t*, explosion_t*, explosion_t*);

/* Spawn the player in the game. */
static void spawn_player(game_t* game) {
    point_t point = {COLS / 2 - 1, ROWS - 5};
    game->player = (player_t) {point, 0, PLAYER_INVINCIBILITY, 0, 0};
}

/* Despawn the player in the game. */
static void despawn_player(game_t* game) {
    game->player.respawning = PLAYER_RESPAWN;
}

/* Spawn an enemy in the game. Returns a pointer to the enemy. */
static enemy_t* spawn_enemy(game_t* game) {
    enemy_t* enemy = malloc(sizeof(enemy_t));
    point_t point;
    double velocity = ((rand() % 11) + 5) / 10.,       // Between 0.5 and 1.5
           bullet_velocity = ((rand() % 3) + 1) * .5;  // Between 0.5 and 1.5
    int max_cooldown = (rand() % 11) + 5;              // Between 5 and 15
    // Final score is between 50 and 200
    int score = (velocity + bullet_velocity + (max_cooldown / 10.)) * 50 - 24.5;

    point.x = rand() % 2 ? 3 : COLS - 3;
    point.y = rand() % 10 + 2;
    *enemy = (enemy_t) {point, score, 0, max_cooldown, velocity,
                        bullet_velocity, game->first_enemy};
    game->first_enemy = enemy;
    return enemy;
}

/* Despawn an enemy in the game. Returns next enemy in linked list. */
static enemy_t* despawn_enemy(game_t* game, enemy_t* enemy, enemy_t* prev) {
    enemy_t* next = enemy->next;
    if (prev)
        prev->next = next;
    else
        game->first_enemy = next;
    free(enemy);
    return next;
}

/* Spawn a bullet in the game. Returns a pointer to the bullet. */
static bullet_t* spawn_bullet(game_t* game, int x, int y, double velocity, int fired_by_player) {
    bullet_t* bullet = malloc(sizeof(bullet_t));
    point_t point = {x, y};
    *bullet = (bullet_t) {point, velocity, fired_by_player, game->first_bullet};
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

/* Spawn an explosion in the game. Returns a pointer to the explosion. */
static explosion_t* spawn_explosion(game_t* game, int x, int y, int score) {
    explosion_t* explosion = malloc(sizeof(explosion_t));
    point_t point = {x, y};
    *explosion = (explosion_t) {point, 0, score, game->first_explosion};
    game->first_explosion = explosion;
    return explosion;
}

/* Despawn an explosion in the game. Returns next explosion in linked list. */
static explosion_t* despawn_explosion(game_t* game, explosion_t* explosion, explosion_t* prev) {
    explosion_t* next = explosion->next;
    if (prev)
        prev->next = next;
    else
        game->first_explosion = next;
    free(explosion);
    return next;
}

/* Make the player shoot a bullet. */
static void player_shoot(game_t* game) {
    game->player.cooldown = PLAYER_COOLDOWN;
    spawn_bullet(game, game->player.point.x, game->player.point.y - 2, PLAYER_BULLET_VELOCITY, 1);
}

/* Do game logic involving moving the player. */
static void do_player_movement_logic(game_t* game) {
    static int vertical_radius = 0, horiz_radius = 0;
    player_t* player = &game->player;
    if (!vertical_radius) {
        vertical_radius = get_sprite(PLAYER)->height / 2;
        horiz_radius = get_sprite(PLAYER)->width / 2;
    }

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

/* Do game logic involving the player. */
static void do_player_logic(game_t* game) {
    player_t* player = &game->player;
    if (player->invincible)  // Post-spawn invincibility timer
        player->invincible--;
    if (player->respawning && !game->over) {  // Respawn timer
        player->respawning--;
        if (!player->respawning) {
            if (!game->lives) {
                player->respawning = 1;  // Prevent player from being displayed
                game->over = GAME_OVER_TIMER;
            }
            else {
                game->lives--;
                spawn_player(game);
                player = &game->player;
            }
        }
    }
    if (player->cooldown)  // Bullet cooldown timer
        player->cooldown--;
    do_player_movement_logic(game);
}

/* Player bullet impact test function. See bullet_impacts(). */
static int player_bullet_impacts(game_t* game, bullet_t* bullet) {
    enemy_t* enemy = game->first_enemy;
    enemy_t* prev = NULL;
    while (enemy) {
        if (collides(&bullet->point, &enemy->point, 3, 1)) {
            game->score += enemy->score;
            despawn_enemy(game, enemy, prev);
            spawn_explosion(game, enemy->point.x, enemy->point.y, enemy->score);
            return 1;
        }
        prev = enemy;
        enemy = enemy->next;
    }
    return 0;
}

/* Enemy bullet impact test function. See bullet_impacts(). */
static int enemy_bullet_impacts(game_t* game, bullet_t* bullet) {
    player_t* player = &game->player;
    if (!player->respawning && !player->invincible) {
        if (collides(&bullet->point, &player->point, 2, 1)) {
            despawn_player(game);
            spawn_explosion(game, player->point.x, player->point.y, 0);
            return 1;
        }
    }
    return 0;
}

/* Test whether a bullet has hit any ships.
   If so, destroy that ship and return 1. Otherwise return 0. */
static int bullet_impacts(game_t* game, bullet_t* bullet) {
    if (bullet->fired_by_player)
        return player_bullet_impacts(game, bullet);
    return enemy_bullet_impacts(game, bullet);
}

/* Do game logic involving bullets. */
static void do_bullet_logic(game_t* game) {
    bullet_t* bullet = game->first_bullet;
    bullet_t* prev = NULL;

    while (bullet) {
        // bullet impact logic goes here
        bullet->point.y += bullet->velocity;
        if (bullet->point.y < 0 || bullet->point.y >= ROWS || bullet_impacts(game, bullet))
            bullet = despawn_bullet(game, bullet, prev);
        else {
            prev = bullet;
            bullet = bullet->next;
        }
    }
}

/* Do game logic involving enemies. */
static void do_enemy_logic(game_t* game) {
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
        if (!enemy->cooldown) {  // Bullet cooldown timer
            enemy->cooldown = enemy->max_cooldown;
            spawn_bullet(game, enemy->point.x, enemy->point.y + 2, enemy->bullet_velocity, 0);
        }
        else
            enemy->cooldown--;
        enemy = enemy->next;
    }
    if (!game->until_spawn) {
        spawn_enemy(game);
        game->until_spawn = game->spawn_timer;
        game->spawn_timer -= SPAWN_TIMER_DECREASE;
        if (game->spawn_timer < MIN_SPAWN_TIMER)
            game->spawn_timer = MIN_SPAWN_TIMER;
    }
    else
        game->until_spawn--;
}

/* Do game logic involving explosions. */
static void do_explosion_logic(game_t* game) {
    explosion_t* explosion = game->first_explosion;
    explosion_t* prev = NULL;

    while (explosion) {
        explosion->step++;
        if (explosion->step > EXPLOSIONS * EXPLOSION_STEPS_PER_SPRITE)
            explosion = despawn_explosion(game, explosion, prev);
        else {
            prev = explosion;
            explosion = explosion->next;
        }
    }
}

/* Do game logic, mainly involving bullets and enemy spawning/movement. */
static void do_logic(game_t* game) {
    do_player_logic(game);
    do_bullet_logic(game);
    do_enemy_logic(game);
    do_explosion_logic(game);
    if (game->over) {
        game->over--;
        if (!game->over)
            game->running = 0;
    }
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
                if (!game->player.respawning) {
                    if (game->player.vertical_accel > 0)
                        game->player.vertical_accel = 0;
                    else
                        game->player.vertical_accel = -1;
                }
                break;
            case KEY_DOWN:
            case 's':
                if (!game->player.respawning) {
                    if (game->player.vertical_accel < 0)
                        game->player.vertical_accel = 0;
                    else
                        game->player.vertical_accel = 1;
                }
                break;
            case KEY_LEFT:
            case 'a':
                if (!game->player.respawning) {
                    if (game->player.horiz_accel > 0)
                        game->player.horiz_accel = 0;
                    else
                        game->player.horiz_accel = -1;
                }
                break;
            case KEY_RIGHT:
            case 'd':
                if (!game->player.respawning) {
                    if (game->player.horiz_accel < 0)
                        game->player.horiz_accel = 0;
                    else
                        game->player.horiz_accel = 1;
                }
                break;
            case ' ':
                if (!game->player.respawning) {
                    if (!game->player.cooldown)
                        player_shoot(game);
                }
                break;
        }
    }
}

/* Draw the heads-up display to the screen. */
static void draw_hud(game_t* game) {
    int i;
    if (!game->over) {
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
    }
    else {
        SETPOS(ROWS / 2, COLS / 2 - 2);
        printf("%sGAME OVER%s", XT_CH_BOLD, XT_CH_NORMAL);
        SETPOS(ROWS / 2 + 1, COLS / 2 - 6);
        printf("%sYOUR SCORE:%s %s%5d%s", XT_CH_BOLD, XT_CH_NORMAL, XT_CH_YELLOW, game->score, XT_CH_NORMAL);
    }
    SETPOS(ROWS, COLS);
}

/* Render the current game data on-screen. */
static void render(game_t* game) {
    enemy_t* enemy = game->first_enemy;
    bullet_t* bullet = game->first_bullet;
    explosion_t* explosion = game->first_explosion;
    int explosion_num = 0;

    xt_par0(XT_CLEAR_SCREEN);
    while (explosion) {
        draw(&(explosion->point), get_sprite(EXPLOSION + (explosion->step / EXPLOSION_STEPS_PER_SPRITE)));
        if (explosion->score) {
            SETPOS(2 + explosion_num, 7);
            printf("%s+%d%s", XT_CH_YELLOW, explosion->score, XT_CH_NORMAL);
            explosion_num++;
        }
        explosion = explosion->next;
    }
    while (enemy) {
        draw(&(enemy->point), get_sprite(ENEMY));
        enemy = enemy->next;
    }
    if (!game->player.respawning && !(game->player.invincible % 2))
        draw(&(game->player.point), get_sprite(PLAYER));
    while (bullet) {
        draw(&(bullet->point), get_sprite(BULLET));
        bullet = bullet->next;
    }
    draw_hud(game);
}

/* Set up the game. */
static void setup(game_t* game) {
    game->running = 1;
    game->score = 0;
    game->lives = PLAYER_LIVES;
    game->over = 0;
    game->until_spawn = FPS;  // Wait one second for first enemy
    game->spawn_timer = MAX_SPAWN_TIMER;
    game->first_enemy = NULL;
    game->first_bullet = NULL;
    game->first_explosion = NULL;
    spawn_player(game);
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
