#include <stdio.h>
#include "settings.h"
#include "terminvaders.h"
#include "star.h"

static int rows = 44;
static int cols = 180;

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
        fputs("44:180", fp);
        fclose(fp);
        fp = fopen("preferences.txt", "r");
    }
    fscanf(fp, "%d:%d", &rows, &cols);
    fclose(fp);
    return;
}

static void draw_menu(int config) {
    int cursor_r = 1, cursor_c = 1;
    SETPOS(1, 1);
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_c++;
        fflush(stdout);
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
        fflush(stdout);
    }

    SETPOS(ROWS / 4, COLS / 2 - 7);
    xt_par0(XT_CH_YELLOW);
    xt_par0(XT_CH_BOLD);
    printf("{[ Settings ]}");
    xt_par0(XT_CH_NORMAL);
    fflush(stdout);

    SETPOS(ROWS / 2, COLS / 2 - 5);
    if (config == 0) {
        xt_par0(XT_CH_BOLD);
        xt_par0(XT_CH_INVERSE);
        printf("Height: ");
        xt_par0(XT_CH_NORMAL);
    }
    else
        printf("Height: ");
    printf("%d", rows);
    fflush(stdout);
    SETPOS(5 * ROWS / 8, COLS / 2 - 4);
    if (config == 1) {
        xt_par0(XT_CH_BOLD);
        xt_par0(XT_CH_INVERSE);
        printf("Width: ");
        xt_par0(XT_CH_NORMAL);
    }
    else
        printf("Width: ");
    printf("%d", cols);

    SETPOS(3 * ROWS / 4, COLS / 2 - 2);
    if (config == 2) {
        xt_par0(XT_CH_BOLD);
        xt_par0(XT_CH_INVERSE);
        printf("Back");
        xt_par0(XT_CH_NORMAL);
    }
    else
        printf("Back");
    SETPOS(ROWS, COLS);
    fflush(stdout);
}

static int read_int(void) {
    char key;
    int result = 0;

    while (1) {
        while ((key = getkey()) == KEY_NOTHING);
        if (key < '0' || key > '9')
            return result;
        putchar(key);
        result = (result * 10) + (key - '0');
    }
}

static void write_preferences(void) {
    FILE *fp = fopen("preferences.txt", "w");
    fprintf(fp, "%d:%d", rows, cols);
    fclose(fp);
}

void configloop(void) {
    ///-- here are the variables --//
    char key = ' ';
    int config = 2;

    dispframe();

    star_t* stars = malloc(sizeof(star_t) * numstars);
    init(stars);

    while (1) {
        update(stars);
        display(stars);
        draw_menu(config);
        usleep(1000000/fps);

        while ((key = getkey()) != KEY_NOTHING){
            update(stars);
            display(stars);
            draw_menu(config);
            usleep(1000000/fps);
            switch (key) {
                case KEY_UP:
                case 'w':
                    config = !config ? 2 : config - 1;  
                    break;
                case KEY_DOWN:
                case 's':
                    config = (config + 1) % 3;
                    break;
                case 'q':
                    return;
                case KEY_ENTER:
                    switch (config) {
                        case 0:
                            SETPOS(ROWS / 2, COLS / 2 + 3);
                            rows = read_int();
                            write_preferences();
                            SETPOS(ROWS / 2, COLS / 2 - 5);
                            break;
                        case 1:
                            SETPOS(5 * ROWS / 8, COLS / 2 + 3);
                            cols = read_int();
                            write_preferences();
                            SETPOS(5 * ROWS / 8, COLS / 2 - 4);
                            break;
                        case 2:
                            return;
                    }
            }
        }
    }
}
