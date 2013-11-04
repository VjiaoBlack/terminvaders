#include <stdio.h>
#include "settings.h"
#include "terminvaders.h"

static int rows = 30;
static int cols = 80;

int get_rows(void) {
    return rows;
}

int get_cols(void) {
    return cols;
}

void load_rc(void) {
    FILE *fp;
    if (!(fp = fopen("preferences.txt", "r"))) {
        fp = fopen("preferences.txt", "w");
        fputs("030080", fp);
        fclose(fp);
        fp = fopen("preferences.txt", "r");
    }
    rows = 100 * (fgetc(fp) - '0') + 10 * (fgetc(fp) - '0') + (fgetc(fp) - '0');
    cols = 100 * (fgetc(fp) - '0') + 10 * (fgetc(fp) - '0') + (fgetc(fp) - '0');
    fclose(fp);
    return;
}

static void write_preferences(void) {
    FILE *fp = fopen("preferences.txt", "w");
    fputc((char) (rows / 100 + '0'), fp);
    fputc((char) (rows % 100 / 10 + '0'), fp);
    fputc((char) (rows % 10 + '0'), fp);
    fputc((char) (cols / 100 + '0'), fp);
    fputc((char) (cols % 100 / 10 + '0'), fp);
    fputc((char) (cols % 10 + '0'), fp);
    fclose(fp);
}

void configloop(void) {
    ///-- here are the variables --//
    char key = ' ';
    int config = 2;
    char a, b, c;

    int cursor_r = 1, cursor_c = 1;
    CLRSCRN();
    SETPOS(1, 1);
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_c++;
    }
    cursor_r = 2;
    while (cursor_r < ROWS) {
        cursor_c = 1;
        SETPOS(cursor_r, cursor_c);
        printf("|");
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        printf("|");
        cursor_r++;
    }

    SETPOS(ROWS / 4, COLS / 2 - 7);
    xt_par0(XT_CH_YELLOW);
    xt_par0(XT_CH_BOLD);
    printf("{[ Settings ]}");
    xt_par0(XT_CH_NORMAL);

    SETPOS(ROWS / 2, COLS / 2 - 5);
    printf("Height: %d", rows);

    SETPOS(5 * ROWS / 8, COLS / 2 - 4);
    printf("Width: %d", cols);

    SETPOS(3 * ROWS / 4, COLS / 2 - 2);
    xt_par0(XT_CH_BOLD);
    xt_par0(XT_CH_WHITE);
    xt_par0(XT_CH_INVERSE);
    printf("Back");
    SETPOS(ROWS, COLS);
    xt_par0(XT_CH_NORMAL);

    while (1) {
        switch ((key = getkey())) {
            case KEY_UP:
            case 'w':
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                switch (config) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        break;
                }
                config = !config ? 2 : config - 1;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (config) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                break;
            case KEY_DOWN:
            case 's':
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                switch (config) { // makes the previous button 'normal'
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        break;
                }
                config = (config + 1) % 3;
                xt_par0(XT_CH_BOLD);
                xt_par0(XT_CH_INVERSE);
                switch (config) { // makes the button highlighted
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        printf("Height: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        printf("Width: ");
                        SETPOS(ROWS, COLS);
                        break;
                    case 2:
                        SETPOS(3 * ROWS / 4, COLS / 2 - 2);
                        printf("Back");
                        SETPOS(ROWS, COLS);
                        break;
                }
                xt_par0(XT_CH_NORMAL);
                xt_par0(XT_CH_WHITE);
                break;
            case 'q':
                return;
            case KEY_ENTER:
                switch (config) {
                    case 0:
                        SETPOS(ROWS / 2, COLS / 2 + 3);
                        while ( (a = getkey()) == KEY_NOTHING);
                        printf("%c", a);
                        while ( (b = getkey()) == KEY_NOTHING);
                        printf("%c", b);
                        while ( (c = getkey()) == KEY_NOTHING);
                        printf("%c", c);
                        rows = 100 * (a - '0') + 10 * (b - '0') + (c - '0');
                        write_preferences();
                        SETPOS(ROWS / 2, COLS / 2 - 5);
                        break;
                    case 1:
                        SETPOS(5 * ROWS / 8, COLS / 2 + 3);
                        while ( (a = getkey()) == KEY_NOTHING);
                        printf("%c", a);
                        while ( (b = getkey()) == KEY_NOTHING);
                        printf("%c", b);
                        while ( (c = getkey()) == KEY_NOTHING);
                        printf("%c", c);
                        cols = 100 * (a - '0') + 10 * (b - '0') + (c - '0');
                        write_preferences();
                        SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                        break;
                    case 2:
                        return;
                }
            }
    }
}
