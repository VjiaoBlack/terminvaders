#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "game.h"
#include "client.h"

/* Some function headers. */
static void despawn_player(game_t*, player_t*);
static enemy_t* spawn_enemy(game_t*);
static enemy_t* despawn_enemy(game_t*, enemy_t*, enemy_t*);
static bullet_t* spawn_bullet(game_t*, int, int, double, int, int);
static bullet_t* despawn_bullet(game_t*, bullet_t*, bullet_t*);
static explosion_t* spawn_explosion(game_t*, int, int, int);
static explosion_t* despawn_explosion(game_t*, explosion_t*, explosion_t*);

/* Spawn a player in the game. */
void spawn_player(game_t* game, player_t* player, int lives) {
    point_t point = {COLS / 2 - 1, ROWS - 5};
    *player = (player_t) {point, lives, 0, PLAYER_INVINCIBILITY, 0, 0, 0, 0, 2};
}

/* Despawn a player in the game. */
static void despawn_player(game_t* game, player_t* player) {
    player->respawning = PLAYER_RESPAWN;
}

/* Spawn an enemy in the game. Returns a pointer to the enemy. */
static enemy_t* spawn_enemy(game_t* game) {
    enemy_t* enemy = malloc(sizeof(enemy_t));
    point_t point;
    double velocity = ((rand() % 11) + 5) / 10.,       // Between 0.5 and 1.5
           bullet_velocity = ((rand() % 3) + 1) * .5;  // Between 0.5 and 1.5
    int max_cooldown = (rand() % 11) + 5;              // Between 5 and 15
    // Final score is between 50 and 200x
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
static bullet_t* spawn_bullet(game_t* game, int x, int y, double velocity, int fired_by_player, int type) {
    bullet_t* bullet = malloc(sizeof(bullet_t));
    point_t point = {x, y};
    //------- change speed for cannons
    if (type == 3) {
        velocity = velocity * 3/4;
    }
    *bullet = (bullet_t) {point, velocity, fired_by_player, type, game->first_bullet};
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

/* Make a player shoot a bullet. */
static void player_shoot(game_t* game, player_t* player) {
    double velocity = PLAYER_BULLET_VELOCITY;
    player->cooldown = PLAYER_COOLDOWN;
    if (player->bullet_type == 3) {
        player->cooldown = PLAYER_COOLDOWN * 3;
        velocity = velocity * 3 / 4;
    }
    if (player->bullet_type == 4) {
        player->cooldown = PLAYER_COOLDOWN * 3;
        velocity *= 2;
    }

    spawn_bullet(game, player->point.x, player->point.y - 2, velocity, 1, player->bullet_type);
}

/* Do game logic involving moving a player. */
static void do_player_movement_logic(game_t* game, player_t* player) {
    static int vertical_radius = 0, horiz_radius = 0;
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

/* Test whether a player is colliding with any enemy ships. If so, destroy both. */
static void do_player_enemy_collision(game_t* game, player_t* player) {
    enemy_t* enemy = game->first_enemy;
    enemy_t* prev = NULL;
    while (enemy) {
        if (collides(&player->point, &enemy->point, 3, 3)) {
            game->score += COLLISION_POINTS;
            despawn_enemy(game, enemy, prev);
            despawn_player(game, player);
            spawn_explosion(game, enemy->point.x, enemy->point.y, COLLISION_POINTS);
            spawn_explosion(game, player->point.x, player->point.y, 0);
            return;
        }
        prev = enemy;
        enemy = enemy->next;
    }
}

/* Do game logic involving a player. */
static void do_player_logic(game_t* game, player_t* player) {
    if (player->invincible)  // Post-spawn invincibility timer
        player->invincible--;
    if (player->respawning && !player->nospawn) {  // Respawn timer
        player->respawning--;
        if (!player->respawning) {
            if (!player->lives) {
                player->respawning = 1;  // Prevent player from being displayed
                player->nospawn = 1;  // Prevent player from respawning
                if (game->multiplayer)
                    check_multiplayer_game_over(game);
                else
                    game->over = GAME_OVER_TIMER;
            }
            else
                spawn_player(game, player, player->lives - 1);
        }
    }
    if (player->cooldown)  // Bullet cooldown timer
        player->cooldown--;
    if (!player->respawning && !player->invincible)
        do_player_enemy_collision(game, player);
    if (!player->respawning) {
        do_player_movement_logic(game, player);
    }
}

/* Player bullet impact test function. See bullet_impacts(). */
static int player_bullet_impacts(game_t* game, bullet_t* bullet) {
    enemy_t* enemy = game->first_enemy;
    enemy_t* prev = NULL;
    while (enemy) {

        int fuzzy = get_sprite(bullet->type)->height / 2;

        fuzzy += 2;

        //-------------------------------------------------------------------------------
        if (collides(&bullet->point, &enemy->point, 3, fuzzy)) {
        //------------------------------------------------------------------------------------
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
    player_t* player;
    int slot;

    for (slot = 0; slot < NUMBER_OF_PLAYERS; slot++) {
        player = &game->players[slot];
        if (!player->respawning && !player->invincible) {
            if (collides(&bullet->point, &player->point, 2, 1)) {
                despawn_player(game, player);
                spawn_explosion(game, player->point.x, player->point.y, 0);
                return 1;
            }
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
        if (bullet->point.y < 0 || bullet->point.y >= ROWS || (bullet_impacts(game, bullet) && bullet->type != 3)) // cannons go thru ships
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
            spawn_bullet(game, enemy->point.x, enemy->point.y + 2, enemy->bullet_velocity, 0, 2);
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
void do_logic(game_t* game) {
    int slot;
    for (slot = 0; slot < NUMBER_OF_PLAYERS; slot++)
        do_player_logic(game, &game->players[slot]);
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
            case '1':
                game->players[0].bullet_type = 2;
                break;
            case '2':
                game->players[0].bullet_type = 3;
                break;
            case '3':
                game->players[0].bullet_type = 4;
                break;
            case 'q':
                game->running = 0;
                break;
            case KEY_UP:
            case 'w':
                if (!game->players[0].respawning) {
                    if (game->players[0].vertical_accel > 0)
                        game->players[0].vertical_accel = 0;
                    else
                        game->players[0].vertical_accel = -1;
                }
                break;
            case KEY_DOWN:
            case 's':
                if (!game->players[0].respawning) {
                    if (game->players[0].vertical_accel < 0)
                        game->players[0].vertical_accel = 0;
                    else
                        game->players[0].vertical_accel = 1;
                }
                break;
            case KEY_LEFT:
            case 'a':
                if (!game->players[0].respawning) {
                    if (game->players[0].horiz_accel > 0)
                        game->players[0].horiz_accel = 0;
                    else
                        game->players[0].horiz_accel = -1;
                }
                break;
            case KEY_RIGHT:
            case 'd':
                if (!game->players[0].respawning) {
                    if (game->players[0].horiz_accel < 0)
                        game->players[0].horiz_accel = 0;
                    else
                        game->players[0].horiz_accel = 1;
                }
                break;
            case ' ':
                if (!game->players[0].respawning) {
                    if (!game->players[0].cooldown)
                        player_shoot(game, &game->players[0]);
                }
                break;
        }
    }
}

/* Draw the heads-up display to the screen. */
static void draw_hud(game_t* game) {
    int i;
    player_t* player = &game->players[game->multiplayer ? game->multiplayer_data.player : 0];

    if (!game->over) {
        SETPOS(1, 1);
        printf("%sScore:%s %s%d%s", XT_CH_BOLD, XT_CH_NORMAL, XT_CH_YELLOW, game->score, XT_CH_NORMAL);
        SETPOS(ROWS, 1);
        printf("%sShips:%s", XT_CH_BOLD, XT_CH_NORMAL);
        if (player->lives >= 5)
            printf(" %s^%s x %s%d%s", XT_CH_GREEN, XT_CH_NORMAL, XT_CH_YELLOW, player->lives, XT_CH_NORMAL);
        else if (player->lives) {
            for (i = 0; i < player->lives; i++)
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
    player_t* player;
    int explosion_num = 0, slot, this_player;

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
    while (bullet) {
        draw(&(bullet->point), get_sprite(bullet->type + 1));
        bullet = bullet->next;
    }
    while (enemy) {
        draw(&(enemy->point), get_sprite(ENEMY));
        enemy = enemy->next;
    }

    this_player = game->multiplayer ? game->multiplayer_data.player : 0;
    for (slot = 0; slot < NUMBER_OF_PLAYERS; slot++) {
        player = &game->players[slot];
        if (!player->respawning && !(player->invincible % 2))
            draw(&(player->point), get_sprite(slot == this_player ? PLAYER : ALLY));
    }
    draw_hud(game);
}

/* Set up the game. */
void setup_game(game_t* game) {
    game->running = 1;
    game->multiplayer = 0;
    game->score = 0;
    game->over = 0;
    game->until_spawn = FPS;  // Wait one second for first enemy
    game->spawn_timer = MAX_SPAWN_TIMER;
    game->first_enemy = NULL;
    game->first_bullet = NULL;
    game->first_explosion = NULL;
    game->players = malloc(sizeof(player_t));
    spawn_player(game, &game->players[0], PLAYER_LIVES);
}

/* Do a single cycle of game logic: render and handle input. */
static void update(game_t* game) {
    if (game->multiplayer) {
        load_server_data(game);
        handle_input_multi(game);
    }
    else {
        do_logic(game);
        handle_input(game);
    }
    render(game);
}

/* Play a game. */
void play(void) {
    game_t game;
    setup_game(&game);
    while (game.running) {
        update(&game);
        usleep(1000000 / FPS);
    }
}
