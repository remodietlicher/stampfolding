CC=mpic++
LIB=
INCLUDE=

build: stampfolding

stampfolding: main.o domino.o timer.o
	$(CC) $(LIB) $(INCLUDE) main.o domino.o timer.o -o stampfolding

main.o: main.cpp
	$(CC) $(LIB) $(INCLUDE) -c main.cpp
	
domino.o: domino.cpp
	$(CC) $(LIB) $(INCLUDE) -c domino.cpp
	
timer.o: timer.cpp
	$(CC) $(LIB) $(INCLUDE) -c timer.cpp

clear:
	rm -rf *o stampfolding

rebuild: clear build
