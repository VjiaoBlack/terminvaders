// Defines

#define PLAYER    0
#define ENEMY     1
#define BULLET    2
#define CANNON    3
#define LASER     4
#define EXPLOSION 5

#define NUM_SPRITES 6
#define MAX_SPRITE_HEIGHT 64

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
