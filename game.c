#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "graphics.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

void render(game_t* game) {
	int r, c;

	xt_par0(XT_CLEAR_SCREEN);
	for (r = 1; r <= ROWS; r++) {
		xt_par2(XT_SET_ROW_COL_POS, r, 1);
		for (c = 0; c < COLS; c++)
			putchar(game->screen[r][c + 1]);
	}
}

void handle_input() {
	int key;

	while (1) {
		while ((key = getkey()) == KEY_NOTHING);
		switch (key) {
//			case KEY_
		}
	}
}

void update(game_t* game) {
	handle_input(game);
	render(game);
}

void play() {
	game_t game;
	game.running = 1;
	while (game.running) {
		update(&game);
		usleep(US_PER_UPDATE);
	}
}
