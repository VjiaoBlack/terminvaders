
#include "xterm/keyboard.c"
#include "lobby.h"

static int rows = 44;
static int cols = 180;

int get_rows(void) {
    return rows;
}
int get_cols(void) {
    return cols;
}

int main() {
    int option;
    FILE *fp;
    if (!(fp = fopen("preferences.txt", "r"))) {
        fp = fopen("preferences.txt", "w");
        fputs("44:180", fp);
        fclose(fp);
        fp = fopen("preferences.txt", "r");
    }
    fscanf(fp, "%d:%d", &rows, &cols);
    fclose(fp);
    while (1){
        switch(option = lobby()) {
            drawlobby(option);
            case MENU_QUIT:
                xt_par0(XT_CH_NORMAL);
                getkey_terminate();
                return 0;
        }
    }

}

void drawlobby(int choice) {
    int numusers = 6;
    user_t *users = malloc(sizeof(user_t) * 6);
    *users = (user_t) {"Earwig"};
    *(++users) = (user_t) {"VjiaoBlack"};
    *(++users) = (user_t) {"jeuwshuawakeup"};
    *(++users) = (user_t) {"Infernous"};
    *(++users) = (user_t) {"Agnok"};
    *(++users) = (user_t) {"Xx.DarkLord.xX"};
    users -= 5;


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


    lobby_t current_lobby = (lobby_t) {"join this room if you have swag", 10, 4, 2, users1, NULL, NULL};
    
    current_lobby.next = &((lobby_t) {"join this room if you dont have swag :(", 4, 1, 1, users2, NULL, &current_lobby});

    current_lobby.next->next = &((lobby_t) {"pls dont join this room ty", 2, 1, 0, users3, NULL, current_lobby.next});


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

    SETPOS(2, COLS - 18);
    printf("Who's Online");
    
    for (int i = 0; i < numusers; i++) {
        SETPOS(4 + i, COLS - 18);
        printf("%s", users[i].username);
    }

    //this prints stuff before the current
    int current_pos = 0;
    lobby_t head;
    lobby_t iterator = current_lobby;
    while (iterator.previous != NULL) {
        iterator = *(iterator.previous);
        current_pos++;
    }
    head = iterator;

    int i = 0;
    while (current_pos > 0) {
        SETPOS(6 + 2 * i, 6);
        printf("%s (%d/%d)", iterator.name, iterator.current_users, iterator.max_users);

        SETPOS(6 + 2 * i, COLS - 30);
        switch(iterator.type){
            case 0:
                printf("Duel");
                break;
            case 1:
                printf("Team");
                break;
            case 2:
                printf("Blitz");
                break;
        }

        iterator = *(iterator.next);
        i++;
    }

    //prints current
    SETPOS(6 + 2 * i, 4);
    printf("> %s (%d/%d)", current_lobby.name, current_lobby.current_users, current_lobby.max_users);

    SETPOS(6 + 2 * i, COLS - 30);
    switch(iterator.type){
        case 0:
            printf("Duel");
            break;
        case 1:
            printf("Team");
            break;
        case 2:
            printf("Blitz");
            break;
    }
    int useroffset = 1;
    while (useroffset <= current_lobby.current_users){
        SETPOS(6 + 2 * i + useroffset, 8);
        printf("%s", current_lobby.users[useroffset-1].username);
        useroffset++;
    }

    i++;




    //this prints stuff after the current
    iterator = current_lobby;
    while (iterator.next != NULL) {

        SETPOS(6 + 2 * i + useroffset, 6);
        printf("%s (%d/%d)", iterator.name, iterator.current_users, iterator.max_users);

        SETPOS(6 + 2 * i + useroffset, COLS - 30);
        switch(iterator.type){
            case 0:
                printf("Duel");
                break;
            case 1:
                printf("Team");
                break;
            case 2:
                printf("Blitz");
                break;
        }

        iterator = *(iterator.next);
        i++;
    }

    SETPOS(2, 3);
    printf("Open Lobbies (Up/Down to select, %sJ%soin, %sC%sreate, %sS%start, %sH%selp)", XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL, XT_CH_UNDERLINE, XT_CH_NORMAL);

    xt_par0(XT_CH_BOLD);
    SETPOS(3, 3);
    printf("terminvaders MuLtIpLaYeR Lobby");

    xt_par0(XT_CH_DEFAULT);

    SETPOS(3 * ROWS / 4, COLS / 2 - 2);
    xt_par0(XT_CH_RED);
    printf("Quit");


    /*
    //display lobbys here

    xt_par0(XT_CH_NORMAL);
    xt_par0(XT_CH_BOLD);
    SETPOS(6, 4);
    printf(">");
    xt_par0(XT_CH_INVERSE); 
    SETPOS(6, 6);
    printf("join this room if you have swag (4/10)");
    xt_par0(XT_CH_NORMAL);
    SETPOS(7, 10);
    printf("Earwig");
    SETPOS(8, 10);
    printf("VjiaoBlack");
    SETPOS(9, 10);
    printf("Infernous");
    SETPOS(10, 10);
    printf("jeuwshuawakeup");
    SETPOS(6, COLS - 30);
    printf("Blitz");//mode  

    xt_par0(XT_CH_NORMAL);
    SETPOS(12, 8);
    printf("join this room if you dont have swag :( (2/4)");
    SETPOS(12, COLS - 30);
    printf("Team");

    SETPOS(14, 8);
    printf("pls dont join this room ty (1/2)");
    SETPOS(14, COLS - 30);
    printf("Duel");

                        /*
                        xt_par0(XT_CH_BOLD);
                        xt_par0(XT_CH_INVERSE);
                        switch (choice) { // makes the button highlighted
                            case 0:
                                SETPOS(ROWS / 2, COLS / 2 - 2);
                                xt_par0(XT_CH_GREEN);
                                printf("Play");
                                SETPOS(ROWS, COLS);
                                break;
                            case 1:
                                SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                                xt_par0(XT_CH_WHITE);
                                xt_par0(XT_CH_BOLD);
                                printf("Settings");
                                SETPOS(ROWS, COLS);
                                break;
                            case 2:
                                SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                                xt_par0(XT_CH_RED);
                                printf("Quit");
                                SETPOS(ROWS, COLS);
                                break;
                        }
                        xt_par0(XT_CH_NORMAL);
                        */

    fflush(stdout);
}

int lobby() {
    xt_par0(XT_CLEAR_SCREEN);
    drawlobby(0);
    int key, choice = 0;
    while(1) {
        if (getkey() == 'q'){
            xt_par0(XT_CLEAR_SCREEN);
            SETPOS(0,0);
            return MENU_QUIT; 
        }  
    }
                        /*
                        while (1) {
                            switch (key = getkey()) {
                            case KEY_UP:
                            case 'w':
                                xt_par0(XT_CH_DEFAULT);
                                switch (choice) { // makes the previous button 'normal'
                                    case 0:
                                        SETPOS(ROWS / 2, COLS / 2 - 2);
                                        xt_par0(XT_CH_GREEN);
                                        printf("Play");
                                        break;
                                    case 1:
                                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                                        xt_par0(XT_CH_WHITE);
                                        printf("Settings");
                                        break;
                                    case 2:
                                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                                        xt_par0(XT_CH_RED);
                                        printf("Quit");
                                        break;
                                }
                                choice = !choice ? 2 : choice - 1;
                                xt_par0(XT_CH_BOLD);
                                xt_par0(XT_CH_INVERSE);
                                switch (choice) { // makes the button highlighted
                                    case 0:
                                        SETPOS(ROWS / 2, COLS / 2 - 2);
                                        xt_par0(XT_CH_GREEN);
                                        printf("Play");
                                        SETPOS(ROWS, COLS);
                                        break;
                                    case 1:
                                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                                        xt_par0(XT_CH_WHITE);
                                        xt_par0(XT_CH_BOLD);
                                        printf("Settings");
                                        SETPOS(ROWS, COLS);
                                        break;
                                    case 2:
                                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                                        xt_par0(XT_CH_RED);
                                        printf("Quit");
                                        SETPOS(ROWS, COLS);
                                        break;
                                }
                                xt_par0(XT_CH_NORMAL);
                                break;
                            case KEY_DOWN:
                            case 's':
                                xt_par0(XT_CH_DEFAULT);
                                switch (choice) { // makes the previous button 'normal'
                                    case 0:
                                        SETPOS(ROWS / 2, COLS / 2 - 2);
                                        xt_par0(XT_CH_GREEN);
                                        printf("Play");
                                        break;
                                    case 1:
                                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                                        xt_par0(XT_CH_WHITE);
                                        printf("Settings");
                                        break;
                                    case 2:
                                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                                        xt_par0(XT_CH_RED);
                                        printf("Quit");
                                        break;
                                }
                                choice = (choice + 1) % 3;
                                xt_par0(XT_CH_BOLD);
                                xt_par0(XT_CH_INVERSE);
                                switch (choice) { // makes the button highlighted
                                    case 0:
                                        SETPOS(ROWS / 2, COLS / 2 - 2);
                                        xt_par0(XT_CH_GREEN);
                                        printf("Play");
                                        SETPOS(ROWS, COLS);
                                        break;
                                    case 1:
                                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                                        xt_par0(XT_CH_WHITE);
                                        xt_par0(XT_CH_BOLD);
                                        printf("Settings");
                                        SETPOS(ROWS, COLS);
                                        break;
                                    case 2:
                                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                                        xt_par0(XT_CH_RED);
                                        printf("Quit");
                                        SETPOS(ROWS, COLS);
                                        break;
                                    }
                                    xt_par0(XT_CH_NORMAL);
                                    break;
                                case KEY_ENTER:
                                    return choice;
                                case 'q':
                                    return MENU_QUIT;
                            }
                        }
                        */
}



void dispframe(){
    
    int cursor_r = 1, cursor_c = 1;
    SETPOS(1, 1);
    while (cursor_c <= get_cols()) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        putchar('-');
        cursor_r = get_rows();
        SETPOS(cursor_r, cursor_c);
        putchar('-');
        cursor_c++;
        //fflush(stdout);
    }
    cursor_r = 2;
    while (cursor_r <= get_rows()) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        putchar('|');
        cursor_c = get_cols();
        SETPOS(cursor_r, cursor_c);
        putchar('|');
        cursor_r++;
        //fflush(stdout);
    }
}
