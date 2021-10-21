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
        
    // Join runs the thread(function) after it exits once (simulator never exits so we dont have to join, manager exits so we join it)
    pthread_join( simulatorThread, NULL );
    pthread_join( managerThread, NULL );

    // simulatorMain();
    // managerMain();
}