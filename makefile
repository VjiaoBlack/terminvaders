terminvaders: build build/terminvaders.o
	gcc -g build/terminvaders.o -o terminvaders

build/terminvaders.o: terminvaders.c
	gcc -g -c terminvaders.c -o build/terminvaders.o

build:
	mkdir build

clean:
	rm -rf build
	rm terminvaders
