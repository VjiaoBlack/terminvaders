COMPILE=gcc -g -O2 -Wall -c
LINK=gcc -g -O2 -Wall

all: terminvaders
	rm -f preferences.txt

terminvaders: build build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o build/star.o build/lobby.o
	$(LINK) build/keyboard.o build/xterm_control.o build/game.o build/graphics.o build/terminvaders.o build/settings.o build/star.o build/lobby.o -o terminvaders

build/keyboard.o: xterm/keyboard.c
	$(COMPILE) xterm/keyboard.c -o build/keyboard.o

build/xterm_control.o: xterm/xterm_control.c
	$(COMPILE) xterm/xterm_control.c -o build/xterm_control.o

build/game.o: game.c game.h graphics.h terminvaders.h settings.h
	$(COMPILE) game.c -o build/game.o

build/graphics.o: graphics.c graphics.h
	$(COMPILE) graphics.c -o build/graphics.o

build/terminvaders.o: terminvaders.c terminvaders.h game.h graphics.h settings.h star.h lobby.h
	$(COMPILE) terminvaders.c -o build/terminvaders.o

build/settings.o: settings.c settings.h terminvaders.h star.h
	$(COMPILE) settings.c -o build/settings.o

build/star.o: star.c star.h
	$(COMPILE) star.c -o build/star.o

build/lobby.o: lobby.c lobby.h
	$(COMPILE) lobby.c -o build/lobby.o

build:
	mkdir build

clean:
	rm -rf build terminvaders preferences.txt
