#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "transmit.h"

/* Serialize status information into databuf. */
void serialize_status_data(client_t* clients, mgame_t* games, char* databuf) {
    int id, n_clients = 0, nc_connecting = 0, nc_idle = 0, nc_waiting = 0,
        nc_in_game = 0, n_games = 0, ng_waiting = 0, ng_playing = 0;

    for (id = 0; id < MAX_CLIENTS; id++) {
        if (clients[id].status != CLIENT_FREE)
            n_clients++;
        switch (clients[id].status) {
            case CLIENT_CONNECTING:
                nc_connecting++; break;
            case CLIENT_IDLE:
                nc_idle++;       break;
            case CLIENT_WAITING:
                nc_waiting++;    break;
            case CLIENT_IN_GAME:
                nc_in_game++;    break;
        }
    }
    for (id = 0; id < MAX_GAMES; id++) {
        if (games[id].status != GAME_FREE)
            n_games++;
        switch (games[id].status) {
            case GAME_WAITING:
                ng_waiting++; break;
            case GAME_PLAYING:
                ng_playing++; break;
        }
    }

    /* This will include the client asking for data, which is bad. */
    n_clients--;
    nc_connecting--;

    snprintf(databuf, 1024, "%d/%d (%d/%d/%d/%d)\n%d/%d (%d/%d)", n_clients,
             MAX_CLIENTS, nc_connecting, nc_idle, nc_waiting, nc_in_game,
             n_games, MAX_GAMES, ng_waiting, ng_playing);
}

/* Serialize lobby info into a malloc()'d buffer. */
#define APPEND(args...) pos += snprintf(buffer + pos, bufsize - pos, args)
void serialize_lobby_info(client_t* clients, mgame_t* games, char** buffer_ptr) {
    int id, status, pos, i, bufsize = (MAX_CLIENTS + MAX_GAMES) * (NAME_LEN + 20);
    char* buffer = malloc(sizeof(char) * bufsize);

    *buffer_ptr = buffer;
    pos = 0;
    for (id = 0; id < MAX_CLIENTS; id++) {
        status = clients[id].status;
        if (status == CLIENT_FREE || status == CLIENT_CONNECTING)
            continue;
        APPEND("%d|%d|%s\n", id, status, clients[id].name);
    }
    APPEND("|\n");
    for (id = 0; id < MAX_GAMES; id++) {
        pthread_mutex_lock(&games[id].state_lock);
        status = games[id].status;
        if (status == GAME_FREE) {
            pthread_mutex_unlock(&games[id].state_lock);
            continue;
        }
        APPEND("%d|%d|%d|%d|", id, status, games[id].slots_total, games[id].slots_filled);
        for (i = 0; i < MAX_SLOTS; i++) {
            APPEND("%d", games[id].players[i]);
            if (i < MAX_SLOTS - 1)
                APPEND(",");
        }
        APPEND("|%s\n", games[id].name);
        pthread_mutex_unlock(&games[id].state_lock);
    }
}
