#include "lobby.h"
#include "server.h"
#include "network.h"
#include "transmit.h"
#include "client.h"

//ben, this should be coming from your server instead of these variables
static multiplayergame_t* games;
static int numgames;
static user_t* users;
static int numusers;

static char* types[] = {"Team "};
static int maxplayers_pertype[] = {4};

int lobby() {
    numusers = 64;
    users = malloc(sizeof(user_t) * 6);
    users[0] = (user_t) {"Earwig"};
    users[1] = (user_t) {"VjiaoBlack"};
    users[2] = (user_t) {"jeuwshuawakeup"};
    users[3] = (user_t) {"Infernous"};
    users[4] = (user_t) {"Agnok"};
    users[5] = (user_t) {"Xx.DarkLord.xX"};

    users[6] = (user_t) {"66666666"};
    users[7] = (user_t) {"77777777"};
    users[8] = (user_t) {"88888888"};
    users[9] = (user_t) {"99999999"};
    users[10] = (user_t) {"10.10.10."};
    users[11] = (user_t) {"11.11.11"};

    int i;
    for (i = 12; i < 63; i++) {
        users[i] = (user_t) {"sweg"};
    }
    users[63] = (user_t) {"end"};

    user_t *users1 = malloc(sizeof(user_t) * 4);
    users1[0] = users[0];
    users1[1] = users[1];
    users1[2] = users[3];
    users1[3] = users[2];

    user_t *users2 = malloc(sizeof(user_t) * 2);
    users2[0] = users[4];
    users2[1] = users[5];

    user_t *users3 = malloc(sizeof(user_t) * 1);
    users3[0] = users[5];

    games = malloc(sizeof(multiplayergame_t) * MAX_GAMES);
    games[0] = (multiplayergame_t) {1,"join this room if you have swag", 10, 4, 2, users1};//lobby_t current_lobby = (lobby_t) {"join this room if you have swag", 10, 4, 2, users1, NULL, NULL};
    games[1] = (multiplayergame_t) {1,"join this room if you dont have swag :(", 4, 1, 1, users2};//current_lobby.next = &((lobby_t) {"join this room if you dont have swag :(", 4, 1, 1, users2, NULL, &current_lobby});
    games[2] = (multiplayergame_t) {1,"pls dont join this room ty", 2, 1, 0, users3};
    for (i = 3; i < 10; i++) {
        games[i] = (multiplayergame_t){1,"testtesttest", i, 4, 0, users1};
    }
    for (i = 10; i < MAX_GAMES; i++) {
        games[i] = (multiplayergame_t) {0};
    }
    numgames = 9;

    int option;
    while (1){
        switch(option = game()) {
            case MENU_QUIT:
                xt_par0(XT_CH_NORMAL);
                return 0;
        }
    }

}











void drawgame(int pos) {
    int selected_game = 0;

    //data stuff that shud b replaced by the working server

    user_t *users1 = malloc(sizeof(user_t) * 4);

    users1[0] = users[0];
    users1[1] = users[1];
    users1[2] = users[3];
    users1[3] = users[2];

    user_t *users2 = malloc(sizeof(user_t) * 2);
    users2[0] = users[4];
    users2[1] = users[5];

    user_t *users3 = malloc(sizeof(user_t) * 1);
    users3[0] = users[5];

    //--------------------------------------------------------
    if (pos > -1) {
        selected_game = pos;
    }

    dispmultiframe();
    xt_par0(XT_CH_NORMAL);
    // this should be scrollable too. Prob with <, > or 1, 0, or something.

    //this prints stuff before the current-----------


    int c = 0;
    int useroffsettotal = 0;
    int ioffset = 0;

    while (2 * ioffset + useroffsettotal + games[numgames - c].current_users < ROWS - 20){
        useroffsettotal += games[numgames - c].current_users;
        c++;
    }

    int i = selected_game - c;

    useroffsettotal = 1;
    int useroffset = 1;
    if (i < 0) i = 0;
    while (i < selected_game) {
        printgame(5 + 2 * ioffset + useroffsettotal, 0, games[i]);
        while (useroffset <= games[i].current_users){
            SETPOS(6 + 2 * ioffset + useroffsettotal, 8);
            printf("%s", games[i].users[useroffset-1].username);
            useroffset++;
            useroffsettotal++;
            fflush(stdout);
        }
        useroffset = 1;
        i++;
        ioffset++;
        fflush(stdout);
    }

    //prints current ---------------
    i = selected_game; // should be redundant. this is a safety catch.
    printgame(5 + 2 * ioffset + useroffsettotal, 1, games[i]);
    while (useroffset <= games[i].current_users){
        SETPOS(6 + 2 * ioffset + useroffsettotal, 8);
        printf("%s", games[i].users[useroffset-1].username);
        useroffset++;
        useroffsettotal++;
        fflush(stdout);
    }
    useroffset = 1;
    i++;
    ioffset++;

    //this prints stuff after the current
    while (i < 64 && games[i].valid != 0 && 2 * ioffset + useroffsettotal + games[i].current_users < ROWS - 6) {
        printgame(5 + 2 * ioffset + useroffsettotal, 0, games[i]);
        while (useroffset <= games[i].current_users){
            SETPOS(6 + 2 * ioffset + useroffsettotal, 8);
            printf("%s", games[i].users[useroffset-1].username);
            useroffset++;
            useroffsettotal++;
            fflush(stdout);
        }
        useroffset = 1;
        i++;
        ioffset++;
        fflush(stdout);
    }

    SETPOS(2, 3);
    printf("Open Lobbies (Up/Down to select, %sJ%soin, %sC%sreate, %sQ%suit, %sH%selp)", XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL);

    xt_par0(XT_CH_BOLD);
    SETPOS(3, 3);
    printf("terminvaders MuLtIpLaYeR Lobby");

    xt_par0(XT_CH_DEFAULT);


    fflush(stdout);

}



void printgame(int row, int is_selected, multiplayergame_t game) {
    SETPOS(row, 4);
    if (is_selected)
        printf("> %s (%d/%d)", game.name, game.current_users, game.max_users);
    else
        printf("  %s (%d/%d)", game.name, game.current_users, game.max_users);
    SETPOS(row, COLS - 30);
    switch(game.type){
        case MODE_TEAM:
            printf("Team");
            break;
    }
}







int game() {

    users[6] = (user_t) {"66666666"};
    users[7] = (user_t) {"77777777"};
    users[8] = (user_t) {"88888888"};
    users[9] = (user_t) {"99999999"};
    users[10] = (user_t) {"10.10.10"};
    users[11] = (user_t) {"11.11.11"};


    int selected_game = 0;
    int onlinelistoffset = 0;

    xt_par0(XT_CLEAR_SCREEN);
    xt_par0(XT_CH_NORMAL);
    SETPOS(2, COLS - 18);
    printf("Who's Online");
    int i;
    for (i = 0; i < numusers - onlinelistoffset; i++) {
        SETPOS(4 + i, COLS - 18);
        if (5 + i > ROWS)
            break;
        printf("%s", users[i + onlinelistoffset].username);
        fflush(stdout);
    }
    drawgame(0);
    int key;
    while(1) {
        while((key = getkey()) == KEY_NOTHING);
        xt_par0(XT_CH_NORMAL);
        SETPOS(2, COLS - 18);
        printf("Who's Online");
        for (i = 0; i < numusers - onlinelistoffset; i++) {
            SETPOS(4 + i, COLS - 18);
            if (5 + i > ROWS)
                break;
            printf("%s", users[i + onlinelistoffset].username);
            fflush(stdout);
        }
        switch(key) {
            case '<':
                xt_par0(XT_CLEAR_SCREEN);
                drawgame(selected_game);
                dispmultiframe();
                if (onlinelistoffset > 0)
                    onlinelistoffset--;
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case '>':
                xt_par0(XT_CLEAR_SCREEN);
                drawgame(selected_game);
                dispmultiframe();
                if (ROWS - 5 + onlinelistoffset < 64)
                    onlinelistoffset++;
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 'q':
            case 'Q':
                xt_par0(XT_CLEAR_SCREEN);
                SETPOS(0,0);
                return MENU_QUIT;
                break;
            case 'w':
            case KEY_UP:
                xt_par0(XT_CLEAR_SCREEN);
                if (selected_game > 0) {
                    drawgame(--selected_game);
                } else {
                    drawgame(selected_game);
                }
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 's':
            case KEY_DOWN:
                xt_par0(XT_CLEAR_SCREEN);
                if (selected_game < 64 && games[selected_game + 1].valid != 0) {
                    drawgame(++selected_game);
                } else {
                    drawgame(selected_game);
                }
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 'j':
            case 'J':
                if (join_popup(games[selected_game]))
                    game_wait(selected_game);
                drawgame(selected_game);
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 'c':
            case 'C':
                if (create_popup()) // create_popup also includes the error message handling
                    game_wait(numgames - 1);
                drawgame(selected_game);
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 'h':
            case 'H':
                help_popup();
                break;
        }
    }
}

int join_popup (multiplayergame_t game) {
    int key = 0;
    //int row = 0, col = 0;

    char* title = malloc(sizeof(char) * 33);
    strcpy(title, game.name);
    int i = strlen(title);

    while (i < 41) {
        strcat(title, " ");
        i++;
    }

    xt_par0(XT_CH_NORMAL);
    SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
    printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 , COLS / 2 - 30);
    printf("%s  %s  Joining: %s    %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, title, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
    printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
    printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);

    int rejected = 1;
    while(1){
        while((key = getkey()) == KEY_NOTHING);
        if ((rejected = 1)) {// rejected should be revalued from server here
            xt_par0(XT_CH_NORMAL);
            SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
            printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 , COLS / 2 - 30);
            printf("%s  %s Rejected by %s. %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, title, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
            printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);
            return 0;
        } else if ((rejected = -1)) { // -1 symbolizes that they were ACCEPTED.
            SETPOS(ROWS / 2 - 2, COLS / 2 - 30);
            printf("%s                         Join Game                          %s", XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 - 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 , COLS / 2 - 30);
            printf("%s  %s Accepted by %s. %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, title, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
            printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

            SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
            printf("%s                            %sB%s%sack                            %s", XT_CH_INVERSE, XT_CH_UNDERLINE, XT_CH_NORMAL,XT_CH_INVERSE, XT_CH_NORMAL);
            usleep(1000000/2);
            return 1;
        }

        switch(key){
            case 'q':
            case 'b':
            case 'B':
                xt_par0(XT_CLEAR_SCREEN);
                return 0;
        }
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

        int key;
        while ((key = getkey()) == KEY_NOTHING);
        xt_par0(XT_CLEAR_SCREEN);

        return 0;
    }

    int type = 0, users = 2, field = 0, pos = 0, titlelen = 0;

    char* title = malloc(sizeof(char) * 33);
    int i = strlen(title);

    while (i < 32) {
        strcat(title, " ");
        i++;
    }

    while(1){

        i = strlen(title);

        while (i < 32) {
            strcat(title, " ");
            i++;
        }

        xt_par0(XT_CH_NORMAL);
        SETPOS(ROWS / 2 - 2, COLS / 2 - 35);
        printf("%s                             Create Game                              %s", XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 - 1, COLS / 2 - 35);
        printf("%s  %s                                                                  %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 , COLS / 2 - 35);
        printf("%s  %s  Title: %s%s%s Users: %s%d%s Type: %s%s   %s %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, title, XT_CH_NORMAL,XT_CH_UNDERLINE, users,XT_CH_NORMAL, XT_CH_UNDERLINE, types[type],XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

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

        while((key = getkey()) == KEY_NOTHING);
        switch(key){
            case KEY_ENTER:
                // ben this shud be ur server magic, ithink
                games[numgames++] = (multiplayergame_t) {1, title, users, 0, type, NULL};
                // NOTE -> the NULL should be replaced with the user that calls this... i'm not sure how to best do it
                xt_par0(XT_CLEAR_SCREEN);
                free(title);
                return 1;
            case 9: //TAB
                if (field < 2)
                    field++;
                else
                    field = 0;
                break;
            case 27: //ESC
                xt_par0(XT_CLEAR_SCREEN);
                free(title);
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
                if (((key <= 'z' && key >= 'A') || (key >= '0' && key <= '9')) && field == 0 && titlelen < 32) {
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
    }
}



void help_popup () {

}

void game_wait(int created_game) {
    int key = 0;

    dispmultiframe();

    // this prints out current users in this game
    int i = created_game; // (which also needs to be integrated with server)
    int onlinelistoffset = 0    ;
    int useroffset = 1;
    while (useroffset <= games[i].current_users){
        SETPOS(6 + 2 * i + useroffset, 8);
        printf("%s", games[i].users[useroffset-1].username);
        useroffset++;
        fflush(stdout);
    }

    // this prints out who's online
    xt_par0(XT_CH_NORMAL);
    SETPOS(2, COLS - 18);
    printf("Who's Online");
    for (i = 0; i < numusers - onlinelistoffset; i++) {
        SETPOS(4 + i, COLS - 18);
        if (5 + i > ROWS)
            break;
        printf("%s", users[i + onlinelistoffset].username);
        fflush(stdout);
    }

    SETPOS(2,3);
    printf("%s", games[created_game].name);
    SETPOS(ROWS, COLS);


    while (1) {
        //------------------------------------------
        //user_t requestinguser;
        /* Server code goes here.
        There is a request for GET_USER_REQUEST or something, much like getkey.
        while ((requestinguser = GET_USER_REQUEST()) != NULL) {
            games[created_game].users[games[created_game].currentusers++] = requestinguser;
        } */

        // please note im not sure if i need a cpyuser funciton. I dont thinks o. tell me if i dio.
        //- ----------------------------------------
        if (games[created_game].current_users == games[created_game].max_users){
            //run some play game funciton;
            return;
        }

        switch ((key = getkey())) {
            case '<':
                xt_par0(XT_CLEAR_SCREEN);
                dispmultiframe();
                SETPOS(2,3);
                printf("%s", games[created_game].name);
                SETPOS(ROWS, COLS);
                if (onlinelistoffset > 0)
                    onlinelistoffset--;
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case '>':
                xt_par0(XT_CLEAR_SCREEN);
                dispmultiframe();
                SETPOS(2,3);
                printf("%s", games[created_game].name);
                SETPOS(ROWS, COLS);
                if (ROWS - 4 + onlinelistoffset < 64)
                    onlinelistoffset++;
                xt_par0(XT_CH_NORMAL);
                SETPOS(2, COLS - 18);
                printf("Who's Online");
                for (i = 0; i < numusers - onlinelistoffset; i++) {
                    SETPOS(4 + i, COLS - 18);
                    if (5 + i > ROWS)
                        break;
                    printf("%s", users[i + onlinelistoffset].username);
                }
                break;
            case 'q':
            case 'Q':
            case 27: // esc
                xt_par0(XT_CLEAR_SCREEN);
                return;
        }

    }
}


void serverlogin(){

    int field = 0, pos = 0, len = 0, addresslen = 0, usernamelen = 0, key = 0;

    char address[33];
    int i = strlen(address);

    char username[17];
    int j = strlen(username);

    while (i < 32) {
        strcat(address, " ");
        i++;
    }

    while (j < 16) {
        strcat(username, " ");
        j++;
    }

    while(1){

        i = strlen(address);
        j = strlen(username);

        while (i < 32) {
            strcat(address, " ");
            i++;
        }

        while (j < 16) {
            strcat(username, " ");
            j++;
        }

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
        printf("%s  %s  Address: %s%s%s             %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, address, XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2, COLS / 2 - 30);
        printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 1, COLS / 2 - 30);
        printf("%s  %s  Username: %s%s%s                            %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_UNDERLINE, username, XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 2, COLS / 2 - 30);
        printf("%s  %s                                                        %s  %s", XT_CH_INVERSE,XT_CH_NORMAL, XT_CH_INVERSE,XT_CH_NORMAL);

        SETPOS(ROWS / 2 + 3, COLS / 2 - 30);
        printf("%s Esc to go back, Enter to connect, Up/Down to switch fields %s", XT_CH_INVERSE, XT_CH_NORMAL);

        fflush(stdout);

        if ((field && addresslen < 32) || (!field && usernamelen < 16))
            xt_par0(XT_CH_UNDERLINE);

        SETPOS(ROWS / 2 + 1, COLS / 2 - 16 + usernamelen);
        putchar(' ');
        SETPOS(ROWS / 2 - 1, COLS / 2 - 17 + addresslen);

        putchar(' ');
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
                // ben this shud be ur server magic,  you gotta connect here
                // NOTE -> the NULL should be replaced with the user that calls this... i'm not sure how to best do it



                // int sockfd = make_connection("earwig.yulli.org"), command;
                // char* databuf;
                // transmit(sockfd, CMD_CONNECT, "Earwig");
                // transmit(sockfd, CMD_SETUP_GAME, "0|2|Test");
                // while((key = getkey()) == KEY_NOTHING);
                // receive(sockfd, &command, &databuf); // PLAYER_JOIN
                // // free(databuf);
                // receive(sockfd, &command, &databuf); // NEW_REQUEST
                // // free(databuf);
                // transmit(sockfd, CMD_ACCEPT_REQ, "1");
                // receive(sockfd, &command, &databuf); // PLAYER_JOIN
                // // free(databuf);
                // receive(sockfd, &command, &databuf); // GAME_START
                // // free(databuf);
                // int flags = fcntl(sockfd, F_GETFL, 0);
                // fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
                // game_t game;
                // setup_game(&game);
                // setup_multiplayer(&game, 2, 0, sockfd);
                // while (game.running) {
                //     update_game(&game);
                //     usleep(1000000 / FPS);
                // }




                int sockfd = make_connection("earwig.yulli.org"), command;
                char* databuf;
                transmit(sockfd, CMD_CONNECT, "Firefly");
                transmit(sockfd, CMD_JOIN_GAME, "0");
                receive(sockfd, &command, &databuf); // PLAYER_JOIN
                // free(databuf);
                receive(sockfd, &command, &databuf); // PLAYER_JOIN
                // free(databuf);
                receive(sockfd, &command, &databuf); // GAME_START
                // free(databuf);
                int flags = fcntl(sockfd, F_GETFL, 0);
                fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
                game_t game;
                setup_game(&game);
                setup_multiplayer(&game, 2, 1, sockfd);
                while (game.running) {
                    update_game(&game);
                    usleep(1000000 / FPS);
                }







                // lobby();
                //return 1;
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
                if (field) {
                    if (pos > 0) {
                        username[--pos] = ' ';
                        username[usernamelen] = '\0';
                        usernamelen--;
                    }
                    for (i = pos + 1; i <= usernamelen; i++) {
                        username[i-1] = username[i];
                    }
                } else {
                    if (pos > 0) {
                        address[--pos] = ' ';
                        address[addresslen] = '\0';
                        addresslen--;
                    }
                    for (i = pos + 1; i <= addresslen; i++) {
                        address[i-1] = address[i];
                    }
                }
            default:
                if (((key <= 'z' && key >= 'A') || (key >= '0' && key <= '9')) && addresslen < 32) {
                    if (field) {
                        if (pos != usernamelen) {
                            for (i = usernamelen; i > pos; i--) {
                                username[i] = username[i-1];
                            }
                        }
                        username[pos] = key;
                        pos++;
                        putchar(key);
                        usernamelen++;
                    } else {
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

int confirm_request(user_t requester) {
    int choice;

    int key = 0;

    char* username = malloc(sizeof(char) * 17);
    strcpy(username, requester.username);
    int j = strlen(username);

    while (j < 16) {
        strcat(username, " ");
        j++;
    }


    xt_par0(XT_CH_NORMAL);
    SETPOS(ROWS / 2 - 3, COLS / 2 - 16);
    printf("%s                                %s", XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 2, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 - 1, COLS / 2 - 16);
    printf("%s  %s Let %s join? %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, username, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 0, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 1, COLS / 2 - 16);
    printf("%s  %s       No         Yes       %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 2, COLS / 2 - 16);
    printf("%s  %s                            %s  %s", XT_CH_INVERSE, XT_CH_NORMAL, XT_CH_INVERSE, XT_CH_NORMAL);

    SETPOS(ROWS / 2 + 3, COLS / 2 - 16);
    printf("%s                                %s", XT_CH_INVERSE, XT_CH_NORMAL);


    while (1){
        while ((key == getkey()) == KEY_NOTHING);

        switch (key) {
            if (choice) {
                SETPOS(ROWS / 2 + 1, COLS / 2 + 5);
                printf(XT_CH_INVERSE);
                printf(XT_CH_BOLD);
                printf("YES");
                printf(XT_CH_NORMAL);
            } else {
                SETPOS(ROWS / 2 + 1, COLS / 2 - 5);
                printf(XT_CH_INVERSE);
                printf(XT_CH_BOLD);
                printf("NO");
                printf(XT_CH_NORMAL);
            }
            case KEY_LEFT:
            case KEY_RIGHT:
                choice = !choice;
                break;
            case KEY_ENTER:
                return choice;
        }
    }



}



void dispmultiframe(){
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



