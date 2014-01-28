#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"
#include "settings.h"
#include "terminvaders.h"

#define fps 200
#define _rows_ 44
#define _cols_ 180

#define vo 10
#define xo 50
#define do 250
#define theta 1.732
#define numstars 100

struct star_t {
    int x;
    int y;
    int xwait;
    int ywait;
    int xwaited;
    int ywaited;
    int roy;
    int rox;
    int d;
};
typedef struct star_t star_t;


void init(star_t*);
void update(star_t*);
void display(star_t*);
int handle_input(int);
int calculate_wait(int, int, int);
void dispframe();
