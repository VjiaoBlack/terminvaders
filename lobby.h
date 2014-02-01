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

void dispframe();
int lobby();
void drawlobby(int);

struct user_t {
    char* username;
};
typedef struct user_t user_t;

struct lobby_t {
    char* name;
    int max_users;
    int current_users;
    int type;
    struct user_t* users;
    struct lobby_t* next;
    struct lobby_t* previous;
};
typedef struct lobby_t lobby_t;

