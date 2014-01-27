#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "xterm/keyboard.c"
#include "xterm/xterm_control.c"

#define FPS 200
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
    int xwaited;
    int ywaited;
    int roy;
    int rox;
    int d;
};
typedef struct star_t star_t;



void update(star_t*);
void display(star_t*);
int handle_input(int);
int calculate_wait(int, int, int);
void dispframe();

int main(int argc, char* argv[]) {
    //setbuf(stdout, NULL); //turns off buffering

    int running = 1;

    dispframe();

    star_t* stars = malloc(sizeof(star_t) * numstars);
    for (int i = 0; i < numstars; ++i) {
        int rox = 0;
        int roy = 0;
        while (!rox) 
            rox = rand() % 400 - 200;
        while (!roy) 
            roy = rand() % 200 - 100;

        int x = ( (rox * xo) / (do + xo) ) + COLS / 2; //( (rox * sqrt(3)) / (do + xo)) * COLS + (COLS / 2);
        int y = ( (roy * xo) / (do + xo) ) + ROWS / 2; //( (roy * sqrt(3)) / (do + xo)) * ROWS + (ROWS / 2);

        int xwait = calculate_wait(1, rox, roy);
        int ywait = calculate_wait(0, rox, roy);

        int d = sqrt((x - COLS / 2) * (x - COLS / 2) + (y - ROWS / 2) * (y - ROWS / 2) * 4);

        stars[i] = (star_t) {x, y, xwait, ywait, 0, 0, rox, roy, d};
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

        stars[i].d = sqrt((stars[i].x - COLS / 2) * (stars[i].x - COLS / 2) + (stars[i].y - ROWS / 2) * (stars[i].y - ROWS / 2 ) * 4);

        if (stars[i].xwait - stars[i].xwaited <= 0) {
            stars[i].xwaited = 0;
            stars[i].xwait = calculate_wait(1, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));
            stars[i].ywait = calculate_wait(0, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));
            /*
            if (stars[i].ywait <= 1) {
                stars[i].ywait = calculate_wait(0, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));
                if (stars[i].y > ROWS / 2)
                    stars[i].y = stars[i].y + 1;
                else
                    stars[i].y = stars[i].y - 1;
            }

            else*/ if (stars[i].x > COLS / 2)
                stars[i].x = stars[i].x + 1;
            else
                stars[i].x = stars[i].x - 1;

        } else {
            stars[i].xwaited++;
        }
        if (stars[i].ywait - stars[i].ywaited <= 0) {
            stars[i].ywaited = 0;
            stars[i].xwait = calculate_wait(1, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));
            stars[i].ywait = calculate_wait(0, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));

            /*
            if (stars[i].xwait <= 1) {
                stars[i].xwait = calculate_wait(1, abs(stars[i].x - COLS / 2), abs(stars[i].y - ROWS / 2));

                if (stars[i].x > COLS / 2)
                    stars[i].x = stars[i].x + 1;
                else
                    stars[i].x = stars[i].x - 1;
            }

            else */ if (stars[i].y > ROWS / 2)
                stars[i].y = stars[i].y + 1;
            else
                stars[i].y = stars[i].y - 1;
            

        } else {
            stars[i].ywaited++;
        }

        if (stars[i].x >= COLS || stars[i].x <= 0 || stars[i].y >= ROWS || stars[i].y <= 0){
            int rox = 0;
            int roy = 0;
            while (!rox) 
                rox = rand() % 400 - 200;
            while (!roy) 
                roy = rand() % 200 - 100;

            int x = ( (rox * xo) / (do + xo) ) + COLS / 2; //( (rox * sqrt(3)) / (do + xo)) * COLS + (COLS / 2);
            int y = ( (roy * xo) / (do + xo) ) + ROWS / 2; //( (roy * sqrt(3)) / (do + xo)) * ROWS + (ROWS / 2);

            int xwait = calculate_wait(1, abs(rox), abs(roy));
            int ywait = calculate_wait(0, abs(rox), abs(roy));

            stars[i] = (star_t) {x, y, xwait, ywait, 0, 0, rox, roy, do};
        }

    }
}

void display(star_t* stars) {
    CLRSCRN();
    
    for (int i = 0; i < numstars; ++i) {
        SETPOS(stars[i].y, stars[i].x);
        
        if (abs(stars[i].x - COLS / 2) + 2 * abs(stars[i].y - ROWS / 2) > 60) {
            putchar('#');
        } else if (abs(stars[i].x - COLS / 2) + 2 * abs(stars[i].y - ROWS / 2) > 35) {
            if (abs((stars[i].x - COLS / 2) - 2 * (stars[i].y - ROWS / 2)) < 12) 
                putchar('\\');
            else if (abs((stars[i].x - COLS / 2) + 2 * (stars[i].y - ROWS / 2)) < 12) 
                putchar('/');
            else if (abs(stars[i].x - COLS / 2) < 20) {
                putchar('|');
            } else if (abs(stars[i].y - ROWS / 2 ) < 20) {
                putchar('_');
            }
        } else {
            putchar('.');
        }
        xt_par0(XT_CH_NORMAL);
        
        //printf("%d", stars[i].ywait - stars[i].ywaited);

        fflush(stdout);
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
        fflush(stdout);
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
        fflush(stdout);
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

int calculate_wait(int is_horiz, int x, int y) { // x and y are displacements from center.
    /*int w;
    int r;
    if (is_horiz){
        r = x;
        w = COLS;
    }
    else {
        w = ROWS;
        r = y;
        y = y / 20;
    }
    */

    double d = sqrt(x * x / 16 + y * y / 8) + 5;
    double r;
    if (is_horiz) {
        r =  ((double)y / (double)x) + .1;
        r *= 2;
    } else {
        r =  ((double)x / (double)y) + .1;
    }

    int fd = 2000 / (d * d);

    return (r * fd);


    /*
    int f = ( ((do + xo)*(do + xo)) / (vo * r * w * theta) );

    return f; */
}

