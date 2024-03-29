CC = gcc # C compiler is gcc
CFLAGS = -Wall -pedantic # Show all reasonable warnings

all: cpmss

# TODO: firealarm.o
cpmss: manager.o simulator.o firealarm.o lpr.o shm.o plates.o cars.o cpmss.o -lm -lpthread -lrt

manager.o: manager.c

simulator.o: simulator.c

firealarm.o: firealarm.c

shm.o: shm.c

plates.o: plates.c

cars.o: cars.c

lpr.o: lpr.c

cpmss.o: cpmss.c


clean:
	rm -f cpmss *.o

run: 
	./cpmss 

dev: 
	valgrind --track-origins=yes ./cpmss
	
.PHONY: all clean # specifying that these are commands 
