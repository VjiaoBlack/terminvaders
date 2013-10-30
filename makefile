terminvaders: build build/keyboard.o build/xterm_control.o build/terminvaders.o
    gcc -g build/keyboard.o build/xterm_control.o build/terminvaders.o -o terminvaders

build/keyboard.o: xterm/keyboard.c
    gcc  -g -c xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: xterm/xterm_control.c
    gcc  -g -c xterm/xterm_control.c -o build/xterm_control.o

build/terminvaders.o: terminvaders.c
    gcc -g -c terminvaders.c -o build/terminvaders.o

build:
    mkdir build

clean:
    rm -rf build
    rm terminvaders
