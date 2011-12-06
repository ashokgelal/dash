
#all: library testcode
all: library

library:
	cd libsrc; make install
	
testcode:
	cd src; make
	
clean:
	cd libsrc; make installclean
	cd src; make clean
	