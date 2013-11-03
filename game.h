#include "graphics.h"
#include "terminvaders.h"

#define US_PER_UPDATE (1000000 / 15)

struct player_t {
    point_t point;
    int health;
};
typedef struct player_t player_t;

struct enemy_t {
    point_t point;
    int health;
};
typedef struct enemy_t enemy_t;

struct game_t {
    int running;
    int score;
    player_t player;
    enemy_t* enemies;
};
typedef struct game_t game_t;

void play(void);

// TODO: lives, levels, waves, etc
