#include <stdio.h>
#include <unistd.h>
#include "terminvaders.h"

int main(int argc, char* argv[]) {
	play();
} 

int play() {
	game_t game;  // not certain of world constructors etc
	game.running = 1;
	while (game.running) {
		// do something, run the game
		update(&game);
		usleep(16666);
	}
}

int update(game_t* game) {

}
