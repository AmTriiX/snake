PKGS=sdl2
CFLAGS=-Wall -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2 SDL2_ttf`
LIBS=`pkg-config --libs sdl2 SDL2_ttf`

main: snake.c
	$(CC) $(CFLAGS) -o snake snake.c $(LIBS)