
INSTALL=..
INCLUDE=$(INSTALL)/include
LIBS=$(INSTALL)/lib
LFLAGS=-L$(LIBS) -lmylib
CC=gcc
#CFLAGS=-Wall -g  -static  -I$(INCLUDE)
CFLAGS=-Wall -g  -I$(INCLUDE)

all: TestList SimpleTest MyTest


TestList: TestList.o Object.o
	$(CC) $(CFLAGS) -o $@ TestList.o Object.o  $(LFLAGS)

SimpleTest: SimpleTest.o 
	$(CC) $(CFLAGS) -o $@ SimpleTest.o   $(LFLAGS)
	
MyTest: MyTest.o Object.o
	$(CC) $(CFLAGS) -o $@ MyTest.o Object.o   $(LFLAGS)

clean:
	rm --force list *.o TestList SimpleTest MyTest
