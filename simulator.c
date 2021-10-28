#include "cpmss.h"
#include "shm.h"   
#include "simulator.h"
#include "lpr.h"
#include "plates.h"
#include "cars.h"

// Generate and return a random number in the provided range
int generateInRange(int lower, int upper) {
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

// Function to convert to milliseconds for sleep  
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
        result = nanosleep(&ts, &ts); 
    } while (result && errno == EINTR);

    return result;
}

// The main thread function for the simulator.c file
void *simulatorMain() {
    // Read plates.txt, adding all entries to a hashtable
    readPlates("plates.txt", "r");

    // Seeded random to current time
    srand(time(NULL));

    // Initialise all level car capacities to 0
    for (int i = 0; i < 5; i++) {
        level[i] = 0;
    }

    // Initialisation of threads
    pthread_t threadWhileLoop, threadRange, threadTemp;

    while (1) {
        // Create three threads and run them simultaneously
        pthread_create(&threadWhileLoop, NULL, generateCars, NULL);
        pthread_create(&threadRange, NULL, generatePlateTime, NULL);
        pthread_create(&threadTemp, NULL, generateTemp, NULL);

        // Join runs the thread(function) after it exits once
        pthread_join(threadWhileLoop, NULL);
        pthread_join(threadRange, NULL);
        pthread_join(threadTemp, NULL);
    }

    return 0;
    
}



