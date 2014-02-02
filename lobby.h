#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xterm/xterm_control.c"
#include "terminvaders.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BLITZ 2
#define TEAM 1
#define DUEL 0

//duplicated code
#define MAX_GAMES 32
#define MAX_CLIENTS 64



struct user_t {
    char* username;
};
typedef struct user_t user_t;

struct game_t {
    int valid;
    char* name;
    int max_users;
    int current_users;
    int type;
    struct user_t* users;
//    struct lobby_t* next;
//    struct lobby_t* previous;
};
typedef struct game_t game_t;

void dispframe();
int game();
void drawgame(int);
void printgame(int,int,game_t);
void join_popup (game_t);
void create_popup ();
void help_popup ();



// 'lobby' shud rly be 'game'!!
/*
if u create a game
u go to a popup
select name, number of players, a mode
then go to a "waiting" screeen

and u can confirm/ reject invites

when players are full u just start game (as soon as u confirm last plaer)

after game over, quit to LOBBY


every 3 secconds make a call to server to get user invo for lobby

"so when connecting to the server (after hitting multiplayer on main menu + inputting server name in a box but before the lobby is displayed) you call int sockfd = make_connection(char* server_name); which is defined in network.h"

then to get user info, keep sockfd somewhere
then 
transmit(sockfd, CMD_CONNECT, user_name); to tell server ur username

transmit(sockfd, CMD_USERS, NULL) to get a user LIST
*/
