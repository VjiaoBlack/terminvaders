#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transmit.h"

/* Serialize status information into databuf. */
void serialize_status_data(client_t clients[MAX_CLIENTS], char* databuf) {
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
void serialize_lobby_info(client_t clients[MAX_CLIENTS], char** buffer_ptr) {
    int id, status, pos, bufsize = (MAX_CLIENTS + MAX_GAMES) * (NAME_LEN + 20);
    char* buffer = malloc(sizeof(char) * bufsize);

    *buffer_ptr = buffer;
    strcpy(buffer, "[");
    pos = 1;
    for (id = 0; id < MAX_CLIENTS; id++) {
        status = clients[id].status;
        if (status == CLIENT_FREE || status == CLIENT_CONNECTING)
            continue;
        pos += snprintf(buffer + pos, bufsize - pos, "{%d.%d.%s}", id, status, clients[id].name);
    }
    pos += snprintf(buffer + pos, bufsize - pos, "][");
    // game data
    pos += snprintf(buffer + pos, bufsize - pos, "]");
}
