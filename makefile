COMPILE=gcc -g -O2 -Wall -lm -c
LINK=gcc -g -O2 -Wall -lm

all: terminvaders server
	rm -f preferences.txt

terminvaders: build build/keyboard.o build/xterm_control.o build/network.o build/transmit.o build/game.o build/graphics.o build/terminvaders.o build/settings.o build/star.o build/client.o build/lobby.o
	$(LINK) build/keyboard.o build/xterm_control.o build/network.o build/transmit.o build/game.o build/graphics.o build/terminvaders.o build/settings.o build/star.o build/client.o build/lobby.o -o terminvaders

server: build build/keyboard.o build/xterm_control.o build/network.o build/transmit.o build/server.o build/game.o build/graphics.o build/settings.o build/star.o build/client.o
	$(LINK) build/keyboard.o build/xterm_control.o build/network.o build/transmit.o build/server.o build/game.o build/graphics.o build/settings.o build/star.o build/client.o -o server

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

build/network.o: network.c network.h
	$(COMPILE) network.c -o build/network.o

build/transmit.o: transmit.c transmit.h network.c network.h server.c server.h
	$(COMPILE) transmit.c -o build/transmit.o

build/server.o: server.c server.h network.c network.h game.c game.h
	$(COMPILE) server.c -o build/server.o

build/client.o: client.c client.h game.c game.h
	$(COMPILE) client.c -o build/client.o

build/star.o: star.c star.h
	$(COMPILE) star.c -o build/star.o

build/lobby.o: lobby.c lobby.h
	$(COMPILE) lobby.c -o build/lobby.o

build:
	mkdir build

clean:
	rm -rf build terminvaders preferences.txt
