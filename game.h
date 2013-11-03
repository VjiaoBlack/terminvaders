#include "graphics.h"
#include "terminvaders.h"

#define MAX_ENEMIES 100
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
    int lives;
    player_t player;
    enemy_t enemies[MAX_ENEMIES];
    int num_enemies;
};
typedef struct game_t game_t;

void play(void);
