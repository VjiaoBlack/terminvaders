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

/* --------------------------- Global Variables ---------------------------- */

int master_sockfd;
client_t clients[MAX_CLIENTS];
mgame_t games[MAX_GAMES];

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
                pthread_mutex_lock(&clients[id].mutex);
                transmit(clients[id].sockfd, CMD_QUIT, ERR_SERVER_STOPPING);
                close(clients[id].sockfd);
                pthread_mutex_unlock(&clients[id].mutex);
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

    for (id = 0; id < MAX_CLIENTS; id++) {
        clients[id] = (client_t) {id, CLIENT_FREE};
        pthread_mutex_init(&clients[id].mutex, NULL);
    }
    for (id = 0; id < MAX_GAMES; id++)
        games[id] = (mgame_t) {id, GAME_FREE};
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

/* Return whether or not the given client is connected from localhost. */
static int is_local_client(int id) {
    char cname[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &clients[id].addr.sin_addr, cname, INET_ADDRSTRLEN);
    return strcmp(cname, "127.0.0.1") == 0;
}

/* Handle a connection with a client. */
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
                serialize_status_data(&clients[0], databuf);
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
            case CMD_LOBBYINFO:
                free(buffer);
                serialize_lobby_info(&clients[0], &games[0], &buffer);
                pthread_mutex_lock(&clients[id].mutex);
                transmit(sockfd, CMD_LOBBYINFO, buffer);
                pthread_mutex_unlock(&clients[id].mutex);
                break;
            default:
                free(buffer);
                transmit(sockfd, CMD_QUIT, ERR_INVALID);
                goto cleanup;
        }
        free(buffer);
    }

    cleanup:
    close(sockfd);
    clients[id].status = CLIENT_FREE;
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
        if ((client_fd = accept(master_sockfd, (struct sockaddr*) &client_addr, &client_len)) < 0) {
            close(master_sockfd);
            stop_server(1);
        }
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
            close(master_sockfd);
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
    int n_clients, n_max, n_connecting, n_idle, n_waiting, n_in_game;
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
    sscanf(resp, "%d/%d (%d/%d/%d/%d)", &n_clients, &n_max, &n_connecting,
           &n_idle, &n_waiting, &n_in_game);

    printf("%d users connected (%d max)\n", n_clients, n_max);
    if (n_connecting > 0)
        printf("  %d users connecting\n", n_connecting);
    if (n_idle > 0)
        printf("  %d users idling in the lobby\n", n_idle);
    if (n_waiting > 0)
        printf("  %d users waiting for a game to start\n", n_waiting);
    if (n_in_game > 0)
        printf("  %d users in a game\n", n_in_game);
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
