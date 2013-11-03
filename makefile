COMPILE=gcc -g -O2 -c
LINK=gcc -g -O2

terminvaders: build build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o
	$(LINK) build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o -o terminvaders

build/keyboard.o: xterm/keyboard.c
	$(COMPILE) xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: xterm/xterm_control.c
	$(COMPILE) xterm/xterm_control.c -o build/xterm_control.o

build/game.o: game.c game.h graphics.h terminvaders.h settings.h
	$(COMPILE) game.c -o build/game.o

build/graphics.o: graphics.c graphics.h
	$(COMPILE) graphics.c -o build/graphics.o

build/terminvaders.o: terminvaders.c terminvaders.h game.h graphics.h settings.h
	$(COMPILE) terminvaders.c -o build/terminvaders.o

build/settings.o: settings.c settings.h terminvaders.h
	$(COMPILE) settings.c -o build/settings.o

build:
	mkdir build

clean:
	rm -rf build terminvaders
