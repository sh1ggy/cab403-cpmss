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
// Setting number of buckets to 100
#define BUCKETS_SIZE 100
size_t buckets = BUCKETS_SIZE;
htab_t h;

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    printf("CREATING SHARED MEMORY\n");
    // Remove any previous instance of the shared memory object, if it exists.
    shm_unlink(share_name);
    
    shm -> name = share_name;

    // shm init, saved to shm_fd
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 
    // Error check
    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    // ftruncate sets capacity of shm object + error check
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

/**
* HASHTABLE
*/
int platesInit(  ) {
    if (!htab_init(&h, buckets))
    {
        printf("failed to initialise hash table\n");
        return EXIT_FAILURE;
    }
    return 0;
} 

// https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
void readPlates( const char * filename, const char * mode ) {
    platesInit();
    FILE* fp;
    int count = 0;
    fp = fopen(filename, mode);
    if (fp == NULL) {
        perror("Failed: ");
        return;
    }

    char buffer[256];

    while (fgets(buffer, sizeof buffer, fp)) {
        buffer[strcspn(buffer, "\n")] = 0;
        // printf("%s\n", buffer);
        char *plate = (char *)malloc(sizeof(char)*100);  
        strcpy(plate, buffer);
        htab_add(&h, plate, count);
        
        // printf("%s\n HASH: ", plate);
        // fgets(plate, 100 - 1, fp);
        
        count++;
    }

    htab_print(&h);
    // item_print(htab_find(&h, "480GML"));

    fclose(fp);
    return;
}

bool checkPlate( char plate[6] ) {
    if (htab_find(&h, plate) != NULL) {
        printf("\n---------FOUND PLATE---------\n");
        printf("VALID PLATE: %s\n", plate);
        // exit(0); // COMMENT OUT LATER< THIS IS FOR TESTING
        return true;
    }
    return false;
}

/**
* GENERATING PLATES TO BE CHECKED
*/

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
    // REMOVE SLEEP TO TEST PLATE GENERATION
    sleep(plateGenerateMS); 

    // Flip while condition for generateCars()
    flag = 1;
    
    // Unlock mutex protection
    pthread_mutex_unlock(&mutex);

    return 0;
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



void *generateCars() {
    char plate[6];

    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);

    // Wait for the time till new license plate to be generated
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
    } 

    // Generate the plate and assign
    strcpy(plate, generatePlate());
    printf("PLATE: %s\n", plate);

    // If true save to shared memory entrance LPR 
    checkPlate(plate);

    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 
    return 0;
}

void simulatorMain() {
    shared_memory_t shm;
    create_shared_object( &shm, SHARE_NAME );

    readPlates("plates.txt", "r");

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

    return;
    
}

