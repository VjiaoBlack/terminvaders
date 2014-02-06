#include "star.h"


void init(star_t* stars) {
    int i;
    for (i = 0; i < numstars; ++i) {
        int rox = 0;
        int roy = 0;
        while (abs(rox) - 2 < 0)
            rox = rand() % 400 - 200;
        while (abs(roy) - 1 < 0)
            roy = rand() % 200 - 100;

        int x = ( (rox * xo) / (do + xo) ) + get_cols() / 2; //( (rox * sqrt(3)) / (do + xo)) * get_cols() + (get_cols() / 2);
        int y = ( (roy * xo) / (do + xo) ) + get_rows() / 2; //( (roy * sqrt(3)) / (do + xo)) * get_rows() + (get_rows() / 2);

        int xwait = calculate_wait(1, rox, roy);
        int ywait = calculate_wait(0, rox, roy);

        int d = sqrt((x - get_cols() / 2) * (x - get_cols() / 2) + (y - get_rows() / 2) * (y - get_rows() / 2) * 4);

        stars[i] = (star_t) {x, y, xwait, ywait, 0, 0, rox, roy, d};
    }
}


void update(star_t* stars) {
    int i;
    for (i = 0; i < numstars; ++i) {

        stars[i].d = sqrt((stars[i].x - get_cols() / 2) * (stars[i].x - get_cols() / 2) + (stars[i].y - get_rows() / 2) * (stars[i].y - get_rows() / 2 ) * 4);

        if (stars[i].xwait - stars[i].xwaited <= 0) {
            stars[i].xwaited = 0;
            stars[i].xwait = calculate_wait(1, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));
            stars[i].ywait = calculate_wait(0, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));

            if (stars[i].ywait <= 3) {
                stars[i].ywait = calculate_wait(0, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));
                if (stars[i].y > get_rows() / 2)
                    stars[i].y = stars[i].y + 1;
                else
                    stars[i].y = stars[i].y - 1;
            }

            else if (stars[i].x > get_cols() / 2)
                stars[i].x = stars[i].x + 1;
            else
                stars[i].x = stars[i].x - 1;

        } else {
            stars[i].xwaited++;
        }
        if (stars[i].ywait - stars[i].ywaited <= 0) {
            stars[i].ywaited = 0;
            stars[i].xwait = calculate_wait(1, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));
            stars[i].ywait = calculate_wait(0, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));


            if (stars[i].xwait <= 3) {
                stars[i].xwait = calculate_wait(1, abs(stars[i].x - get_cols() / 2), abs(stars[i].y - get_rows() / 2));

                if (stars[i].x > get_cols() / 2)
                    stars[i].x = stars[i].x + 1;
                else
                    stars[i].x = stars[i].x - 1;
            }

            else if (stars[i].y > get_rows() / 2)
                stars[i].y = stars[i].y + 1;
            else
                stars[i].y = stars[i].y - 1;


        } else {
            stars[i].ywaited++;
        }

        if (stars[i].x >= get_cols() || stars[i].x <= 0 || stars[i].y >= get_rows() || stars[i].y <= 0){
            int rox = 0;
            int roy = 0;
            while (abs(rox) - 2 < 0)
                rox = rand() % 400 - 200;
            while (abs(roy) - 1 < 0)
                roy = rand() % 200 - 100;

            int x = ( (rox * xo) / (do + xo) ) + get_cols() / 2; //( (rox * sqrt(3)) / (do + xo)) * get_cols() + (get_cols() / 2);
            int y = ( (roy * xo) / (do + xo) ) + get_rows() / 2; //( (roy * sqrt(3)) / (do + xo)) * get_rows() + (get_rows() / 2);

            int xwait = calculate_wait(1, abs(rox), abs(roy));
            int ywait = calculate_wait(0, abs(rox), abs(roy));

            stars[i] = (star_t) {x, y, xwait, ywait, 0, 0, rox, roy, do};
        }

    }
}

void display(star_t* stars) {
    int i;
    CLRSCRN();
    for (i = 0; i < numstars; ++i) {
        SETPOS(stars[i].y, stars[i].x);

        if (abs(stars[i].x - get_cols() / 2) + 2 * abs(stars[i].y - get_rows() / 2) > 60) {
            putchar('#');
        } else if (abs(stars[i].x - get_cols() / 2) + 2 * abs(stars[i].y - get_rows() / 2) > 35) {
            if (abs((stars[i].x - get_cols() / 2) - 2 * (stars[i].y - get_rows() / 2)) < 12) {
                putchar('\\');
            }
            else if (abs((stars[i].x - get_cols() / 2) + 2 * (stars[i].y - get_rows() / 2)) < 12) {
                putchar('/');
            }
            else if (abs(stars[i].x - get_cols() / 2) < 20) {
                putchar('|');
            }
            else if (abs(stars[i].y - get_rows() / 2 ) < 20) {
                putchar('_');
            }
        } else {
            putchar('.');
        }
        xt_par0(XT_CH_NORMAL);

    }

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
    }
}

int handle_input(int running) {
    int key;
    while ((key = getkey()) != KEY_NOTHING) {
        switch (key) {
            case 'q':
                running = 0;
                break;
        }
    }
    return running;
}

int calculate_wait(int is_horiz, int x, int y) { // x and y are displacements from center.

    double d = sqrt(x * x / 16 + y * y / 8) + 5;
    double r;
    if (is_horiz) {
        r =  ((double)y / (double)x) + .1;
        r *= 2;
    } else {
        r =  ((double)x / (double)y) + .1;
    }

    int fd = 2000 / (d * d);

    return (r * fd);

}
