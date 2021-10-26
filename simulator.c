#include "cpmss.h"
#include "shm.h"   
#include "simulator.h"
#include "lpr.h"
#include "plates.h"
#include "cars.h"

int generateInRange(int lower, int upper) {
    
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

int msSleep (long msec) {
    struct timespec ts;
    int result;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        // REMOVE SLEEP TO TEST PLATE GENERATION
        result = nanosleep(&ts, &ts); 
    } while (result && errno == EINTR);

    return result;
}

void *simulatorMain() {


    readPlates("plates.txt", "r");

    srand(time(NULL));

    for (int i = 0; i < 5; i++) {
        level[i] = 0;
    }

    pthread_t threadWhileLoop, threadRange;

    while (1) {
        // Create two threads to be ran simultaneously
        pthread_create(&threadWhileLoop, NULL, generateCars, NULL );
        pthread_create(&threadRange, NULL, generatePlateTime, NULL);
        
        // Runs both threads
        pthread_join( threadWhileLoop, NULL );
        pthread_join( threadRange, NULL );
        

    }

    return 0;
    
}



