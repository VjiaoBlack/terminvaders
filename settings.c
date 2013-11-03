#include <stdio.h>
#include "settings.h"
#include "terminvaders.h"

void configloop(void) {
    ///-- here are the variables --//
    char up = 'w', down = 's', left = 'a', right = 'd', quit = 'q', shoot = ' ';
    int config = 0;

    int cursor_r = 1, cursor_c = 1;
    clrscrn();
    SETPOS(1, 1);
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_c++;
    }
    cursor_r = 2;
    while (cursor_r < ROWS) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        printf("|");
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        printf("|");
        cursor_r++;
    }

    SETPOS(ROWS / 4, COLS / 2 - 7);
    xt_par0(XT_CH_YELLOW);
    xt_par0(XT_CH_BOLD);
    printf("{[ Settings ]}");
    xt_par0(XT_CH_NORMAL);

    SETPOS(ROWS * 2 / 5, COLS / 3 - 3);
    printf("up : '%c'", up);

    SETPOS(ROWS * 8 / 15, COLS / 3 - 5);
    printf("down : '%c'", down);

    SETPOS(ROWS * 2 / 3, COLS / 3 - 5);
    printf("left : '%c'", left);

    SETPOS(ROWS * 4 / 5, COLS / 3 - 6);
    printf("right : '%c'", right);

    SETPOS(ROWS * 8 / 15, COLS * 2 / 3 );
    printf("quit : '%c'", quit);

    SETPOS(ROWS * 2 / 3, COLS * 2 / 3 - 1);
    printf("shoot : '%c'", shoot); 

    SETPOS(ROWS * 2 / 5, COLS / 3 - 1);

    while(1) {

    }






}
