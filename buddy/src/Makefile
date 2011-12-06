
INSTALL=..
INCLUDE=$(INSTALL)/include
LIBS=$(INSTALL)/lib
LFLAGS=-L$(LIBS) -lmybuddylib
CC=gcc

CFLAGS=-Wall -O -g -I$(INCLUDE)

all: buddy-test malloc-test

buddy-test: buddy-test.o
	$(CC) $(CFLAGS) -o $@ $? $(LFLAGS)

malloc-test: malloc-test.o 
	$(CC) $(CFLAGS) -o $@ $? $(LFLAGS)

clean:	
	/bin/rm -f *.o a.out buddy-test malloc-test
	