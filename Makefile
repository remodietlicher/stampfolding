CC=mpic++
LIB=
INCLUDE=

build: stampfolding

stampfolding: main.o domino.o
	$(CC) $(LIB) $(INCLUDE) main.o domino.o -o stampfolding

main.o: main.cpp
	$(CC) $(LIB) $(INCLUDE) -c main.cpp
	
domino.o: domino.cpp
	$(CC) $(LIB) $(INCLUDE) -c domino.cpp

clear:
	rm -rf *o stampfolding

rebuild: clear build
