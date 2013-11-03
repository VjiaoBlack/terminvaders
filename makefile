terminvaders: build build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o
	gcc -g build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o -o terminvaders

build/keyboard.o: xterm/keyboard.c
	gcc -g -c xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: xterm/xterm_control.c
	gcc -g -c xterm/xterm_control.c -o build/xterm_control.o

build/game.o: game.c game.h graphics.h terminvaders.h
	gcc -g -c game.c -o build/game.o

build/graphics.o: graphics.c graphics.h
	gcc -g -c graphics.c -o build/graphics.o

build/terminvaders.o: terminvaders.c terminvaders.h game.h graphics.h
	gcc -g -c terminvaders.c -o build/terminvaders.o

build/settings.o: settings.c settings.h terminvaders.h
	gcc -g -c settings.c -o build/settings.o

build:
	mkdir build

clean:
	rm -rf build terminvaders
