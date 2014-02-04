#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "transmit.h"

/* Macros */

#define APPEND(args...) pos += snprintf(buffer + pos, bufsize - pos, args)

#define SCAN(args...) sscanf(buffer + pos, args, &offset); \
    pos += offset;

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
void serialize_lobby_info(client_t* clients, mgame_t* games, char** buffer_ptr) {
    int id, status, pos, i, bufsize = (MAX_CLIENTS + MAX_GAMES) * (NAME_LEN + 20);
    char* buffer = malloc(sizeof(char) * bufsize);

    *buffer_ptr = buffer;
    pos = 0;
    for (id = 0; id < MAX_CLIENTS; id++) {
        status = clients[id].status;
        if (status == CLIENT_FREE || status == CLIENT_CONNECTING)
            continue;
        APPEND("1|%d|%d|%s\n", id, status, clients[id].name);
    }
    APPEND("0|");
    for (id = 0; id < MAX_GAMES; id++) {
        pthread_mutex_lock(&games[id].state_lock);
        status = games[id].status;
        if (status == GAME_FREE) {
            pthread_mutex_unlock(&games[id].state_lock);
            continue;
        }
        APPEND("1|%d|%d|%d|%d|%d|%s\n", id, status, games[id].mode, games[id].slots_total, games[id].slots_filled, games[id].name);
        for (i = 0; i < MAX_SLOTS; i++)
            APPEND("%d|", games[id].players[i]);
        pthread_mutex_unlock(&games[id].state_lock);
    }
    APPEND("0|");
}

/* Unserialize lobby info. */
void unserialize_lobby_info(char* buffer, user_t* users, multiplayergame_t* games) {
    int pos = 0, offset, more, userid, status, gameid, mode, tslots, fslots, i;
    char name[NAME_LEN + 1];

    /* Reset data. */
    for (userid = 0; userid < MAX_CLIENTS; userid++)
        users[userid].status = CLIENT_FREE;
    for (gameid = 0; gameid < MAX_GAMES; gameid++)
        games[gameid].status = GAME_FREE;

    /* Scan users. */
    while (1) {
        SCAN("%d|%n", &more);
        if (!more)
            break;
        SCAN("%d|%d|%s\n%n", &userid, &status, name);
        users[userid].status = status;
        strcpy(users[userid].username, name);
    }

    /* Scan games. */
    while (1) {
        SCAN("%d|%n", &more);
        if (!more)
            break;
        SCAN("%d|%d|%d|%d|%d|%s\n%n", &gameid, &status, &mode, &tslots, &fslots, name);
        games[gameid].status = status;
        games[gameid].mode = mode;
        games[gameid].slots_total = tslots;
        games[gameid].slots_filled = fslots;
        strcpy(games[gameid].name, name);
        for (i = 0; i < MAX_SLOTS; i++)
            SCAN("%d|%n", &games[gameid].players[i]);
    }
}

/* Serialize game data into a malloc()'d buffer. */
void serialize_game_data(game_t* game, char** buffer_ptr) {
    int pos = 0, bufsize = 32768, slot;  // TODO: fix this rough upper bound
    char* buffer = malloc(sizeof(char) * bufsize);
    player_t* player;
    enemy_t* enemy = game->first_enemy;
    bullet_t* bullet = game->first_bullet;
    explosion_t* explosion = game->first_explosion;

    *buffer_ptr = buffer;
    APPEND("%d|%d|%d\n", game->running, game->score, game->over);
    for (slot = 0; slot < game->multiplayer_data.players; slot++) {
        player = &game->players[slot];
        APPEND("%.2lf,%.2lf|%d|%d|%d\n", player->point.x, player->point.y,
               player->lives, player->respawning, player->invincible);
    }
    while (enemy) {
        APPEND("1|%.2lf,%.2lf\n", enemy->point.x, enemy->point.y);
        enemy = enemy->next;
    }
    APPEND("0|");
    while (bullet) {
        APPEND("1|%.2lf,%.2lf|%d|%d\n", bullet->point.x, bullet->point.y,
               bullet->fired_by_player, bullet->type);
        bullet = bullet->next;
    }
    APPEND("0|");
    while (explosion) {
        APPEND("1|%.2lf,%.2lf|%d|%d\n", explosion->point.x, explosion->point.y,
               explosion->step, explosion->score);
        explosion = explosion->next;
    }
    APPEND("0|");
}

/* Unserialize game setup data. */
void unserialize_game_setup(char* buffer, char* name, int* type, int* slots) {
    sscanf(buffer, "%d|%d|%s", type, slots, name);
}

/* Unserialize game data. */
void unserialize_game_data(char* buffer, game_t* game) {
    int pos = 0, offset, slot, more;
    player_t* player;
    enemy_t *enemy;
    bullet_t *bullet;
    explosion_t *explosion;

    /* Destory existing data, which is now invalid. */
    while (game->first_enemy) {
        enemy = game->first_enemy->next;
        free(game->first_enemy);
        game->first_enemy = enemy;
    }
    while (game->first_bullet) {
        bullet = game->first_bullet->next;
        free(game->first_bullet);
        game->first_bullet = bullet;
    }
    while (game->first_explosion) {
        explosion = game->first_explosion->next;
        free(game->first_explosion);
        game->first_explosion = explosion;
    }

    /* Scan known-sized data. */
    SCAN("%d|%d|%d\n%n", &game->running, &game->score, &game->over);
    for (slot = 0; slot < game->multiplayer_data.players; slot++) {
        player = &game->players[slot];
        SCAN("%lf,%lf|%d|%d|%d\n%n", &player->point.x, &player->point.y,
             &player->lives, &player->respawning, &player->invincible);
    }

    /* Scan enemy data. Other fields get dummy values. */
    while (1) {
        SCAN("%d|%n", &more);
        if (!more)
            break;
        enemy = malloc(sizeof(enemy_t));
        *enemy = (enemy_t) {{0}};
        SCAN("%lf,%lf\n%n", &enemy->point.x, &enemy->point.y);
        enemy->next = game->first_enemy;
        game->first_enemy = enemy;
    }

    /* Scan bullet data. Other fields get dummy values. */
    while (1) {
        SCAN("%d|%n", &more);
        if (!more)
            break;
        bullet = malloc(sizeof(bullet_t));
        *bullet = (bullet_t) {{0}};
        SCAN("%lf,%lf|%d|%d\n%n", &bullet->point.x, &bullet->point.y,
             &bullet->fired_by_player, &bullet->type);
        bullet->next = game->first_bullet;
        game->first_bullet = bullet;
    }

    /* Scan explosion data. Other fields get dummy values. */
    while (1) {
        SCAN("%d|%n", &more);
        if (!more)
            break;
        explosion = malloc(sizeof(explosion_t));
        *explosion = (explosion_t) {{0}};
        SCAN("%lf,%lf|%d|%d\n%n", &explosion->point.x, &explosion->point.y,
             &explosion->step, &explosion->score);
        explosion->next = game->first_explosion;
        game->first_explosion = explosion;
    }
}
