#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>    
#include "cpmss.h"
#include "lpr.h"
#include "simulator.h"
#include "shm.h"

int main( int argc, char **argv )
{     
    htab_destroy(&h);
    
    simulatorMain();
    managerMain();
}