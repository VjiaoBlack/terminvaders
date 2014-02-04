#pragma once
#include "server.h"
#include "game.h"

void serialize_status_data(client_t*, mgame_t*, char*);
void serialize_lobby_info(client_t*, mgame_t*, char**);
void serialize_game_data(game_t*, char**);

void unserialize_game_setup(char*, char*, int*, int*);
void unserialize_game_data(char*, game_t*);
