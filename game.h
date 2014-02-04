#pragma once
#include "graphics.h"
#include "terminvaders.h"

/* Defines */

#define FPS 20

#define PLAYER_LIVES 3
#define PLAYER_RESPAWN (FPS * 1.5)
#define PLAYER_INVINCIBILITY (FPS * 3)
#define PLAYER_X_VELOCITY 1
#define PLAYER_Y_VELOCITY 0.5
#define PLAYER_BULLET_VELOCITY -1
#define PLAYER_COOLDOWN (FPS / 4)
#define NUMBER_OF_PLAYERS (game->multiplayer ? game->multiplayer_data.players : 1)

#define COLLISION_POINTS 25
#define EXPLOSION_STEPS_PER_SPRITE 2
#define MAX_SPAWN_TIMER (FPS * 4)
#define MIN_SPAWN_TIMER (FPS * 2)
#define SPAWN_TIMER_DECREASE 3
#define GAME_OVER_TIMER (FPS * 4)

/* Structs */

struct player_t {
    point_t point;
    int lives;
    int respawning;
    int invincible;
    int cooldown;
    int nospawn;
    int vertical_accel;
    int horiz_accel;
    int bullet_type;
};
typedef struct player_t player_t;

struct enemy_t {
    point_t point;
    int score;
    int cooldown;
    int max_cooldown;
    double velocity;
    double bullet_velocity;
    struct enemy_t* next;
};
typedef struct enemy_t enemy_t;

struct bullet_t {
    point_t point;
    double velocity;
    int fired_by_player;
    int type; // 2 for normal, 4 for laser, 3 for cannon
    struct bullet_t* next;
};
typedef struct bullet_t bullet_t;

struct explosion_t {
    point_t point;
    int step;
    int score;
    struct explosion_t* next;
};
typedef struct explosion_t explosion_t;

struct multidata_t {
    int mode;
    int players;
    int player;
    int sockfd;
};
typedef struct multidata_t multidata_t;

struct game_t {
    int running;
    int multiplayer;
    int score;
    int over;
    int until_spawn;
    int spawn_timer;
    player_t* players;
    enemy_t* first_enemy;
    bullet_t* first_bullet;
    explosion_t* first_explosion;
    multidata_t multiplayer_data;
};
typedef struct game_t game_t;

/* Functions */

void spawn_player(game_t*, player_t*, int);
void handle_serializable_input(game_t*, int, int);
void setup_game(game_t*);
void do_logic(game_t*);
void play(void);
