#pragma once
#include "server.h"

void serialize_status_data(client_t clients[MAX_CLIENTS], char*);
void serialize_lobby_info(client_t clients[MAX_CLIENTS], char**);
