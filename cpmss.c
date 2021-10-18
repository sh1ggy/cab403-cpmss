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

    sleep(1);    

    pthread_t simulatorThread, managerThread;

    pthread_create(&simulatorThread, NULL, simulatorMain, NULL );
    pthread_create(&managerThread, NULL, managerMain, NULL );
        
        // Runs both threads
    pthread_join( simulatorThread, NULL );
    pthread_join( managerThread, NULL );

    // simulatorMain();
    // managerMain();
}