#pragma once
#include "game.h"

void setup_multiplayer(game_t*, int, int, int);
void check_multiplayer_game_over(game_t*);
void load_server_data(game_t*);
void handle_input_multi(game_t*);
