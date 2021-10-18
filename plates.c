#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h> 
#include <unistd.h>   

#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"
#include "cars.h"

/* global mutex for our program. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag = 0;
bool flagPlateFound = false;
pthread_mutex_t plateGenerationMutex;

bool checkPlate( char plate[6] ) {
    if (htab_find(&h, plate) == NULL) {
        return false;
    }
    else {
        printf("\n---------FOUND PLATE---------\n");
        printf("VALID PLATE: %s\n", plate);
        // exit(0); // -------------COMMENT OUT LATER< THIS IS FOR TESTING
        sleep(3);
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

void *generatePlateTime() {
    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);
    
    // Signal that work is being done
    pthread_cond_signal(&cond);

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    // -------------- REMOVE SLEEP TO TEST PLATE GENERATION
    // Generate time  
    // int plateGenerateTime = generateInRange(1, 100);
    // int plateGenerateMS;    
    // printf("RAND TIME: %d\n", plateGenerateTime); //-- printing rand time
    
    // Calculate sleep for ms
    // plateGenerateMS = msSleep(plateGenerateTime);
    // sleep(plateGenerateMS); 

    // Flip while condition for generateCars()
    flag = 1;
    
    // Unlock mutex protection
    pthread_mutex_unlock(&mutex);

    return 0;
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
    printf("%s\n", plate);

    // If true save to shared memory entrance LPR 
    if (checkPlate(plate)) {
        int entranceRand = generateInRange(0, 4);
        shared_memory_t shm;
        get_shared_object(&shm, SHARE_NAME);
        printf("ENTRANCE: %d\n", entranceRand);

        //using "strncpy" instead of "strcpy" to adds a terminating null byte, and does not pad the target with (further) null bytes
        //"strcpy" was producing a strange box character for entrances 2-5 and not entrance 1. 
        strncpy(shm.data->entrances[entranceRand].sensor.plate, plate, 6);
        // destroy_shared_object(&shm);
        flagPlateFound = true;
        // INSERT ITEM PLATE CREATE THREAD
        // TODO: NULL CHAR WHEN PRINTING FROM INSERT_ITEM STRING

        initCars();    

        insert_item(plate);
        
        sleep(5);
    }

    else {
        flagPlateFound = false;
    }
    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 
    return 0;
}
