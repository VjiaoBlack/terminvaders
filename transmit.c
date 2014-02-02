#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transmit.h"

/* Serialize status information into databuf. */
void serialize_status_data(client_t* clients, char* databuf) {
    int id, n_clients = 0, n_connecting = 0, n_idle = 0, n_waiting = 0, n_in_game = 0;

    for (id = 0; id < MAX_CLIENTS; id++) {
        if (clients[id].status != CLIENT_FREE)
            n_clients++;
        switch (clients[id].status) {
            case CLIENT_CONNECTING:
                n_connecting++; break;
            case CLIENT_IDLE:
                n_idle++;       break;
            case CLIENT_WAITING:
                n_waiting++;    break;
            case CLIENT_IN_GAME:
                n_in_game++;    break;
        }
    }

    /* This will include the client asking for data, which is bad. */
    n_clients--;
    n_connecting--;

    snprintf(databuf, 1024, "%d/%d (%d/%d/%d/%d)", n_clients, MAX_CLIENTS,
             n_connecting, n_idle, n_waiting, n_in_game);
}

/* Serialize lobby info into a malloc()'d buffer. */
#define APPEND(args...) pos += snprintf(buffer + pos, bufsize - pos, args)
void serialize_lobby_info(client_t* clients, mgame_t* games, char** buffer_ptr) {
    int id, status, pos, i, bufsize = (MAX_CLIENTS + MAX_GAMES) * (NAME_LEN + 20);
    char* buffer = malloc(sizeof(char) * bufsize);

    *buffer_ptr = buffer;
    strcpy(buffer, "[");
    pos = 1;
    for (id = 0; id < MAX_CLIENTS; id++) {
        status = clients[id].status;
        if (status == CLIENT_FREE || status == CLIENT_CONNECTING)
            continue;
        APPEND("{%d.%d.%s}", id, status, clients[id].name);
    }
    APPEND("][");
    for (id = 0; id < MAX_GAMES; id++) {
        status = games[id].status;
        if (status == GAME_FREE)
            continue;
        APPEND("{%d.%d.%d.%d.", id, status, games[id].slots_total, games[id].slots_filled);
        for (i = 0; i < MAX_SLOTS; i++) {
            APPEND("%d", games[id].players[i]);
            if (i < MAX_SLOTS - 1)
                APPEND(",");
        }
        APPEND(".%s}", games[id].name);
    }
    APPEND("]");
}
