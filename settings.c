#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "settings.h"
#include "terminvaders.h"
#include "star.h"

static int rows = 40;
static int cols = 100;

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
        fputs("40:120", fp);
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
    int test = 0;
    int lim = 3;
    while (cursor_c <= COLS) {
        cursor_r = 1;
        SETPOS(cursor_r, cursor_c);
        printf("-");
        cursor_r = ROWS;
        SETPOS(cursor_r, cursor_c);
        printf("-");
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
        printf("|");
        cursor_c = COLS;
        SETPOS(cursor_r, cursor_c);
        printf("|");
        cursor_r++;
        if (test == lim){
            fflush(stdout);
            test = 0;
        }
        test++;
    }

    SETPOS(ROWS / 4, COLS / 2 - 7);
    xt_par0(XT_CH_YELLOW);
    xt_par0(XT_CH_BOLD);
    printf("{[ Settings ]}");
    xt_par0(XT_CH_NORMAL);
    //fflush(stdout);

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
    //fflush(stdout);
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

static int read_int(int rows) {
    char key;
    int len= 0;
    //  int i;
    int place = 0; // this number indicates the place value of the input digit. i.e. tens, hundreds.
    int result = 0;
    char* data = malloc(sizeof(char) * 4); // no HD displays here; fourth is a "\0"
    int pos = 0;
    int i;

    data[0] = data[1] = data[2] = data[3] = '\0';
    /*
    if (rows == ROWS / 2) { // then its height
        for (i = 2; i >= 0; i--) {
            if (ROWS / (int) pow(10, i) > 0)
                data[place++] = (char) (((int)'0') + (ROWS / ((int) pow(10,i))));
        }
    } else {
        for (i = 2; i >= 0; i--) {
            if (COLS / (int) pow(10, i) > 0)
                data[place++] = (char) (((int)'0') + (COLS / ((int) pow(10,i))));
        }
    }
*/
    place = 0;

    while (1) {
        SETPOS(rows, COLS / 2 + 3);
        printf("%s   ", data);
        SETPOS(rows, COLS / 2 + 3 + pos);
        while ((key = getkey()) == KEY_NOTHING);


        if (key >= '0' && key <= '9' && pos < 3){
            SETPOS(rows, COLS / 2 + 3 + pos);
            putchar(key);
            for(int i = 1; i >= pos; i--) {
                data[i+1] = data[i];
            }
            data[pos] = key;
            pos++;
            len++;
            SETPOS(rows, COLS / 2 + 3);
            printf("%s   ", data);
            SETPOS(rows, COLS / 2 + 3 + pos);
        } else switch (key) {
            case KEY_LEFT:
                if (pos > 0)
                    pos--;
                SETPOS(rows, COLS / 2 + 3 + pos);
                break;
            case KEY_RIGHT:
                if (pos < 3 && pos < len)
                    pos++;
                SETPOS(rows, COLS / 2 + 3 + pos);
                break;
            case KEY_BACKSPACE:
                if (pos > 0) {
                    for (i = pos - 1; i <= 2; i++) {
                        data[i] = data[i+1];
                    }
                    pos--;
                    len--;
                    SETPOS(rows, COLS / 2 + 3 + pos);
                    putchar(' ');
                    SETPOS(rows, COLS / 2 + 3);
                    printf("%s   ", data);
                    SETPOS(rows, COLS / 2 + 3 + pos);
                }
                break;
            case KEY_DELETE:
                if (pos < 3) {
                    for (i = pos; i <= 2; i++) {
                        data[i] = data[i+1];
                    }
                }
                SETPOS(rows, COLS / 2 + 3);
                printf("%s   ", data);
                SETPOS(rows, COLS / 2 + 3 + pos);
                break;
            case KEY_ENTER:
                for (i = 2; i >= 0; i--) {
                    if (data[i] != '\0')
                        result += pow(10, place++) * (data[i] - '0');
                }
                return result;
            default:
                break;
        }
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
                            rows = read_int(ROWS / 2);
                            write_preferences();
                            SETPOS(ROWS / 2, COLS / 2 - 5);
                            break;
                        case 1:
                            SETPOS(5 * ROWS / 8, COLS / 2 + 3);
                            cols = read_int(5 * ROWS / 8);
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
