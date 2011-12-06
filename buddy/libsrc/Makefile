
INSTALL=..
LIB=$(INSTALL)/lib
INCLUDE=$(INSTALL)/include
CC=gcc
CFLAGS=-Wall -O -g -fPIC -shared -I$(INCLUDE)/
LFLAGS=-L$(LIB) -lmybuddylib


LIBOBJS=buddy.o

all: libs

libs: libmybuddylib.so libmybuddylib.a

libmybuddylib.so: $(LIBOBJS)
	$(LD) -shared -fPIC -o $@ $(LIBOBJS)
	
libmybuddylib.a: $(LIBOBJS)
	$(AR) rcv $@ $(LIBOBJS)
	ranlib $@
	
install: libs
	install --mode=644 buddy.h $(INCLUDE)/
	install --mode=644 common.h $(INCLUDE)/
	install --mode=755 libmybuddylib.so $(LIB)/libmybuddylib.so
	install --mode=755 libmybuddylib.a $(LIB)/libmybuddylib.a
	
installclean: clean
	rm --force $(LIB)/libmybuddylib.so
	rm --force $(LIB)/libmybuddylib.a
	rm --force $(INCLUDE)/buddy.h
	rm --force $(INCLUDE)/common.h
	
clean:
	rm --force buddy *.o a.out core libmybuddylib.so libmybuddylib.a
	