#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

// Defines

#define ROWS 30
#define COLS 80

#define MENU_PLAY     0
#define MENU_SETTINGS 1
#define MENU_QUIT     2

// Macros

#define SETPOS(x, y) xt_par2(XT_SET_ROW_COL_POS, (x), (y))

// Functions

void clear(void);
