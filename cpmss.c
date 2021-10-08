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

// pthread_mutex_t plateMutex = PTHREAD_MUTEX_INITIALIZER;
/* global mutex for our program. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag = 0;

int generateInRange(int lower, int upper) {
    
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

void *generatePlateTime() {
    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);
    
    // Signal that work is being done
    pthread_cond_signal(&cond);

    // Generate time  
    int plateGenerateTime = generateInRange(1, 100);
    int plateGenerateMS;    
    printf("RAND TIME: %d\n", plateGenerateTime);
    
    // Calculate sleep for ms
    plateGenerateMS = msSleep(plateGenerateTime);
    sleep(plateGenerateMS);

    // Flip while condition for generateCars()
    flag = 1;
    
    // Unlock mutex protection
    pthread_mutex_unlock(&mutex);

    return 0;
}

int msSleep (long msec)
{
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

void *generateCars() {
    char plates[6];

    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);

    // Wait for the time till new license plate to be generated
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
        // sleep(plateGenerateMS);
    } 

    // Generate the plate and assign
    strcpy(plates, generatePlate());
    printf("PLATE: %s\n", plates);

    // Assign plate & then check the plate
    // Allow into car park if found
    
    // Discard if not found


    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 
    return 0;
}

int main( int argc, char **argv )
{     
    simulatorMain();
    srand(time(NULL));

    pthread_t threadWhileLoop, threadRange;
    // int threadWhileId, threadRangeId;
    // pthread_mutex_init(&mutex, NULL);
    // pthread_cond_init(&cond, NULL);
    
    // threadWhileId = pthread_create( &threadWhileLoop, NULL, generateCars, &threadWhileId );
    // threadRangeId = pthread_create( &threadRange, NULL, generateInRange, NULL );

    while (1) {
        // Create two threads to be ran simultaneously
        pthread_create(&threadWhileLoop, NULL, generateCars, NULL);
        pthread_create(&threadRange, NULL, generatePlateTime, NULL);
        
        // Runs both threads
        pthread_join( threadWhileLoop, NULL );
        pthread_join( threadRange, NULL );
    }

    return 0;
}