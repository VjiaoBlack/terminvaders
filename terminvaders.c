#include <stdio.h>
#include "terminvaders.h"
#include "game.h"

/* Set up the game by clearing the screen, etc. */
static void setup(void) {
    setbuf(stdout, NULL);  // Turn off stdout buffering
    xt_par0(XT_CLEAR_SCREEN);
}

/* Clear the screen. */
static void clear(void) {
    int r, c;
    for (r = 1; r <= ROWS; r++) {
        SETPOS(r, 1);
        for (c = 1; c <= COLS; c++)
            putchar(' ');
    }
}

/* Main menu: render the menu and return an option selected by the user. */
static int menu(void) {
    int key, choice = 0;
    int cursor_r = 1, cursor_c = 1;
    clear();
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

    xt_par0(XT_CH_BOLD);
    xt_par0(XT_CH_YELLOW);
    SETPOS(ROWS / 4, COLS / 2 - 11);
    printf("-~<[ terminvaders ]>~-");

    xt_par0(XT_CH_NORMAL);
    xt_par0(XT_CH_GREEN);
    xt_par0(XT_CH_INVERSE);
    xt_par0(XT_CH_BOLD);
    SETPOS(ROWS / 2, COLS / 2 - 2);
    printf("Play");

    xt_par0(XT_CH_NORMAL);
    SETPOS(5 * ROWS / 8, COLS / 2 - 4);
    printf("Settings");

    xt_par0(XT_CH_RED);
    SETPOS(3 * ROWS / 4, COLS / 2 - 2);
    printf("Quit");

    SETPOS(ROWS, COLS);
    while (1) {
        switch ((key = getkey())) {
            case KEY_UP:
            case 'w':
                xt_par0(XT_CH_DEFAULT);
                switch (choice) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 2);
                        xt_par0(XT_CH_GREEN);
                        printf("Play");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        xt_par0(XT_CH_WHITE);
                        printf("Settings");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        xt_par0(XT_CH_RED);
                        printf("Quit");
                        break;
                }
                choice = !choice ? 2 : choice - 1;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (choice) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 2);
                        xt_par0(XT_CH_GREEN);
                        printf("Play");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        xt_par0(XT_CH_WHITE);
                        xt_par0(XT_CH_BOLD);
                        printf("Settings");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        xt_par0(XT_CH_RED);
                        printf("Quit");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                break;
            case KEY_DOWN:
            case 's':
                xt_par0(XT_CH_DEFAULT);
                switch (choice) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 2);
                        xt_par0(XT_CH_GREEN);
                        printf("Play");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        xt_par0(XT_CH_WHITE);
                        printf("Settings");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        xt_par0(XT_CH_RED);
                        printf("Quit");
                        break;
                }
                choice = (choice + 1) % 3;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (choice) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 2);
                        xt_par0(XT_CH_GREEN);
                        printf("Play");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        xt_par0(XT_CH_WHITE);
                        xt_par0(XT_CH_BOLD);
                        printf("Settings");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        xt_par0(XT_CH_RED);
                        printf("Quit");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                break;
            case KEY_ENTER:
                return choice;
            case 'q':
                return MENU_QUIT;
        }
    }
}

/* Main menu loop. */
static void loop(void) {
    int option;
    while (1) {
        switch ((option = menu())) {
            case MENU_PLAY:
                play();
            case MENU_SETTINGS:
                break;
            case MENU_QUIT:
                return;
        }
    }
}

/* Reset the keyboard and screen. */
static void finish(void) {
    getkey_terminate();
    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    SETPOS(1, 1);
}

int main(int argc, char* argv[]) {
    setup();
    loop();
    finish();
    return 0;
}
