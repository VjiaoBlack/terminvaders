#pragma once

// Defines

#define PLAYER     0
#define ALLY       1
#define ENEMY      2
#define BULLET     3
#define CANNON     4
#define LASER      5
#define EXPLOSION  6
#define EXPLOSIONS 6

#define NUM_SPRITES 12
#define MAX_SPRITE_HEIGHT 7

// Structs

struct point_t {
    double x;
    double y;
};
typedef struct point_t point_t;

struct sprite_t {
    char* graphic[MAX_SPRITE_HEIGHT];
    int width;
    int height;
    char* fg_color;
    char* bg_color;
};
typedef struct sprite_t sprite_t;

// Functions

sprite_t* get_sprite(int);
void draw(point_t*, sprite_t*);
int collides(point_t*, point_t*, int, int);
