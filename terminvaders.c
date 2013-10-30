#include <stdio.h>
#include <unistd.h>
#include "terminvaders.h"

#define US_PER_UPDATE (1000000 / 60)

int update(game_t* game) {
    return 0;
}

void play() {
    game_t game;
    game.running = 1;
    while (game.running) {
        if (update(&game))
            break;
        usleep(US_PER_UPDATE);
    }
}

int main(int argc, char* argv[]) {
    play();
    return 0;
}
