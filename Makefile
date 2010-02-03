CC = cc
GDBFLAGS = -O0 -ggdb -g3
CFLAGS = $(GDBFLAGS) -Wall -pedantic -I/usr/include/GL
LDFLAGS = -lm -lGL -lglut

all:
	$(CC) $(CFLAGS) $(LDFLAGS) fnake.c -o fnake

clean:
	rm -f fnake.o fnake
