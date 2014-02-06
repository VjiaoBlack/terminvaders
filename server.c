#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "server.h"
#include "transmit.h"
#include "client.h"

/* --------------------------- Global Variables ---------------------------- */

int master_sockfd;
client_t clients[MAX_CLIENTS];
mgame_t games[MAX_GAMES];
pthread_mutex_t new_game_lock = PTHREAD_MUTEX_INITIALIZER;

/* ----------------------------- Helper Macros ----------------------------- */

#define SAFE_TRANSMIT(id, sockfd, command, data) \
    pthread_mutex_lock(&clients[id].send_lock); \
    transmit(sockfd, command, data); \
    pthread_mutex_unlock(&clients[id].send_lock);

#define SAFE_TRANSMIT2(id, command, data) \
    pthread_mutex_lock(&clients[id].send_lock); \
    transmit(clients[id].sockfd, command, data); \
    pthread_mutex_unlock(&clients[id].send_lock);

#define ENFORCE_CONTEXT(desired_status) \
    if (clients[id].status != desired_status) { \
        free(buffer); \
        SAFE_TRANSMIT(id, sockfd, CMD_QUIT, ERR_BAD_CONTEXT); \
        goto cleanup; \
    }

/* --------------------------- Helper Functions ---------------------------- */

/* Return whether or not the server is currently running. */
static int is_running(void) {
    int sockfd = make_connection("localhost");

    if (sockfd > 0)
        close(sockfd);
    return sockfd > 0;
}

/* Get the PID of the server, assuming it is running. */
static pid_t get_server_pid(void) {
    int sockfd = make_connection("localhost");
    char resp[16] = "\0";

    if (sockfd < 0)
        return -1;
    if (transmit(sockfd, CMD_GETPID, NULL) < 0)
        return -1;
    read(sockfd, resp, 16);
    return atol(resp);
}

/* Shutdown the server by sending a quit message to all clients and exiting. */
static void stop_server(int retval) {
    int id;

    for (id = 0; id < MAX_CLIENTS; id++) {
        switch (clients[id].status) {
            case CLIENT_FREE:
                break;
            case CLIENT_CONNECTING:
            case CLIENT_IDLE:
            case CLIENT_WAITING:
            case CLIENT_IN_GAME:
                pthread_mutex_lock(&clients[id].send_lock);
                transmit(clients[id].sockfd, CMD_QUIT, ERR_SERVER_STOPPING);
                close(clients[id].sockfd);
                pthread_mutex_unlock(&clients[id].send_lock);
        }
    }
    close(master_sockfd);
    exit(retval);
}

/* Catch signals in order to safely close client connections. */
static void catch_signal(int sig) {
    if (sig == SIGTERM)
        stop_server(0);
}

/* Set up the server connection but do not enter the main loop. */
static void start_server(void) {
    int sockfd, id;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error: cannot create socket");
        exit(1);
    }

    /* Create socket structure. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error: couldn't bind()");
        exit(1);
    }
    if (listen(sockfd, 16) < 0) {
        perror("Error: couldn't listen()");
        exit(1);
    }

    /* Set up remaining data. */
    srand(time(NULL));
    for (id = 0; id < MAX_CLIENTS; id++) {
        clients[id] = (client_t) {id, CLIENT_FREE};
        clients[id].request = NO_REQUEST;
        pthread_mutex_init(&clients[id].send_lock, NULL);
    }
    for (id = 0; id < MAX_GAMES; id++) {
        games[id] = (mgame_t) {id, GAME_FREE};
        pthread_mutex_init(&games[id].state_lock, NULL);
        pthread_mutex_init(&games[id].input_buffer.lock, NULL);
    }
    if (signal(SIGTERM, catch_signal) == SIG_ERR) {
        printf("Error: couldn't attach a signal handler.\n");
        exit(1);
    }
    master_sockfd = sockfd;
}

/* Daemonize the current running process, for after starting the server. */
static void daemonize(void) {
    /* https://en.wikipedia.org/wiki/Daemon_(computing)#Creation */
    pid_t pid = fork();

    if (pid < 0) {
        printf("error: couldn't fork.\n");
        exit(1);
    }
    if (pid > 0)
        exit(0);  /* Forked correctly: exit parent. */
    umask(0);
    if (setsid() < 0) {
        printf("error: couldn't create a new session.\n");
        exit(1);
    }
    if ((chdir("/")) < 0) {
        printf("error: couldn't chdir(\"/\").\n");
        exit(1);
    }
    printf("done.\n");
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

/* Get a free client ID. Return -1 if none are available. */
static int get_free_client_id(void) {
    int id;

    for (id = 0; id < MAX_CLIENTS; id++) {
        if (clients[id].status == CLIENT_FREE)
            return id;
    }
    return -1;
}

/* Get a free game ID. Return -1 if none are available. */
static int get_free_game_id(void) {
    int id;

    for (id = 0; id < MAX_GAMES; id++) {
        if (games[id].status == GAME_FREE)
            return id;
    }
    return -1;
}

/* Return whether or not the given client is connected from localhost. */
static int is_local_client(int id) {
    char cname[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &clients[id].addr.sin_addr, cname, INET_ADDRSTRLEN);
    return strcmp(cname, "127.0.0.1") == 0;
}

/* Cancel any pending requests for a game. */
static void cancel_requests(int game_id) {
    int id;

    for (id = 0; id < MAX_CLIENTS; id++) {
        if (clients[id].request == game_id) {
            SAFE_TRANSMIT2(id, CMD_REJECT_REQ, NULL);
            clients[id].request = NO_REQUEST;
        }
    }
}

/* Get the game slot that corresponds to a client ID. */
static int get_slot_for_client(int game_id, int player) {
    int slot;

    for (slot = 0; slot < games[game_id].slots_total; slot++) {
        if (games[game_id].players[slot] == player)
            return slot;
    }
    return EMPTY_SLOT;
}

/* Handle the logic in a running game. This runs in its own thread. */
static void* handle_game(void* arg) {
    int id = *((int*) arg), action, player, slot;
    input_t* input;
    char *buffer;

    /* Set up the game and enter the logic loop. */
    setup_game(&games[id].data);
    setup_multiplayer(&games[id].data, games[id].slots_total, -1, -1);
    while (games[id].data.running) {
        pthread_mutex_lock(&games[id].state_lock);
        if (games[id].status != GAME_PLAYING) {  /* All players have left. */
            pthread_mutex_unlock(&games[id].state_lock);
            return NULL;
        }
        do_logic(&games[id].data);

        /* Handle unprocessed player input. */
        pthread_mutex_lock(&games[id].input_buffer.lock);
        while (games[id].input_buffer.first != NULL) {
            /* Pop the first input item off the stack. */
            input = games[id].input_buffer.first;
            player = input->player;
            action = input->action;
            games[id].input_buffer.first = input->next;
            free(input);

            /* Process the input in the context of its sender. */
            slot = get_slot_for_client(id, player);
            if (slot == EMPTY_SLOT)
                continue;
            handle_serializable_input(&games[id].data, slot, action);
        }
        pthread_mutex_unlock(&games[id].input_buffer.lock);

        /* Update all players with the current game status. */
        serialize_game_data(&games[id].data, &buffer);
        for (slot = 0; slot < games[id].slots_total; slot++) {
            player = games[id].players[slot];
            if (player == EMPTY_SLOT)
                continue;
            SAFE_TRANSMIT2(player, CMD_GAME_UPDATE, buffer);
        }

        pthread_mutex_unlock(&games[id].state_lock);
        free(buffer);
        usleep(1000000 / FPS);
    }

    /* Game has ended; inform remaining players. */
    pthread_mutex_lock(&games[id].state_lock);
    for (slot = 0; slot < games[id].slots_total; slot++) {
        player = games[id].players[slot];
        if (player == EMPTY_SLOT)
            continue;
        SAFE_TRANSMIT2(player, CMD_GAME_OVER, NULL);
        clients[player].status = CLIENT_IDLE;
    }
    games[id].status = GAME_FREE;
    cancel_requests(id);
    pthread_mutex_unlock(&games[id].state_lock);
    return NULL;
}

/* Start a game pthread. */
static void start_game_thread(int id) {
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&games[id].thread, &attr, handle_game, (void*) &id) < 0) {
        pthread_attr_destroy(&attr);
        stop_server(1);
    }
    pthread_attr_destroy(&attr);
}

/* Remove a player from a game; notify remaining players or end the game. */
static void remove_player(int id, int game_id) {
    int slot;

    /* Update game slots data. */
    pthread_mutex_lock(&games[game_id].state_lock);
    slot = get_slot_for_client(game_id, id);
    if (slot < 0) {
        /* Something is very wrong, but I'm not sure how to handle it. Running
           around like a headless chicken is ill-advised, so let's pretend this
           never happened. */
        pthread_mutex_unlock(&games[game_id].state_lock);
        return;
    }
    games[game_id].players[slot] = EMPTY_SLOT;
    games[game_id].slots_filled--;

    /* If there are players left, kill the player. */
    if (games[game_id].slots_filled > 0) {
        games[game_id].data.players[slot].lives = 0;
        games[game_id].data.players[slot].respawning = 1;
    }
    /* If that was the last player, end the game. */
    else {
        games[game_id].status = GAME_FREE;
        cancel_requests(game_id);
    }
    pthread_mutex_unlock(&games[game_id].state_lock);
}

/* Process CMD_SETUP_GAME. */
static void process_setup_game(int id, int sockfd, char* buffer) {
    int game_id, game_mode, slots_total, slot;
    char name[NAME_LEN + 1], tempbuf[8];

    /* Load the game data and try to fetch a free ID. */
    unserialize_game_setup(buffer, name, &game_mode, &slots_total);
    pthread_mutex_lock(&new_game_lock);
    if ((game_id = get_free_game_id()) < 0) {
        pthread_mutex_unlock(&new_game_lock);
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_FULL);
        return;
    }

    /* Populate the game's struct. */
    games[game_id].slots_total = slots_total;
    games[game_id].slots_filled = 1;
    games[game_id].players[0] = id;
    for (slot = 1; slot < slots_total; slot++)
        games[game_id].players[slot] = EMPTY_SLOT;
    strcpy(games[game_id].name, name);
    games[game_id].mode = game_mode;
    games[game_id].status = GAME_WAITING;
    pthread_mutex_unlock(&new_game_lock);

    /* Populate the game's admin's struct and inform them of the creation. */
    clients[id].game = game_id;
    clients[id].status = CLIENT_WAITING;
    snprintf(tempbuf, 8, "%d", id);
    SAFE_TRANSMIT(id, sockfd, CMD_PLAYER_JOIN, tempbuf);
}

/* Process CMD_JOIN_GAME. */
static void process_join_game(int id, int sockfd, char* buffer) {
    int game_id, admin;
    char tempbuf[8];

    /* Check for an old pending request; check for validity of new one. */
    if (clients[id].request != NO_REQUEST) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_ALREADY_PENDING);
        return;
    }
    game_id = atoi(buffer);
    if (game_id < 0 || game_id >= MAX_GAMES) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_BAD_ID);
        return;
    }

    /* Reject if game isn't waiting for new players, or if it's full. */
    pthread_mutex_lock(&games[game_id].state_lock);
    if (games[game_id].status != GAME_WAITING) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, (games[game_id].status == GAME_PLAYING) ? ERR_GAME_STARTED : ERR_BAD_ID);
        return;
    }
    if (games[game_id].slots_filled >= games[game_id].slots_total) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_GAME_FULL);
        return;
    }

    /* Inform the game admin of the new request. */
    clients[id].request = game_id;
    admin = games[game_id].players[0];
    snprintf(tempbuf, 8, "%d", id);
    SAFE_TRANSMIT2(admin, CMD_NEW_REQUEST, tempbuf);
    pthread_mutex_unlock(&games[game_id].state_lock);
}

/* Process CMD_CANCEL_REQ. */
static void process_cancel_req(int id, int sockfd) {
    int game_id = clients[id].request, admin;
    char tempbuf[8];

    /* Ensure the client has a pending request to cancel. */
    if (game_id == -1) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_NO_REQUEST);
        return;
    }

    /* Invalidate the request and inform the admin if necessary. */
    clients[id].request = NO_REQUEST;
    pthread_mutex_lock(&games[game_id].state_lock);
    if (games[game_id].status == GAME_WAITING) {
        admin = games[game_id].players[0];
        snprintf(tempbuf, 8, "%d", id);
        SAFE_TRANSMIT2(admin, CMD_CANCEL_REQ, tempbuf);
    }
    pthread_mutex_unlock(&games[game_id].state_lock);
}

/* Process CMD_ACCEPT_REQ. */
static void process_accept_req(int id, int sockfd, char* buffer) {
    int game_id = clients[id].game, requester, slot, player;
    char tempbuf[8], tempbuf2[8];

    /* Only admins can accept requests; ensure request is valid. */
    if (id != games[game_id].players[0]) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_NOT_ADMIN);
        return;
    }
    requester = atoi(buffer);
    if (clients[requester].request != game_id) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_NO_REQUEST);
        return;
    }

    /* Remove the pending request and inform the requester; update struct. */
    clients[requester].request = NO_REQUEST;
    SAFE_TRANSMIT2(requester, CMD_ACCEPT_REQ, NULL);
    clients[requester].status = CLIENT_WAITING;
    clients[requester].game = game_id;

    /* Inform current players of the new guy and vice versa. */
    snprintf(tempbuf, 8, "%d", requester);
    pthread_mutex_lock(&games[game_id].state_lock);
    for (slot = 0; slot < games[game_id].slots_total; slot++) {
        player = games[game_id].players[slot];
        if (player == EMPTY_SLOT)
            continue;
        snprintf(tempbuf2, 8, "%d", player);
        SAFE_TRANSMIT2(player, CMD_PLAYER_JOIN, tempbuf);
        SAFE_TRANSMIT2(requester, CMD_PLAYER_JOIN, tempbuf2);
    }
    SAFE_TRANSMIT2(requester, CMD_PLAYER_JOIN, tempbuf);

    /* Fill an empty slot with the new player. */
    for (slot = 0; slot < games[game_id].slots_total; slot++) {
        if (games[game_id].players[slot] == EMPTY_SLOT) {
            games[game_id].players[slot] = requester;
            break;
        }
    }
    games[game_id].slots_filled++;

    /* If we have enough players now, start the game. */
    if (games[game_id].slots_filled >= games[game_id].slots_total) {
        games[game_id].status = GAME_PLAYING;

        /* Inform players of the game start. */
        for (slot = 0; slot < games[game_id].slots_total; slot++) {
            player = games[game_id].players[slot];
            if (player == EMPTY_SLOT)
                continue;
            clients[player].status = CLIENT_IN_GAME;
            snprintf(tempbuf2, 8, "%d", slot);
            SAFE_TRANSMIT2(player, CMD_GAME_START, tempbuf2);
        }
        cancel_requests(game_id);
        start_game_thread(game_id);
    }
    pthread_mutex_unlock(&games[game_id].state_lock);
}

/* Process CMD_REJECT_REQ. */
static void process_reject_req(int id, int sockfd, char* buffer) {
    int game_id = clients[id].game, requester;

    /* Only admins can accept requests; ensure request is valid. */
    if (id != games[game_id].players[0]) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_NOT_ADMIN);
        return;
    }
    requester = atoi(buffer);
    if (clients[requester].request != game_id) {
        SAFE_TRANSMIT(id, sockfd, CMD_ERROR, ERR_NO_REQUEST);
        return;
    }

    /* Remove the pending request and inform the requester. */
    clients[requester].request = NO_REQUEST;
    SAFE_TRANSMIT2(requester, CMD_REJECT_REQ, NULL);
}

/* Process CMD_PLAYER_PART. */
static void process_player_part(int id) {
    clients[id].status = CLIENT_IDLE;
    remove_player(id, clients[id].game);
}

/* Process CMD_INPUT. */
static void process_input(int id, int sockfd, char* buffer) {
    int game_id = clients[id].game;
    input_t* input = malloc(sizeof(input_t));

    /* Lock the input buffer, create a new node, and push it to the top. */
    pthread_mutex_lock(&games[game_id].input_buffer.lock);
    input->player = id;
    input->action = atoi(buffer);
    input->next = games[game_id].input_buffer.first;
    games[game_id].input_buffer.first = input;
    pthread_mutex_unlock(&games[game_id].input_buffer.lock);
}

/* Clean up data involving a particular client after their socket is closed. */
static void cleanup_client(int id) {
    int game_id, admin;
    char tempbuf[8];

    /* If client has a pending request: invalidate it and inform admin. */
    if (clients[id].status == CLIENT_IDLE && clients[id].request != -1) {
        game_id = clients[id].request;
        clients[id].request = NO_REQUEST;
        pthread_mutex_lock(&games[game_id].state_lock);
        if (games[game_id].status == GAME_WAITING) {
            admin = games[game_id].players[0];
            snprintf(tempbuf, 8, "%d", id);
            SAFE_TRANSMIT2(admin, CMD_CANCEL_REQ, tempbuf);

        }
        pthread_mutex_unlock(&games[game_id].state_lock);
    }

    /* If client is in a game, act as if they've parted. */
    else if (clients[id].status == CLIENT_WAITING || clients[id].status == CLIENT_IN_GAME)
        remove_player(id, clients[id].game);

    clients[id].status = CLIENT_FREE;
}

/* Handle a connection with a client. This runs in its own thread. */
static void* handle_client(void* arg) {
    char* buffer, databuf[1024];
    int id = *((int*) arg), sockfd = clients[id].sockfd, command;

    if (receive(sockfd, &command, &buffer) < 0)
        goto cleanup;

    switch (command) {
        case CMD_CONNECT:
            if (strlen(buffer) < 2 || strlen(buffer) > NAME_LEN) {
                free(buffer);
                transmit(sockfd, CMD_QUIT, ERR_BAD_NAME);
                goto cleanup;
            }
            strcpy(clients[id].name, buffer);
            free(buffer);
            clients[id].status = CLIENT_IDLE;
            snprintf(databuf, 1024, "%d", id);
            transmit(sockfd, CMD_CONNECT, databuf);
            break;
        case CMD_GETPID:
            free(buffer);
            if (is_local_client(id)) {
                snprintf(databuf, 1024, "%d", getpid());
                write(sockfd, databuf, strlen(databuf));
            }
            else
                transmit(sockfd, CMD_QUIT, ERR_UNAUTHORIZED);
            goto cleanup;
        case CMD_STATUS:
            free(buffer);
            if (is_local_client(id)) {
                serialize_status_data(&clients[0], &games[0], databuf);
                write(sockfd, databuf, strlen(databuf));
            }
            else
                transmit(sockfd, CMD_QUIT, ERR_UNAUTHORIZED);
            goto cleanup;
        default:
            free(buffer);
            transmit(sockfd, CMD_QUIT, ERR_INVALID);
            goto cleanup;
    }

    while (1) {
        if (receive(sockfd, &command, &buffer) < 0)
            goto cleanup;
        switch (command) {
            case CMD_LOBBY_INFO:
                ENFORCE_CONTEXT(CLIENT_IDLE);
                free(buffer);
                serialize_lobby_info(&clients[0], &games[0], &buffer);
                SAFE_TRANSMIT(id, sockfd, CMD_LOBBY_INFO, buffer);
                break;
            case CMD_SETUP_GAME:
                ENFORCE_CONTEXT(CLIENT_IDLE);
                process_setup_game(id, sockfd, buffer);
                break;
            case CMD_JOIN_GAME:
                ENFORCE_CONTEXT(CLIENT_IDLE);
                process_join_game(id, sockfd, buffer);
                break;
            case CMD_CANCEL_REQ:
                ENFORCE_CONTEXT(CLIENT_IDLE);
                process_cancel_req(id, sockfd);
                break;
            case CMD_ACCEPT_REQ:
                ENFORCE_CONTEXT(CLIENT_WAITING);
                process_accept_req(id, sockfd, buffer);
                break;
            case CMD_REJECT_REQ:
                ENFORCE_CONTEXT(CLIENT_WAITING);
                process_reject_req(id, sockfd, buffer);
                break;
            case CMD_PLAYER_PART:
                ENFORCE_CONTEXT(CLIENT_WAITING && clients[id].status != CLIENT_IN_GAME);
                process_player_part(id);
                break;
            case CMD_INPUT:
                ENFORCE_CONTEXT(CLIENT_IN_GAME);
                process_input(id, sockfd, buffer);
                break;
            default:
                free(buffer);
                SAFE_TRANSMIT(id, sockfd, CMD_QUIT, ERR_INVALID);
                goto cleanup;
        }
        free(buffer);
    }

    cleanup:
    close(sockfd);
    cleanup_client(id);
    return NULL;
}

/* Main server loop which listens for and handles connections. */
static void serve() {
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int client_fd, id;
    pthread_attr_t attr;

    client_len = sizeof(client_addr);
    while (1) {
        if ((client_fd = accept(master_sockfd, (struct sockaddr*) &client_addr, &client_len)) < 0)
            stop_server(1);
        if ((id = get_free_client_id()) < 0) {
            transmit(client_fd, CMD_QUIT, ERR_FULL);
            close(client_fd);
        }

        clients[id].status = CLIENT_CONNECTING;
        clients[id].sockfd = client_fd;
        clients[id].addr = client_addr;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&clients[id].thread, &attr, handle_client, (void*) &id) < 0) {
            pthread_attr_destroy(&attr);
            transmit(client_fd, CMD_QUIT, ERR_SERVER_STOPPING);
            clients[id].status = CLIENT_FREE;
            stop_server(1);
        }
        pthread_attr_destroy(&attr);
    }
}

/* ------------------------------- Commands -------------------------------- */

static void do_help(char* name) {
    printf("%s [status|start|stop]\n", name);
}

static void do_status(void) {
    int sockfd = make_connection("localhost");
    int n_clients, nc_max, nc_connecting, nc_idle, nc_waiting, nc_in_game,
        n_games, ng_max, ng_waiting, ng_playing;
    char resp[1024];

    printf("Terminvaders server: ");
    if (sockfd < 0) {
        printf("offline.\n");
        return;
    }
    printf("running on port %d.\n", PORT);

    if (transmit(sockfd, CMD_STATUS, NULL) < 0)
        return;
    read(sockfd, resp, 1024);
    close(sockfd);
    sscanf(resp, "%d/%d (%d/%d/%d/%d)\n%d/%d (%d/%d)", &n_clients, &nc_max,
           &nc_connecting, &nc_idle, &nc_waiting, &nc_in_game, &n_games,
           &ng_max, &ng_waiting, &ng_playing);

    printf("%d users connected (%d max)\n", n_clients, nc_max);
    if (nc_connecting > 0)
        printf("  %d users connecting\n", nc_connecting);
    if (nc_idle > 0)
        printf("  %d users idling in the lobby\n", nc_idle);
    if (nc_waiting > 0)
        printf("  %d users waiting for a game to start\n", nc_waiting);
    if (nc_in_game > 0)
        printf("  %d users in a game\n", nc_in_game);

    printf("%d games active (%d max)\n", n_games, ng_max);
    if (ng_waiting > 0)
        printf("  %d games waiting for players\n", ng_waiting);
    if (ng_playing > 0)
        printf("  %d games in progress\n", ng_playing);
}

static void do_start(void) {
    if (is_running()) {
        printf("Error: the server is already running!\n");
        exit(1);
    }
    start_server();
    printf("Terminvaders server listening on port %d.\n", PORT);
    printf("Daemonizing... ");
    fflush(stdout);
    daemonize();
    serve();
}

static void do_stop(void) {
    pid_t pid;

    if (!is_running()) {
        printf("Error: the server not running!\n");
        exit(1);
    }
    printf("Stopping server... ");
    fflush(stdout);
    if ((pid = get_server_pid()) < 0) {
        perror("error: couldn't get PID of server");
        exit(1);
    }
    if (kill(pid, SIGTERM)) {
        if (errno == EPERM)
            printf("error: no permission to kill(%d).\n", pid);
        else
            printf("error: can't kill(%d). Already stopped?\n", pid);
        exit(1);
    }

    while (1) {  /* Poll until server quits. */
        if (kill(pid, 0)) {
            if (errno == ESRCH) {
                printf("stopped.\n");
                return;
            }
            perror("error");
            exit(1);
        }
        usleep(500);  /* Try again in half a second. */
    }
}

/* --------------------------------- Main ---------------------------------- */

int main(int argc, char* argv[]) {
    if (argc < 2)
        do_help(argv[0]);
    else if (!strcmp(argv[1], "status"))
        do_status();
    else if (!strcmp(argv[1], "start"))
        do_start();
    else if (!strcmp(argv[1], "stop"))
        do_stop();
    else {
        do_help(argv[0]);
        return 1;
    }
    return 0;
}
