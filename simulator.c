#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>    

#include "cpmss.h"
#include "simulator.h"
#include "lpr.h"

// shared memory libs
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>       
#include <fcntl.h>   
#include <stddef.h>       


/* global mutex for our program. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag = 0;

pthread_mutex_t plateGenerationMutex;
#define SHARE_NAME "PARKING"
#define PARKING_SIZE 2920
#define NUM_LEVELS 5

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    printf("CREATING SHARED MEMORY\n");
    // Remove any previous instance of the shared memory object, if it exists.
    shm_unlink(share_name);
    
    shm -> name = share_name;

    // Create the shared memory object, allowing read-write access, and saving the
    // resulting file descriptor in shm->fd. If creation failed, ensure 
    // that shm->data is NULL and return false.
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 
    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    // Set the capacity of the shared memory object via ftruncate. If the 
    // operation fails, ensure that shm->data is NULL and return false. 
    
    if (ftruncate(shm->fd, PARKING_SIZE) != 0){
        shm->data = NULL;
        return false;
    }

    // Otherwise, attempt to map the shared memory via mmap, and save the address
    // in shm->data. If mapping fails, return false.
    // shm -> data = mmap(0, PARKING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 

    // if (shm->data == MAP_FAILED)
    // {
    //     return false;
    // }

    // TO DO: SHARED MEMORY RETRIEVAL FUNCTION

    // NULL address ensures this is inserted appropriately into shared memory - the address is chosen by the OS
    
    carpark_t *carpark = mmap(NULL, sizeof(carpark_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = carpark;

    printf("CARPARK:\n");

    printf("LEVEL 1 -> ENTRANCE -> LPR -> PLATE:\n");
    char* plate = "123XXX";
    strcpy(carpark->entrances[0].sensor.plate, plate);
    printf("READING LPR PLATE: %s\n", carpark->entrances[0].sensor.plate);

    printf("LEVEL 1 -> ENTRANCE -> GATE -> STATUS:\n");
    char* status = "O";
    strcpy(carpark->entrances[0].gate.status, status);
    printf("READING BOOM GATE: %s\n", carpark->entrances[0].gate.status);
    
    printf("READING LPR PLATE, AGAIN: %s\n", carpark->entrances[0].sensor.plate);

    // printf("entrances offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, entrances));

    // printf("exits offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, exits));

    // printf("levels offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, levels));

    int carparkInt = sizeof(carpark_t);
    printf("ENTIRE CARPARK: %d\n", carparkInt);

    printf("DONE CREATING SHARED MEMORY\n");
    return true;
}

bool get_shared_object( shared_memory_t* shm, const char* share_name ) {
    // Get a file descriptor connected to shared memory object and save in 
    // shm->fd. If the operation fails, ensure that shm->data is 
    // NULL and return false.
    int shm_fd = shm_open(share_name, O_RDWR, 0);
    shm -> fd = shm_fd;
    if (shm_fd < 0) 
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Otherwise, attempt to map the shared memory via mmap, and save the address
    // in shm->data. If mapping fails, return false.
    // INSERT SOLUTION HERE
    carpark_t *map = mmap(0, 48, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = map;

    if (map == MAP_FAILED)
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Modify the remaining stub only if necessary.
    return true;
}

void simulatorTest(  ) {
    printf("Hello! I am the simulator");
    return;
}

char *generatePlate(  ) {
    // Mutex locks to protect the global random
    pthread_mutex_lock(&plateGenerationMutex);

    // Uses current time as the random seed
    char *plate = (char *)malloc(6);
    for (int i = 0; i < 3; i++) {
        int randIntNumber = rand();
        int randIntLetter = rand();

        // Generate random integers as a part of the plate
        randIntNumber %= 10;
        randIntNumber += 48;
        char randNumberConverted = randIntNumber;
        plate[i] = randNumberConverted;

        // Generate random letters as a part of the plate
        randIntLetter %= 26;
        randIntLetter += 65;
        char randLetterConverted = randIntLetter;
        plate[i+3] = randLetterConverted;

    }
    pthread_mutex_unlock(&plateGenerationMutex);
    return plate;

}

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

void simulatorMain() {
    shared_memory_t shm;
    create_shared_object( &shm, SHARE_NAME );

    srand(time(NULL));

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

