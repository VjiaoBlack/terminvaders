#include "graphics.h"
#include "terminvaders.h"

#define PLAYER_COOLDOWN 5
#define FPS 15

struct player_t {
    point_t point;
    int cooldown;
};
typedef struct player_t player_t;

struct enemy_t {
    point_t point;
    struct enemy_t* next;
};
typedef struct enemy_t enemy_t;

struct bullet_t {
    point_t point;
    int fired_by_player;
    struct bullet_t* next;
};
typedef struct bullet_t bullet_t;

struct game_t {
    int running;
    int score;
    int lives;
    player_t player;
    enemy_t* first_enemy;
    bullet_t* first_bullet;
};
typedef struct game_t game_t;

void play(void);
