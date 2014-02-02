#pragma once

/* Defines */

#define PORT 6413
#define PROTOCOL_VER 1
#define NAME_LEN 32

#define CMD_QUIT 0           /* C <-- S (server disconnecting client with a reason) */
#define CMD_ERROR 1          /* C <-- S (server reporting error with client request) */

#define CMD_STATUS 2         /* C --> S (local client requesting server status) */
#define CMD_GETPID 3         /* C --> S (local client requesting server process ID) */

#define CMD_CONNECT 4        /* C --> S (client connecting to server lobby) */
#define CMD_LOBBY_INFO 5     /* C <-> S (client requesting lobby info; server replying) */
#define CMD_SETUP_GAME 6     /* C --> S (client in lobby trying to set up a new game) */
#define CMD_JOIN_GAME 7      /* C --> S (client in lobby trying to join a game) */

#define CMD_NEW_REQUEST 8    /* C <-- S (server informing client of a join request) */
#define CMD_ACCEPT_REQ 9     /* C --> S (client accepting a game join request) */
#define CMD_REJECT_REQ 10    /* C --> S (client rejecting a game join request) */
#define CMD_PLAYER_JOIN 11   /* C <-- S (server informing client that a player joined) */
#define CMD_PLAYER_PART 12   /* C <-> S (client leaving game; server informing other clients) */
#define CMD_GAME_ABORT 13    /* C <-- S (server informing client of a game being aborted) */
#define CMD_GAME_START 14    /* C <-- S (server informing client of game start) */

#define CMD_GAME_UPDATE 15   /* C <-- S (server sending client game update info) */
#define CMD_INPUT 16         /* C --> S (client informing server of user input) */
#define CMD_GAME_OVER 17     /* C <-- S (server informing client of game over) */

#define ERR_FULL "The server is full."
#define ERR_SERVER_STOPPING "The server is shutting down."
#define ERR_UNAUTHORIZED "Only a local client can issue this command."
#define ERR_INVALID "Invalid command."
#define ERR_BAD_CONTEXT "You issued a valid command in an invalid context."
#define ERR_BAD_NAME "The name you entered is invalid."
#define ERR_KICKED "You have been kicked from the server."

/* Functions */

int make_connection(char*);
int transmit(int, int, char*);
int receive(int, int*, char**);
