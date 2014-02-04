#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "server.h"
#include "network.h"
#include "transmit.h"

/* Set up a multiplayer game. */
void setup_multiplayer(game_t* game, int players, int player, int sockfd) {
    int i;

    game->multiplayer = 1;
    game->multiplayer_data.mode = MODE_CO_OP;
    game->multiplayer_data.players = players;
    game->multiplayer_data.player = player;
    game->multiplayer_data.sockfd = sockfd;

    game->players = realloc(game->players, sizeof(player_t) * players);
    for (i = 1; i < players; i++)
        spawn_player(game, &game->players[i], PLAYER_LIVES);
}

/* Check whether a multiplayer game should end. */
void check_multiplayer_game_over(game_t* game) {
    player_t* peer;

    // for () {
    //     if ()
    //         return;
    // }
    game->over = GAME_OVER_TIMER;
}

/* Load multiplayer data from the server; non-blocking. */
void load_server_data(game_t* game) {
    // TODO: loop while server has data to send (non-blocking loop)
    // TODO: if server has broken the connection, end the game
    // TODO: handle CMD_GAME_UPDATE, CMD_GAME_OVER, CMD_QUIT
}

/* Handle user keyboard input during a multiplayer game. */
void handle_input_multi(game_t* game) {
    int key;
    char tmpbuf[8];

    while ((key = getkey()) != KEY_NOTHING) {
        switch (key) {
            case 'q':
                transmit(game->multiplayer_data.sockfd, CMD_PLAYER_PART, NULL);
                game->running = 0;
                break;
            case KEY_UP:
            case 'w':
                snprintf(tmpbuf, 8, "%d", INPUT_UP);
                transmit(game->multiplayer_data.sockfd, CMD_INPUT, tmpbuf);
                break;
            case KEY_DOWN:
            case 's':
                snprintf(tmpbuf, 8, "%d", INPUT_DOWN);
                transmit(game->multiplayer_data.sockfd, CMD_INPUT, tmpbuf);
                break;
            case KEY_LEFT:
            case 'a':
                snprintf(tmpbuf, 8, "%d", INPUT_LEFT);
                transmit(game->multiplayer_data.sockfd, CMD_INPUT, tmpbuf);
                break;
            case KEY_RIGHT:
            case 'd':
                snprintf(tmpbuf, 8, "%d", INPUT_RIGHT);
                transmit(game->multiplayer_data.sockfd, CMD_INPUT, tmpbuf);
                break;
            case ' ':
                snprintf(tmpbuf, 8, "%d", INPUT_SHOOT);
                transmit(game->multiplayer_data.sockfd, CMD_INPUT, tmpbuf);
                break;
        }
    }
}
