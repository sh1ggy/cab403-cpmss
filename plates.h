#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>   
#include <unistd.h>  

#include "simulator.h"
#include "lpr.h"

// flag for manager to use for the While Loop
extern bool flagPlateFound;
#define MAX_LEVEL_CAPACITY 20
#define RAISE_TIME 10
#define OPEN_TIME 20
#define LOWER_TIME 10

int level[5];

bool checkPlate( char plate[6] );

void *generatePlate(  );

void *generatePlateTime();

void *scanningCars();

void *generateCars();



