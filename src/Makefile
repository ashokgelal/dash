
CC=gcc
CFLAGS=-g -Wall
LIBS=-lreadline -lncurses

PROGS=dash version

all: $(PROGS)

version: version.o
	$(CC) $(CFLAGS) -o $@ $<

dash: dash.o version.o
	$(CC) $(CFLAGS) -o $@ $< version.o $(LIBS)

clean:
	/bin/rm -f *.o $(PROGS) a.out	
