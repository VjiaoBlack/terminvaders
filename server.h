#pragma once
#include <sys/types.h>
#include <netinet/in.h>
#include "graphics.h"
#include "network.h"

/* Defines */

#define MAX_CLIENTS 64
#define MAX_GAMES 32
#define MAX_SLOTS 4

#define CLIENT_FREE 0
#define CLIENT_CONNECTING 1
#define CLIENT_IDLE 2
#define CLIENT_WAITING 3
#define CLIENT_IN_GAME 4

#define GAME_FREE 0
#define GAME_WAITING 1
#define GAME_PLAYING 2

/* Structs */

struct client_t {
    int id;
    int status;
    int sockfd;
    int game;
    char name[NAME_LEN + 1];
    struct sockaddr_in addr;
    pthread_t thread;
    pthread_mutex_t mutex;
};
typedef struct client_t client_t;

struct mgame_t {
    int id;
    int status;
    int slots_total;
    int slots_filled;
    int players[MAX_SLOTS];
    char name[NAME_LEN + 1];
};
typedef struct mgame_t mgame_t;
