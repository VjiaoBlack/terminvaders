#pragma once
#include <sys/types.h>
#include <netinet/in.h>
#include "graphics.h"
#include "network.h"
#include "game.h"

/* Defines */

#define MAX_CLIENTS 64
#define MAX_GAMES 32
#define MAX_SLOTS 4
#define EMPTY_SLOT -1
#define NO_REQUEST -1

#define MODE_TEAM 0

#define CLIENT_FREE 0
#define CLIENT_CONNECTING 1
#define CLIENT_IDLE 2
#define CLIENT_WAITING 3
#define CLIENT_IN_GAME 4

#define GAME_FREE 0
#define GAME_WAITING 1
#define GAME_PLAYING 2

#define INPUT_UP 0
#define INPUT_DOWN 1
#define INPUT_LEFT 2
#define INPUT_RIGHT 3
#define INPUT_SHOOT 4

/* Structs */

struct client_t {
    int id;
    int status;
    int sockfd;
    int game;
    char name[NAME_LEN + 1];
    int request;
    struct sockaddr_in addr;
    pthread_t thread;
    pthread_mutex_t send_lock;
};
typedef struct client_t client_t;

struct input_t {
    int player;
    int action;
    struct input_t* next;
};
typedef struct input_t input_t;

struct inptbuf_t {
    input_t* first;
    pthread_mutex_t lock;
};
typedef struct inptbuf_t inptbuf_t;

struct mgame_t {
    int id;
    int status;
    int slots_total;
    int slots_filled;
    int players[MAX_SLOTS];
    char name[NAME_LEN + 1];
    int mode;
    game_t data;
    pthread_t thread;
    inptbuf_t input_buffer;
    pthread_mutex_t state_lock;
};
typedef struct mgame_t mgame_t;
