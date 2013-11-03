#define ROWS 30
#define COLS 80


#define setrc(x, y) xt_par2(XT_SET_ROW_COL_POS, (x), (y))
#define clrscrn() xt_par0(XT_CLEAR_SCREEN);