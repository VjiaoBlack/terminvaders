#include <stdio.h>
#include "settings.h"
#include "terminvaders.h"

int configloop(void) {
    ///-- here are the variables --//
    char key = ' ';
    char up = 'w', down = 's', left = 'a', right = 'd', Back = 'q', shoot = ' ';
    int config = 0;

    int cursor_r = 1, cursor_c = 1;
    CLRSCRN();
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

    SETPOS(ROWS / 2, COLS / 2 - 5);
    printf("Height: '%c'", up);

    SETPOS(5 * ROWS / 8, COLS / 2 - 4);
    printf("Width: '%c'", down);

    SETPOS(3 * ROWS / 4, COLS / 2 - 2);
    xt_par0(XT_CH_BOLD);
    xt_par0(XT_CH_INVERSE);
    printf("Back");

    while (1) {
            switch ((key = getkey())) {
            case KEY_UP:
            case 'w':
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                switch (config) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        break;
                }
                config = !config ? 2 : config - 1;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (config) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                break;
            case KEY_DOWN:
            case 's':
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                switch (config) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        break;
                }
                config = (config + 1) % 3;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (config) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                break;
            case KEY_ENTER:
                switch (config) {
                    case 0:
                    case 1:
                    case 2:
                        return 1;
                }
            case 'q':
                return MENU_QUIT;
        }
    }






}
