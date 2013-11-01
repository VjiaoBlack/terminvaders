#include "terminvaders.h"
#include "game.h"
#include "xterm/xterm_control.h"

int menu(void) {
}

int main(int argc, char* argv[]) {
    int option;

    xt_par0(XT_CLEAR_SCREEN);
    while (1) {
        switch ((option = menu())) {
            case MENU_PLAY:
                break;
            case MENU_SETTINGS:
                break;
            case MENU_QUIT:
                break;
        }
    }
    play();
    xt_par0(XT_CLEAR_SCREEN);
    return 0;
}
