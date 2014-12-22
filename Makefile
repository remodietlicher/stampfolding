CC=mpic++
LIB=
INCLUDE=

build: stampfolding

stampfolding: main.o domino.o timer.o mpi_stamp.o
	$(CC) $(LIB) $(INCLUDE) main.o domino.o timer.o mpi_stamp.o -o stampfolding

main.o: main.cpp
	$(CC) $(LIB) $(INCLUDE) -c main.cpp
	
domino.o: domino.cpp
	$(CC) $(LIB) $(INCLUDE) -c domino.cpp
	
timer.o: timer.cpp
	$(CC) $(LIB) $(INCLUDE) -c timer.cpp
	
mpi_stamp.o: mpi_stamp.cpp
	$(CC) $(LIB) $(INCLUDE) -c mpi_stamp.cpp

clear:
	rm -rf *o stampfolding

rebuild: clear build
