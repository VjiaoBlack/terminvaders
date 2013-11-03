#include <stdio.h>
#include "terminvaders.h"
#include "game.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

/* Start the game by clearing the screen, etc. */
void start(void) {
    xt_par0(XT_CLEAR_SCREEN);
}

/* Main menu: render the menu and return an option selected by the user. */
int menu(void) {
    int key, choice = 0;
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    printf(" Play\n Settings\n Quit\n");
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    while (1) {
        switch ((key = getkey())) {
            case KEY_UP:
            case 'w':
                choice = !choice ? 2 : choice - 1;
                xt_par2(XT_SET_ROW_COL_POS, choice + 1, 1);
                break;
            case KEY_DOWN:
            case 's':
                choice = (choice + 1) % 3;
                xt_par2(XT_SET_ROW_COL_POS, choice + 1, 1);
                break;
            case KEY_ENTER:
                return choice;
            case 'q':
                return MENU_QUIT;
        }
    }
}

/* Main menu loop. */
void loop(void) {
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
void finish(void) {
    getkey_terminate();
    xt_par0(XT_CLEAR_SCREEN);
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
}

int main(int argc, char* argv[]) {
    start();
    loop();
    finish();
    return 0;
}
