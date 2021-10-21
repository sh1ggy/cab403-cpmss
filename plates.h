#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>    

#include "simulator.h"
#include "lpr.h"

// flag for manager to use for the While Loop
extern bool flagPlateFound;
#define MAX_LEVEL_CAPACITY 20

// int *level1;
// int *level2;
// int *level3;
// int *level4;
// int *level5;

int level[5];

bool checkPlate( char plate[6] );

bool *generatePlate(  );

void *generatePlateTime();

void *scanningCars();

void *generateCars();



