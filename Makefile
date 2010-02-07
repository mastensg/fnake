CC = cc
GDBFLAGS = -O0 -ggdb -g3
CFLAGS = $(GDBFLAGS) -Wall -pedantic -I/usr/include/GL -I/usr/X11R6/include -I/usr/local/include `Magick-config --cflags`
LDFLAGS = -lm -lGL -lglut -L/usr/X11R6/lib -L/usr/local/lib `Magick-config --ldflags --libs`

all:
	$(CC) $(CFLAGS) $(LDFLAGS) fnake.c -o fnake

clean:
	rm -f fnake.o fnake
