#pragma once
#include "server.h"

void serialize_status_data(client_t*, mgame_t*, char*);
void serialize_lobby_info(client_t*, mgame_t*, char**);

void unserialize_game_setup(char*, char**, int*, int*);
