#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include "settings.h"
#include "terminvaders.h"

struct user_t {
    char* username;
};
typedef struct user_t user_t;

struct multiplayergame_t {
    int valid;
    char* name;
    int max_users;
    int current_users;
    int type;
    struct user_t* users;
//    struct lobby_t* next;
//    struct lobby_t* previous;
};
typedef struct multiplayergame_t multiplayergame_t;

void dispmultiframe();
int game();
void drawgame(int);
void printgame(int,int,multiplayergame_t);
int join_popup (multiplayergame_t);
int create_popup ();
void help_popup ();
void game_wait(int);
int lobby();
void serverlogin();
int confirm_request(user_t);


/* notes:
    display all palyers in game
    also the whitespace at the bottom; bottom entry should be disp at bottom


    multiplayer button -> name and server entry -> the mainscreen

    also make the invite / accept thign actualy.


*/



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
