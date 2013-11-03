#define CLRSCRN() xt_par0(XT_CLEAR_SCREEN);

#define ROWS get_rows()
#define COLS get_cols()

int configloop(void);

int get_rows();
int get_cols();