#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>    

#include "simulator.h"
#include "lpr.h"

bool checkPlate( char plate[6] );

char *generatePlate(  );

void *generatePlateTime();

void *generateCars();


