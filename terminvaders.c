#include <stdio.h>
// not sure if we're doing delta time algorithm
#include <unistd.h>

#include "terminvaders.h"

// #include <rest-of-files.h>



int main(int argc, char* argv[]) {
	game_t game; // not certain of world constructors etc
	game.running = 1;
	while (game.running) {
		// do something, run the game
		update(&game);

		usleep(16666);

	}
}

int update(game_t* game) {

}
