#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "xterm/keyboard.c"
#include "xterm/xterm_control.c"

#define FPS 60
#define ROWS 44
#define COLS 160

#define vo 10
#define xo 50
#define do 250
#define theta 1.732
#define numstars 100

#define CLRSCRN() xt_par0(XT_CLEAR_SCREEN);
#define SETPOS(x, y) xt_par2(XT_SET_ROW_COL_POS, (x), (y))

struct star_t {
    int x;
    int y;
    int xwait;
    int ywait;
    int roy;
    int rox;
    int d;
};
typedef struct star_t star_t;



void update(star_t*);
void display(star_t*);
int handle_input(int);
int calculate_wait(int, int);
void dispframe();

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL); //turns off buffering

    int running = 1;

    dispframe();

    star_t* stars = malloc(sizeof(star_t) * numstars);
    for (int i = 0; i < numstars; ++i) {
        int rox = 0;
        int roy = 0;
        while (!rox) 
            rox = rand() % 150 - 75;
        while (!roy) 
            roy = rand() % 150 - 75;

        int x = ( (rox * xo) / (do + xo) ) + COLS / 2; //( (rox * sqrt(3)) / (do + xo)) * COLS + (COLS / 2);
        int y = ( (roy * xo) / (do + xo) ) + ROWS / 2; //( (roy * sqrt(3)) / (do + xo)) * ROWS + (ROWS / 2);

        int xwait = calculate_wait(1, rox);
        int ywait = calculate_wait(0, roy);

        stars[i] = (star_t) {x, y, xwait, ywait, rox, roy, do};
    }

    while(running) {
        running = handle_input(running);
        update(stars);
        display(stars);
        usleep(1000000/FPS); //fps is 60
    }
}


void update(star_t* stars) {

    for (int i = 0; i < numstars; ++i) {

        if (stars[i].xwait <= 0) {
            stars[i].xwait = calculate_wait(1, abs(stars[i].x - COLS / 2));
            if (stars[i].x > COLS / 2)
                stars[i].x = stars[i].x + 1;
            else
                stars[i].x = stars[i].x - 1;

        } else {
            stars[i].xwait--;
        }
        if (stars[i].ywait <= 0) {
            stars[i].ywait = calculate_wait(0, abs(stars[i].y - ROWS / 2));

            if (stars[i].y > ROWS / 2)
                stars[i].y = stars[i].y + 1;
            else
                stars[i].y = stars[i].y - 1;
            

        } else {
            stars[i].ywait--;
        }

        if (stars[i].x >= COLS || stars[i].x <= 0 || stars[i].y >= ROWS || stars[i].y <= 0){
            int rox = 0;
            int roy = 0;
            while (!rox) 
                rox = rand() % 150 - 75;
            while (!roy) 
                roy = rand() % 150 - 75;

            int x = ( (rox * xo) / (do + xo) ) + COLS / 2; //( (rox * sqrt(3)) / (do + xo)) * COLS + (COLS / 2);
            int y = ( (roy * xo) / (do + xo) ) + ROWS / 2; //( (roy * sqrt(3)) / (do + xo)) * ROWS + (ROWS / 2);

            int xwait = calculate_wait(1, rox);
            int ywait = calculate_wait(0, roy);

            stars[i] = (star_t) {x, y, xwait, ywait, rox, roy, do};
        }

    }
}

void display(star_t* stars) {
    CLRSCRN();

    for (int i = 0; i < numstars; ++i) {
        SETPOS(stars[i].y, stars[i].x);
        if (abs(stars[i].x - COLS / 2) + abs(stars[i].y - ROWS / 2) > 30) 
            putchar('*');
        else 
            putchar('.');
    }
    dispframe();
}

void dispframe(){
    
    int cursor_r = 1, cursor_c = 1;
    SETPOS(1, 1);
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        putchar('-');
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        putchar('-');
        cursor_c++;
        usleep(1);
    }
    cursor_r = 2;
    while (cursor_r <= ROWS) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        putchar('|');
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        putchar('|');
        cursor_r++;
        usleep(1);
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

int calculate_wait(int is_horiz, int r) {
    int w;
    if (is_horiz)
        w = COLS;
    else 
        w = ROWS;

    int f = ( ((do + xo)*(do + xo)*(do + xo)) / (vo * r * r * w * theta) ) / 20;
    return f;   
}
