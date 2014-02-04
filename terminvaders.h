#include "settings.h"
#include "xterm/keyboard.h"
#include "xterm/xterm_control.h"

// Defines

#define MENU_PLAY     0
#define MENU_SETTINGS 2
#define MENU_QUIT     3
#define MULTIPLAYER   1

#define OFFSET (ROWS / 10)

// Macros

#define SETPOS(x, y) xt_par2(XT_SET_ROW_COL_POS, (x), (y))
