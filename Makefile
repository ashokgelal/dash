
all: budmem list dash

budmem:
	cd buddy; make
	cp buddy/lib/*.* ./lib/ 
	
list:
	cd generic-list; make
	cp generic-list/lib/*.* ./lib/ 
	
dash:
	cd src; make; cp ./dash ../
	
clean:
	cd src; make clean
	rm -f ./dash
	rm -f lib/*.*; cd generic-list; make clean
	cd buddy; make clean
	
#clean:
	#cd src; make clean
	#rm -f ./dash
