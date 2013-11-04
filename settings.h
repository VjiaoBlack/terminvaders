#define CLRSCRN() xt_par0(XT_CLEAR_SCREEN);

#define ROWS get_rows()
#define COLS get_cols()

void configloop(void);

int get_rows(void);
int get_cols(void);
void load_rc(void);
