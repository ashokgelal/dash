
INSTALL=..
INCLUDE=$(INSTALL)/include
LIBS = $(INSTALL)/lib
LFLAGS=-L$(LIBS) -lmylib -lreadline -lncurses
CC=gcc
CFLAGS=-g -Wall -I$(INCLUDE)

PROGS=dash 

all: $(PROGS)

dash: dash.o version.o Job.o
	$(CC) $(CFLAGS) -o $@ dash.o version.o Job.o  $(LFLAGS)

clean:
	/bin/rm -f *.o $(PROGS) a.out	
