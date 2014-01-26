#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

#define FPS 60
#define ROWS 40
#define COLS 80

#define vo 10
#define xo 50
#define theta 1.732

struct star_t {
    int x;
    int y;
    int xwait;
    int ywait;
    int roy;
    int rox;
};
typedef struct star_t star_t;



void update(star_t*);
void display(star_t*);
int handle_input(int);
int calculate_wait(int, int, int);

int main(int argc, char* argv[]) {
    int running = 1;

    star_t* stars = malloc(sizeof(star_t) * 100);
    for (int i = 0; i < 100; ++i) {
        int x = rand() % 20;
        int y = rand() % 10;

        int rox = rand() % (COLS / 3) + 1;
        int roy = rand() % (ROWS / 3) + 1;

        int xwait = calculate_wait(x, 1, rox);
        int ywait = calculate_wait(y, 1, roy);

        stars[i] = (star_t) {x, y, xwait, ywait, rox, roy};
    }

    while(running) {
        running = handle_input(running);
        update(stars);
        display(stars);
        usleep(1000000/FPS); //fps is 60
    }
}


void update(star_t* stars) {
    for (int i = 0; i < 100; ++i) {
        if (stars[i].xwait <= 0) {
            stars[i].xwait = calculate_wait(stars[i].x, 1, stars[i].rox);
        } else {
            stars[i].xwait--;
        }
        if (stars[i].ywait <= 0) {
            stars[i].ywait = calculate_wait(stars[i].y, 1, stars[i].roy);
        } else {
            stars[i].ywait--;
        }


    }
}

void display(star_t* stars) {
    for (int i = 0; i < 100; ++i) {
        xt_par2(XT_SET_ROW_COL_POS, stars[i].x, stars[i].y);
        printf(".\n");
    }
}

int handle_input(int running) {
    int key;
    while ((key = getkey()) != KEY_NOTHING) {
        switch (key) {
            case 'q':
                running = 0;
                break;
        }
    }
    return running;
}

int calculate_wait(int pos, int is_horiz, int r) {
    int w;
    if (is_horiz)
        w = COLS;
    else 
        w = ROWS;

    int f = ( ((pos + xo)*(pos + xo)) / (vo * r * w * theta) );
    return f;
}
