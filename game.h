#include "graphics.h"
#include "terminvaders.h"

#define PLAYER_LIVES 3
#define PLAYER_RESPAWN 20
#define PLAYER_INVINCIBILITY 60
#define PLAYER_X_VELOCITY 1
#define PLAYER_Y_VELOCITY 0.5
#define PLAYER_BULLET_VELOCITY -2
#define PLAYER_COOLDOWN 5

#define GAME_OVER_TIMER 60
#define FPS 20

struct player_t {
    point_t point;
    int respawning;
    int invincible;
    int cooldown;
    int vertical_accel;
    int horiz_accel;
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
    struct bullet_t* next;
};
typedef struct bullet_t bullet_t;

struct game_t {
    int running;
    int score;
    int lives;
    int over;
    int spawn_timer;
    player_t player;
    enemy_t* first_enemy;
    bullet_t* first_bullet;
};
typedef struct game_t game_t;

void play(void);
