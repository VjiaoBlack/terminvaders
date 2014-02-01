#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xterm/xterm_control.c"
#include "terminvaders.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void dispframe();
int lobby();
void drawlobby(int);
