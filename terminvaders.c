#include <stdio.h>
#include <unistd.h>
#include "terminvaders.h"
#include "game.h"
#include "xterm/xterm_control.h"

int menu(void) {
    xt_par2(XT_SET_ROW_COL_POS, 1, 1);
    printf("Testing...\n");
    usleep(2000000);
    return 0;
}

int main(int argc, char* argv[]) {
    int option;

    xt_par0(XT_CLEAR_SCREEN);
    while (1) {
        switch ((option = menu())) {
            case MENU_PLAY:
                play();
            case MENU_SETTINGS:
                break;
            case MENU_QUIT:
                break;
        }
    }
    xt_par0(XT_CLEAR_SCREEN);
    return 0;
}
