
all: list dash

list:
	cd generic-list; make
	cp generic-list/lib/*.* ./lib/ 
	
dash:
	cd src; make; cp ./dash ../
	
clean:
	cd src; make clean
	rm -f ./dash
	rm -f lib/*.*; cd generic-list; make clean
	
#clean:
	#cd src; make clean
	#rm -f ./dash
