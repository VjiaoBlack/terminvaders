#include <errno.h>
#include "lobby.h"
#include "server.h"
#include "network.h"
#include "transmit.h"
#include "client.h"

static multiplayergame_t games[MAX_GAMES];
static int numgames;
static user_t users[MAX_CLIENTS];
static int sockfd;
static int our_id;

static char* types[] = {"Team "};
static int maxplayers_pertype[] = {4};

int lobby() {
    char* buffer;
    int option, command;
    int flags = fcntl(sockfd, F_GETFL, 0);
    int j;

    for (j = 0; j < MAX_CLIENTS; j++)
        users[j].id = j;
    for (j = 0; j < MAX_GAMES; j++)
        games[j].id = j;

    while (1) {
        if (transmit(sockfd, CMD_LOBBY_INFO, NULL) < 0)
            return 0;
        if (receive(sockfd, &command, &buffer) < 0)
            return 0;
        switch (command) {
            case CMD_LOBBY_INFO: {
                int ii;
                unserialize_lobby_info(buffer, &users[0], &games[0]);
                numgames = 0;
                for (ii = 0; ii < MAX_GAMES; ii++) {
                    if (games[ii].status != GAME_FREE)
                        numgames++;
                }
                break;
            }
            case CMD_QUIT:
            case CMD_ERROR:
                return 0;
        }
        free(buffer);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        option = game(); //game() calls the actual multiplayer lobby logic.
        fcntl(sockfd, F_SETFL, flags);
        switch (option) {
            case MENU_QUIT:
                xt_par0(XT_CH_NORMAL);
                return 0;
        }

        usleep(1000000 / 5);
    }

}


void drawgames(int pos) {
    int selected_game = 0;

    if (games[pos].status != GAME_FREE) {
        selected_game = pos;
    }
    else
        selected_game = 0;

    dispmultiframe();
    xt_par0(XT_CH_NORMAL);

    int c = 0;
    int useroffsettotal = 0;
    int ioffset = 0;

    while (2 * ioffset + useroffsettotal + (games[numgames - c].status == GAME_FREE ? 0 : games[numgames - c].slots_filled) < ROWS - 20){
        useroffsettotal += games[numgames - c].status == GAME_FREE ? 0 : games[numgames - c].slots_filled;
        c++;
    }

    int i = selected_game - c;
    int ii;

    useroffsettotal = 1;
    int useroffset = 1;
    if (i < 0) i = 0;


    for (; i < MAX_GAMES; i++) {

        if (games[i].status == GAME_FREE)
            continue;

        printgame(5 + 2 * ioffset + useroffsettotal, i == selected_game, games[i]);
        for (ii = 0; ii < games[i].slots_filled; ii++) {
            if (games[i].players[ii] == EMPTY_SLOT)
                continue;
            SETPOS(6 + 2 * ioffset + useroffsettotal, 8);
            printf("%s", users[games[i].players[ii]].username);
            useroffset++;
            useroffsettotal++;
        }
        useroffset = 1;
        i++;
        ioffset++;

    }


    SETPOS(2, 3);
    printf("Open Lobbies (Up/Down to select, %sJ%soin, %sC%sreate, %sQ%suit, %sH%selp)", XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL);

    xt_par0(XT_CH_BOLD);
    SETPOS(3, 3);
    printf("terminvaders MuLtIpLaYeR Lobby");

    xt_par0(XT_CH_DEFAULT);

}



void printgame(int row, int is_selected, multiplayergame_t game) {
    SETPOS(row, 4);
    if (is_selected)
        printf("> %s (%d/%d)", game.name, game.slots_filled, game.slots_total);
    else
        printf("  %s (%d/%d)", game.name, game.slots_filled, game.slots_total);
    SETPOS(row, COLS - 30);
    switch(game.mode){
        case MODE_TEAM:
            printf("Team");
            break;
    }
}


void draw_users() {
    int i;
    int userdisplayoffset = 0;
    printf("Who's Online");
    for (i = 0; i < 16; i++) {
        if (users[i].status != CLIENT_FREE) {
            SETPOS(4 + userdisplayoffset, COLS - 18);
            printf("%s", users[i].username);
            userdisplayoffset++;
        }
    }
}


int game() {
    static int selected_game = 0;

    drawgames(selected_game);
    xt_par0(XT_CH_NORMAL);
    SETPOS(2, COLS - 18);
    draw_users();
    fflush(stdout);
    int key;
    if ((key = getkey()) == KEY_NOTHING)
        return 0;

    xt_par0(XT_CH_NORMAL);
    SETPOS(2, COLS - 18);
    // printf("Who's Online");
    // for (i = 0; i < numusers - onlinelistoffset; i++) {
    //     SETPOS(4 + i, COLS - 18);
    //     if (5 + i > ROWS)
    //         break;
    //     printf("%s", users[i + onlinelistoffset].username);

    // }
    switch(key) {
        // case '<':
        //     xt_par0(XT_CLEAR_SCREEN);
        //     drawgames(selected_game);
        //     dispmultiframe();
        //     if (onlinelistoffset > 0)
        //         onlinelistoffset--;
        //     xt_par0(XT_CH_NORMAL);
        //     SETPOS(2, COLS - 18);
        //     printf("Who's Online");
        //     for (i = 0; i < numusers - onlinelistoffset; i++) {
        //         SETPOS(4 + i, COLS - 18);
        //         if (5 + i > ROWS)
        //             break;
        //         if (users[i + onlinelistoffset].status != CLIENT_FREE)
        //             printf("%s", users[i + onlinelistoffset].username);
        //     }
        //     break;
        // case '>':
        //     xt_par0(XT_CLEAR_SCREEN);
        //     drawgames(selected_game);
        //     dispmultiframe();
        //     if (ROWS - 5 + onlinelistoffset < 64)
        //         onlinelistoffset++;
        //     xt_par0(XT_CH_NORMAL);
        //     SETPOS(2, COLS - 18);
        //     printf("Who's Online");
        //     for (i = 0; i < numusers - onlinelistoffset; i++) {
        //         SETPOS(4 + i, COLS - 18);
        //         if (5 + i > ROWS)
        //             break;
        //         if (users[i + onlinelistoffset].status != CLIENT_FREE)
        //             printf("%s", users[i + onlinelistoffset].username);
        //     }
        //     break;
        case 'q':
        case 'Q':
            xt_par0(XT_CLEAR_SCREEN);
            SETPOS(0,0);
            return MENU_QUIT;
        case 'w':
        case KEY_UP: {
            int i;
            for (i = selected_game - 1; i > 0; i--) {
                if (games[i].status != GAME_FREE) {
                    selected_game = i;
                    drawgames(selected_game);
                    xt_par0(XT_CH_NORMAL);
                    SETPOS(2, COLS - 18);
                    draw_users();
                    fflush(stdout);
                }
            }
            break;
        }
        case 's':
        case KEY_DOWN: {
            int i;
            for (i = selected_game + 1; i < MAX_GAMES; i++) {
                if (games[i].status != GAME_FREE) {
                    selected_game = i;
                    drawgames(selected_game);
                    xt_par0(XT_CH_NORMAL);
                    SETPOS(2, COLS - 18);
                    draw_users();
                    fflush(stdout);
                }
            }
            break;
        }
        case 'j':
        case 'J':
            if (join_popup(&games[selected_game]))
                game_wait(selected_game);
            drawgames(selected_game);
            xt_par0(XT_CH_NORMAL);
            SETPOS(2, COLS - 18);
            draw_users();
            break;
        case 'c':
        case 'C':
            if (create_popup()) // create_popup also includes the error message handling
                game_wait(numgames - 1);
            drawgames(selected_game);
            xt_par0(XT_CH_NORMAL);
            SETPOS(2, COLS - 18);
            draw_users();
            break;
        case 'h':
        case 'H':
            help_popup();
            break;
    }
    return 0;
}

int join_popup (multiplayergame_t* game) {
    int key = 0;
    //int row = 0, col = 0;

    char tmpbuf[8];
    snprintf(tmpbuf, 8, "%d\n", game->id);
    transmit(sockfd, CMD_JOIN_GAME, tmpbuf);

    xt_par0(XT_CH_NORMAL);
    SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
    printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 , COLS / 2 - 30);
    printf("%s  %s  Joining: %-41s    %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, game->name, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
    printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);
    fflush(stdout);
    usleep(1000000);
    int rejected = 0;
    while(1){

        int command;
        char* buffer;
        if (receive(sockfd, &command, &buffer) < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN)
                return 0;
        }
        else {
            free(buffer);
            switch (command) {
                case CMD_ACCEPT_REQ:
                    rejected = -1;
                    break;
                case CMD_REJECT_REQ:
                    rejected = 1;
                    break;
                case CMD_QUIT:
                case CMD_ERROR:
                    return 0;
            }
        }

        if (rejected == 1) {
            xt_par0(XT_CH_NORMAL);
            SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
            printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 , COLS / 2 - 30);
            printf("%s  %s Rejected by %-42s %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, game->name, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
            printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);
            fflush(stdout);
            usleep(1000000);
            return 0;
        } else if (rejected == -1) { // -1 symbolizes that they were ACCEPTED.
            SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
            printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 , COLS / 2 - 30);
            printf("%s  %s Accepted by %-42s %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, game->name, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
            printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);
            fflush(stdout);
            usleep(1000000 / 5);
            return 1;
        }

        key = getkey();
        switch(key){
            case 'q':
            case 'Q':
            case 'b':
            case 'B':
                transmit(sockfd, CMD_CANCEL_REQ, NULL);
                return 0;
        }
        fflush(stdout);
        usleep(1000000/2);
    }
}

int create_popup () {
    int key;
    // here should check if max games has been reached.
    if (numgames >= MAX_GAMES) {
        xt_par0(XT_CH_NORMAL);
        SETPOS(ROWS / 2 - 2, COLS / 2 - 15);
        printf("%s         Create Game          %s", XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 - 1, COLS / 2 - 15);
        printf("%s  %s                          %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 , COLS / 2 - 15);
        printf("%s  %s      Lobby is full!      %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 1, COLS / 2 - 15);
        printf("%s  %s                          %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 2, COLS / 2 - 15);
        printf("%s   Press any key to go back   %s", XT_CH_INVERSE, XT_CH_NORMAL);
        fflush(stdout);

        int key;
        while ((key = getkey()) == KEY_NOTHING);
        xt_par0(XT_CLEAR_SCREEN);

        return 0;
    }

    int type = 0, users = 2, field = 0, pos = 0, titlelen = 0, i;

    char title[NAME_LEN + 1] = "";

    while(1){
        xt_par0(XT_CH_NORMAL);
        SETPOS(ROWS / 2 - 2, COLS / 2 - 35);
        printf("%s                             Create Game                              %s", XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 - 1, COLS / 2 - 35);
        printf("%s  %s                                                                  %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 , COLS / 2 - 35);
        printf("%s  %s  Title: %s%-32s%s Users: %s%d%s Type: %s%s   %s %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, title, XT_CH_NORMAL,XT_CH_UNDERLINE, users,XT_CH_NORMAL, XT_CH_UNDERLINE, types[type],XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 1, COLS / 2 - 35);
        printf("%s  %s                                                                  %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 2, COLS / 2 - 35);
        printf("%s       Esc to cancel, Tab to switch field, Up/Down/Type to input      %s", XT_CH_INVERSE, XT_CH_NORMAL);

        if (titlelen < 32)
            xt_par0(XT_CH_UNDERLINE);
        SETPOS(ROWS / 2, COLS / 2 - 24 + titlelen);
        putchar(' ');
        xt_par0(XT_CH_NORMAL);

        switch (field) {
            case 0:
                SETPOS(ROWS / 2 , COLS / 2 - 24 + pos);
                break;
            case 1:
                SETPOS(ROWS / 2, COLS / 2 + 16);
                break;
            case 2:
                SETPOS(ROWS / 2, COLS / 2 + 24);
                break;
        }
        fflush(stdout);

        while((key = getkey()) == KEY_NOTHING);
        switch(key){
            case KEY_ENTER: {
                char* bufff;
                serialize_game_setup(type, users, title, &bufff);
                transmit(sockfd, CMD_SETUP_GAME, bufff);
                free(bufff);
                int nnn = numgames;
                games[nnn] = (multiplayergame_t) {nnn, GAME_WAITING, users, 0, {0}, {0}, type};
                strcpy(games[nnn].name, title);
                numgames++;
                xt_par0(XT_CLEAR_SCREEN);
                return 1;
            }
            case 9: //TAB
                if (field < 2)
                    field++;
                else
                    field = 0;
                break;
            case 27: //ESC
                xt_par0(XT_CLEAR_SCREEN);
                return 0;
            case KEY_UP:
                if (field == 1){
                    if (users < maxplayers_pertype[type])
                        users++;
                } else if (field == 2) {
                    if (type < 2) {
                        type++;
                        if (users > maxplayers_pertype[type])
                            users = maxplayers_pertype[type];
                    }
                }
                break;
            case KEY_DOWN:
                if (field == 1){
                    if (users > 2)
                        users--;
                } else if (field == 2) {
                    if (type > 0) {
                        type--;
                        if (users > maxplayers_pertype[type])
                            users = maxplayers_pertype[type];
                    }
                }
                break;
            case KEY_LEFT:
                if (pos > 0){
                    pos--;
                }
                break;
            case KEY_RIGHT:
                if (pos < titlelen){
                    pos++;
                }
                break;
            case KEY_BACKSPACE:
                if (field == 0) {
                    if (pos > 0) {
                        title[--pos] = ' ';
                        title[titlelen] = '\0';
                        titlelen--;
                    }
                    for (i = pos + 1; i <= titlelen; i++) {
                        title[i-1] = title[i];
                    }
                }
            default:
                if ((key >= ' ' && key <= '~') && field == 0 && titlelen < 32) {
                    if (pos != titlelen) {
                        for (i = titlelen; i > pos; i--) {
                            title[i] = title[i-1];
                        }
                    }
                    title[pos] = key;
                    pos++;
                    putchar(key);
                    titlelen++;
                }
                break;
        }
        fflush(stdout);
    }
}



void help_popup () {

}

void game_wait(int created_game) {
    int key = 0;

    dispmultiframe();

    // this prints out current users in this game
    int i = created_game; // (which also needs to be integrated with server)
    // int onlinelistoffset = 0    ;

    int p_id;
    int foxes;
    int command;
    char* buffer;

    while (1) {
        int useroffset = 1;

        if (receive(sockfd, &command, &buffer) < 0) {
            if (errno != EWOULDBLOCK && errno != EAGAIN)
                return;
        }
        else {
            switch (command) {
                case CMD_NEW_REQUEST:
                    if (confirm_request(buffer))
                        transmit(sockfd, CMD_ACCEPT_REQ, buffer);
                    else
                        transmit(sockfd, CMD_REJECT_REQ, buffer);
                    dispmultiframe();
                    break;
                case CMD_CANCEL_REQ:
                    // TODO: implement
                    break;
                case CMD_PLAYER_JOIN:
                    sscanf(buffer, "%d", &p_id);
                    for (foxes = 0; foxes < games[i].slots_total; foxes++) {
                        if (games[i].players[foxes] == EMPTY_SLOT) {
                            games[i].players[foxes] = p_id;
                            break;
                        }
                    }
                    break;
                case CMD_PLAYER_PART:
                    sscanf(buffer, "%d", &p_id);
                    for (foxes = 0; foxes < games[i].slots_total; foxes++) {
                        if (games[i].players[foxes] == p_id) {
                            games[i].players[foxes] = EMPTY_SLOT;
                            break;
                        }
                    }
                    break;
                case CMD_GAME_START: {
                    game_t game;
                    setup_game(&game);
                    setup_multiplayer(&game, games[i].slots_total, atoi(buffer), sockfd);
                    while (game.running) {
                        update_game(&game);
                        usleep(1000000 / FPS);
                    }
                    return;
                }
                case CMD_QUIT:
                case CMD_ERROR:
                    return;
            }
            free(buffer);
        }

        switch ((key = getkey())) {
            case 'q':
            case 'Q':
            case 27: // esc
                transmit(sockfd, CMD_PLAYER_PART, NULL);
                xt_par0(XT_CLEAR_SCREEN);
                return;
        }

        for (foxes = 0; foxes < games[i].slots_total; foxes++) {
            SETPOS(6 + 2 * i + useroffset, 8);
            if (games[i].players[foxes] != EMPTY_SLOT) {
                printf("%d", games[i].players[foxes]);
                useroffset++;
            }
        }

        SETPOS(2,3);
        printf("%s", games[created_game].name);
        SETPOS(ROWS, COLS);

        fflush(stdout);
        usleep(1000000 / 20);
    }
}


void serverlogin(){
    int badserver;
    int field = 0, pos = 0, len = 0, addresslen = 0, usernamelen = 0, key = 0;

    char address[33] = "";
    int i;

    char username[17] = "";
    int j;

    while(1){

        i = strlen(address);
        j = strlen(username);

        if (field)
            len = usernamelen;
        else
            len = addresslen;

        xt_par0(XT_CH_NORMAL);
        SETPOS(ROWS / 2 - 3, COLS / 2 - 30);
        printf("%s                       Server Connect                       %s", XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
        printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
        printf("%s  %s  Address: %s                                %s             %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);
        SETPOS(ROWS / 2 - 1, COLS / 2 - 17);
        xt_par0(XT_CH_UNDERLINE);
        printf("%s", address);
        xt_par0(XT_CH_NORMAL);

        SETPOS(ROWS / 2, COLS / 2 - 30);
        printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
        printf("%s  %s  Username: %s                %s                            %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);
        SETPOS(ROWS / 2 + 1, COLS / 2 - 16);
        xt_par0(XT_CH_UNDERLINE);
        printf("%s", username);
        xt_par0(XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
        printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 3, COLS / 2 - 30);
        printf("%s Esc to go back, Enter to connect, Up/Down to switch fields %s", XT_CH_INVERSE, XT_CH_NORMAL);

        fflush(stdout);

        if (addresslen < 32) {
            xt_par0(XT_CH_NORMAL);
            if (pos < 32)
                xt_par0(XT_CH_UNDERLINE);
            SETPOS(ROWS / 2 - 1, COLS / 2 - 17 + addresslen);
            putchar(' ');
        }

        if (usernamelen < 16) {
            xt_par0(XT_CH_NORMAL);
            if (pos < 16)
                xt_par0(XT_CH_UNDERLINE);
            SETPOS(ROWS / 2 + 1, COLS / 2 - 16 + usernamelen);
            putchar(' ');
        }
        xt_par0(XT_CH_NORMAL);

        switch (field) {
            case 0:
                SETPOS(ROWS / 2 - 1, COLS / 2 - 17 + pos);
                break;
            case 1:
                SETPOS(ROWS / 2 + 1, COLS / 2 - 16 + pos);
                break;
        }

        while((key = getkey()) == KEY_NOTHING);


        switch(key){
            case KEY_ENTER: {
                if (i == 0 || j < 2)
                    break;
                badserver = 0;
                sockfd = make_connection(address);
                if (sockfd < 0)
                    badserver = 1;
                else if (transmit(sockfd, CMD_CONNECT, username) < 0)
                    badserver = 1;
                else {
                    int command;
                    char* buffer;
                    if (receive(sockfd, &command, &buffer) < 0)
                        badserver = 1;
                    else {
                        if (command == CMD_CONNECT)
                            our_id = atoi(buffer);
                        else
                            badserver = 1;
                        free(buffer);
                    }
                }

                if (badserver) {
                    xt_par0(XT_CH_NORMAL);
                    SETPOS(ROWS / 2 - 3, COLS / 2 - 30);
                    printf("%s                       Server Connect                       %s", XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
                    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
                    printf("%s  %s  Address: %s%s%s             %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, address, XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2, COLS / 2 - 30);
                    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
                    printf("%s  %s           Server not found on this address.            %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
                    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

                    SETPOS(ROWS / 2 + 3, COLS / 2 - 30);
                    printf("%s                       Esc to go back                       %s", XT_CH_INVERSE, XT_CH_NORMAL);
                    fflush(stdout);
                    break;
                }

                xt_par0(XT_CLEAR_SCREEN);
                lobby();
                close(sockfd);
                return;
            }
            case 27: //ESC
                xt_par0(XT_CLEAR_SCREEN);
                //return 0;
                return;
            case KEY_UP:
            case KEY_DOWN:
                field = !field;
                if (field)
                    len = usernamelen;
                else
                    len = addresslen;
                pos = 0;
                break;
            case KEY_LEFT:
                if (pos > 0){
                    pos--;
                }
                break;
            case KEY_RIGHT:
                if (pos < len){
                    pos++;
                }
                break;
            case KEY_BACKSPACE:
                if (field && pos > 0) {
                    username[--pos] = ' ';
                    username[usernamelen--] = '\0';
                    for (i = pos + 1; i <= usernamelen; i++) {
                        username[i-1] = username[i];
                    }
                } else if (!field && pos > 0) {
                    address[--pos] = ' ';
                    address[addresslen--] = '\0';
                    for (i = pos + 1; i <= addresslen; i++) {
                        address[i-1] = address[i];
                    }
                }
                break;
            default:
                if (key >= ' ' && key <= '~') {
                    if (field && usernamelen < 16) {
                        if (pos != usernamelen) {
                            for (i = usernamelen; i > pos; i--) {
                                username[i] = username[i-1];
                            }
                        }
                        username[pos] = key;
                        pos++;
                        putchar(key);
                        usernamelen++;
                    } else if (!field && addresslen < 32) {
                        if (pos != addresslen) {
                            for (i = addresslen; i > pos; i--) {
                                address[i] = address[i-1];
                            }
                        }
                        address[pos] = key;
                        pos++;
                        putchar(key);
                        addresslen++;
                    }
                }
                break;
        }
    }
}

int confirm_request(char* username) {
    int choice = 0;

    int key = 0;

    xt_par0(XT_CH_NORMAL);
    SETPOS(ROWS / 2 - 3, COLS / 2 - 16);
    printf("%s                                %s", XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 2, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 1, COLS / 2 - 16);
    printf("%s  %s Let %16s join? %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, username, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 0, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 1, COLS / 2 - 16);
    printf("%s  %s       No         Yes       %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 2, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 3, COLS / 2 - 16);
    printf("%s                                %s", XT_CH_INVERSE, XT_CH_NORMAL);
    fflush(stdout);

    while (1){
        while ((key = getkey()) == KEY_NOTHING);

        switch (key) {
            case KEY_LEFT:
                choice = 0;
                break;
            case KEY_RIGHT:
                choice = 1;
                break;
            case KEY_ENTER:
                return choice;
        }

        if (choice) {
            SETPOS(ROWS / 2 + 1, COLS / 2 + 4);
            printf(XT_CH_INVERSE);
            printf(XT_CH_BOLD);
            printf("Yes");
            printf(XT_CH_NORMAL);
            SETPOS(ROWS / 2 + 1, COLS / 2 - 7);
            printf("No");
            fflush(stdout);
        } else {
            SETPOS(ROWS / 2 + 1, COLS / 2 - 7);
            printf(XT_CH_INVERSE);
            printf(XT_CH_BOLD);
            printf("No");
            printf(XT_CH_NORMAL);
            SETPOS(ROWS / 2 + 1, COLS / 2 + 4);
            printf("Yes");
            fflush(stdout);
        }
    }



}



void dispmultiframe(){
    xt_par0(XT_CLEAR_SCREEN);
    SETPOS(10,10);
    xt_par0(XT_CH_NORMAL);
    int cursor_r = 1, cursor_c = 1;
    SETPOS(1, 1);
    int test = 0;
    int lim = 3;
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("~");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("~");
        cursor_c++;

        if (test == lim){
            fflush(stdout);
            test = 0;
        }
        test++;
    }


    cursor_r = 2;
    while (cursor_r < ROWS) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        printf("#");
        cursor_c = COLS - 20;
        SETPOS(cursor_r, cursor_c);
        putchar('|');
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        printf("#");
        cursor_r++;
        if (test == lim){
            fflush(stdout);
            test = 0;
        }
        test++;
    }
}





