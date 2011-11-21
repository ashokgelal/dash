
all: dash

dash-all:
	cd generic-list; make; cp lib/*.* ../lib/ 
	cd src; make
dash:
	cd src; make; cp ./dash ../
	
clean-all:
	rm -f lib/*.*; cd generic-list; make clean
	cd src; make clean
clean:
	cd src; make clean
	rm -f ./dash
