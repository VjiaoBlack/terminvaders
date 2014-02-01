#include <sys/types.h>
#include <netinet/in.h>
#include "graphics.h"
#include "network.h"

/* Defines */

#define MAX_CLIENTS 64
#define MAX_GAMES 32

#define CLIENT_FREE 0
#define CLIENT_CONNECTING 1
#define CLIENT_IDLE 2
#define CLIENT_WAITING 3
#define CLIENT_IN_GAME 4

/* Structs */

struct client_t {
    int id;
    int status;
    int sockfd;
    struct sockaddr_in addr;
    char name[NAME_LEN + 1];
    pthread_t thread;
    pthread_mutex_t mutex;
};
typedef struct client_t client_t;
