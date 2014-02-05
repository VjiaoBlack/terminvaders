#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terminvaders.h"
#include "game.h"
#include "settings.h"
#include "star.h"  
#include "lobby.h"

/* Set up the game by clearing the screen, etc. */
static void setup(void) {
    srand(time(NULL));
    setbuf(stdout, NULL);  // Turn off stdout buffering
    xt_par0(XT_CLEAR_SCREEN);
    load_rc();
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

static void drawmenu(int choice) {

    int cursor_r = 1, cursor_c = 1;
    SETPOS(1, 1);
    int test = 0;
    int lim = 3;
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("~");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("~");
        cursor_c++;

        if (test == lim){
            //fflush(stdout);
            test = 0;
        }
        test++;
    }
    cursor_r = 2;
    while (cursor_r < ROWS) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        printf("#");
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        printf("#");
        cursor_r++;
        if (test == lim){
            //fflush(stdout);
            test = 0;
        }
        test++;
    }

    xt_par0(XT_CH_BOLD);
    xt_par0(XT_CH_YELLOW);
    SETPOS(ROWS / 4, COLS / 2 - 11);
    printf("-~<[ terminvaders ]>~-");

    /*xt_par0(XT_CH_NORMAL);
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
    printf("Quit");*/
    //fflush(stdout);

    xt_par0(XT_CH_DEFAULT);

    SETPOS(ROWS / 2, COLS / 2 - 6);
    xt_par0(XT_CH_GREEN);
    printf("Single Player");

    SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
    xt_par0(XT_CH_WHITE);
    printf("Settings");

    SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
    xt_par0(XT_CH_RED);
    printf("Quit");

    SETPOS(ROWS / 2 + OFFSET, COLS / 2 - 5);
    xt_par0(XT_CH_RED);
    xt_par0(XT_CH_BOLD);
    printf("M");
    xt_par0(XT_CH_YELLOW);
    printf("u");
    xt_par0(XT_CH_GREEN);
    printf("L");
    xt_par0(XT_CH_CYAN);
    printf("t");
    xt_par0(XT_CH_BLUE);
    printf("I");
    xt_par0(XT_CH_MAGENTA);
    printf("p");
    xt_par0(XT_CH_RED);
    printf("L");
    xt_par0(XT_CH_YELLOW);
    printf("a");
    xt_par0(XT_CH_GREEN);
    printf("Y");
    xt_par0(XT_CH_CYAN);
    printf("e");
    xt_par0(XT_CH_BLUE);
    printf("R");



    xt_par0(XT_CH_BOLD);
    xt_par0(XT_CH_INVERSE);
    switch (choice) { // makes the button highlighted
        case 0:
            SETPOS(ROWS / 2, COLS / 2 - 6);
            xt_par0(XT_CH_GREEN);
            printf("Single Player");
            SETPOS(ROWS, COLS);
            break;
        case 2:
            SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
            xt_par0(XT_CH_WHITE);
            xt_par0(XT_CH_BOLD);
            printf("Settings");
            SETPOS(ROWS, COLS);
            break;
        case 3:
            SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
            xt_par0(XT_CH_RED);
            printf("Quit");
            SETPOS(ROWS, COLS);
            break;
        case 1:
            SETPOS(ROWS / 2 + 1 * OFFSET, COLS / 2 - 5);
            xt_par0(XT_CH_RED);
            xt_par0(XT_CH_BOLD);
            printf("M");
            xt_par0(XT_CH_YELLOW);
            printf("u");
            xt_par0(XT_CH_GREEN);
            printf("L");
            xt_par0(XT_CH_CYAN);
            printf("t");
            xt_par0(XT_CH_BLUE);
            printf("I");
            xt_par0(XT_CH_MAGENTA);
            printf("p");
            xt_par0(XT_CH_RED);
            printf("L");
            xt_par0(XT_CH_YELLOW);
            printf("a");
            xt_par0(XT_CH_GREEN);
            printf("Y");
            xt_par0(XT_CH_CYAN);
            printf("e");
            xt_par0(XT_CH_BLUE);
            printf("R");
            SETPOS(ROWS, COLS);
            break;
    }
    xt_par0(XT_CH_NORMAL);

    //fflush(stdout);
}
static int menu(void) {
    clear();
    drawmenu(0);
    star_t* stars = malloc(sizeof(star_t) * numstars);
    init(stars);

    int frame = 1;
    int key, choice = 0;
    SETPOS(ROWS, COLS);
    while (1) {
        update(stars);
        if (frame % 10 == 0) {
            display(stars);
            frame = 1;
        }
        else
            frame++;
        drawmenu(choice);
        switch ((key = getkey())) {
        case KEY_UP:
        case 'w':
            xt_par0(XT_CH_DEFAULT);
            switch (choice) { // makes the previous button 'normal'
                case 0:
                    SETPOS(ROWS / 2, COLS / 2 - 6);
                    xt_par0(XT_CH_GREEN);
                    printf("Single Player");
                    break;
                case 2:
                    SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
                    xt_par0(XT_CH_WHITE);
                    printf("Settings");
                    break;
                case 3:
                    SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
                    xt_par0(XT_CH_RED);
                    printf("Quit");
                    break;
                case 1:
                    SETPOS(ROWS / 2 + 1 * OFFSET, COLS / 2 - 5);
                    xt_par0(XT_CH_RED);
                    xt_par0(XT_CH_BOLD);
                    printf("M");
                    xt_par0(XT_CH_YELLOW);
                    printf("u");
                    xt_par0(XT_CH_GREEN);
                    printf("L");
                    xt_par0(XT_CH_CYAN);
                    printf("t");
                    xt_par0(XT_CH_BLUE);
                    printf("I");
                    xt_par0(XT_CH_MAGENTA);
                    printf("p");
                    xt_par0(XT_CH_RED);
                    printf("L");
                    xt_par0(XT_CH_YELLOW);
                    printf("a");
                    xt_par0(XT_CH_GREEN);
                    printf("Y");
                    xt_par0(XT_CH_CYAN);
                    printf("e");
                    xt_par0(XT_CH_BLUE);
                    printf("R");
                    break;
            }
            if (choice > 0)
                choice--;
            else 
                choice = 3;
            xt_par0(XT_CH_BOLD);
            xt_par0(XT_CH_INVERSE);
            switch (choice) { // makes the button highlighted
                case 0:
                    SETPOS(ROWS / 2, COLS / 2 - 6);
                    xt_par0(XT_CH_GREEN);
                    printf("Single Player");
                    SETPOS(ROWS, COLS);
                    break;
                case 2:
                    SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
                    xt_par0(XT_CH_WHITE);
                    xt_par0(XT_CH_BOLD);
                    printf("Settings");
                    SETPOS(ROWS, COLS);
                    break;
                case 3:
                    SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
                    xt_par0(XT_CH_RED);
                    printf("Quit");
                    SETPOS(ROWS, COLS);
                    break;
                case 1:
                    SETPOS(ROWS / 2 + 1 * OFFSET, COLS / 2 - 5);
                    xt_par0(XT_CH_RED);
                    xt_par0(XT_CH_BOLD);
                    printf("M");
                    xt_par0(XT_CH_YELLOW);
                    printf("u");
                    xt_par0(XT_CH_GREEN);
                    printf("L");
                    xt_par0(XT_CH_CYAN);
                    printf("t");
                    xt_par0(XT_CH_BLUE);
                    printf("I");
                    xt_par0(XT_CH_MAGENTA);
                    printf("p");
                    xt_par0(XT_CH_RED);
                    printf("L");
                    xt_par0(XT_CH_YELLOW);
                    printf("a");
                    xt_par0(XT_CH_GREEN);
                    printf("Y");
                    xt_par0(XT_CH_CYAN);
                    printf("e");
                    xt_par0(XT_CH_BLUE);
                    printf("R");
                    break;
            }
            xt_par0(XT_CH_NORMAL);
            break;
        case KEY_DOWN:
        case 's':
            xt_par0(XT_CH_DEFAULT);
            switch (choice) { // makes the previous button 'normal'
                case 0:
                    SETPOS(ROWS / 2, COLS / 2 - 6);
                    xt_par0(XT_CH_GREEN);
                    printf("Single Player");
                    break;
                case 2:
                    SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
                    xt_par0(XT_CH_WHITE);
                    printf("Settings");
                    break;
                case 3:
                    SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
                    xt_par0(XT_CH_RED);
                    printf("Quit");
                    break;
                case 1:
                    SETPOS(ROWS / 2 + 1 * OFFSET, COLS / 2 - 5);
                    xt_par0(XT_CH_RED);
                    xt_par0(XT_CH_BOLD);
                    printf("M");
                    xt_par0(XT_CH_YELLOW);
                    printf("u");
                    xt_par0(XT_CH_GREEN);
                    printf("L");
                    xt_par0(XT_CH_CYAN);
                    printf("t");
                    xt_par0(XT_CH_BLUE);
                    printf("I");
                    xt_par0(XT_CH_MAGENTA);
                    printf("p");
                    xt_par0(XT_CH_RED);
                    printf("L");
                    xt_par0(XT_CH_YELLOW);
                    printf("a");
                    xt_par0(XT_CH_GREEN);
                    printf("Y");
                    xt_par0(XT_CH_CYAN);
                    printf("e");
                    xt_par0(XT_CH_BLUE);
                    printf("R");
                    break;
            }
            if (choice < 3) 
                choice++;
            else
                choice = 0;
            xt_par0(XT_CH_BOLD);
            xt_par0(XT_CH_INVERSE);
            switch (choice) { // makes the button highlighted
                case 0:
                    SETPOS(ROWS / 2, COLS / 2 - 6);
                    xt_par0(XT_CH_GREEN);
                    printf("Single Player");
                    SETPOS(ROWS, COLS);
                    break;
                case 2:
                    SETPOS(ROWS / 2 + 2 * OFFSET, COLS / 2 - 4);
                    xt_par0(XT_CH_WHITE);
                    xt_par0(XT_CH_BOLD);
                    printf("Settings");
                    SETPOS(ROWS, COLS);
                    break;
                case 3:
                    SETPOS(ROWS / 2 + 3 * OFFSET, COLS / 2 - 2);
                    xt_par0(XT_CH_RED);
                    printf("Quit");
                    SETPOS(ROWS, COLS);
                    break;
                case 1:
                    SETPOS(ROWS / 2 + OFFSET, COLS / 2 - 5);
                    xt_par0(XT_CH_RED);
                    xt_par0(XT_CH_BOLD);
                    printf("M");
                    xt_par0(XT_CH_YELLOW);
                    printf("u");
                    xt_par0(XT_CH_GREEN);
                    printf("L");
                    xt_par0(XT_CH_CYAN);
                    printf("t");
                    xt_par0(XT_CH_BLUE);
                    printf("I");
                    xt_par0(XT_CH_MAGENTA);
                    printf("p");
                    xt_par0(XT_CH_RED);
                    printf("L");
                    xt_par0(XT_CH_YELLOW);
                    printf("a");
                    xt_par0(XT_CH_GREEN);
                    printf("Y");
                    xt_par0(XT_CH_CYAN);
                    printf("e");
                    xt_par0(XT_CH_BLUE);
                    printf("R");
                    break;
                }
                xt_par0(XT_CH_NORMAL);
                break;
            case KEY_ENTER:
                return choice;
            case 'q':
                return MENU_QUIT;
        }
	fflush(stdout);
        usleep(1000000/fps);
        
    }
}

/* Main menu loop. */
static void loop(void) {
    int option;
    while (1) {
        switch ((option = menu())) {
            case MENU_PLAY:
                play();
                break;
            case MENU_SETTINGS:
                configloop();
                break;
            case MULTIPLAYER:
                serverlogin();
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
