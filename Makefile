
all: dash

dash-all: dash
	cd generic-list; make; cp lib/*.* ../lib/ 
dash:
	cd src; make; cp ./dash ../
	
clean-all:
	rm -f lib/*.*; cd generic-list; make clean
	cd src; make clean
clean:
	cd src; make clean
	rm -f ./dash
