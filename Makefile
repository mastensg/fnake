CC = cc
GDBFLAGS = -O0 -ggdb -g3
CFLAGS = -Wall -pedantic $(GDBFLAGS)
LDFLAGS = -lm -lGL -lglut

fnake:
	$(CC) $(CFLAGS) $(LDFLAGS) fnake.c -o fnake
