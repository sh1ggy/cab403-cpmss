CC = gcc # C compiler is gcc
CFLAGS = -Wall -pedantic # Show all reasonable warnings

all: cpmss

# TODO: firealarm.o
cpmss: manager.o lpr.o simulator.o cpmss.o -lm -lrt

manager.o: manager.c

lpr.o: lpr.c

simulator.o: simulator.c

cpmss.o: cpmss.c

# firealarm.o: firealarm.c

clean:
	rm -f cpmss *.o

run: 
	./cpmss

.PHONY: all clean # specifying that these are commands 
