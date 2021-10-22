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
bool found = false;
shared_memory_t shm;
pthread_mutex_t plateGenerationMutex;

bool checkPlate( char *plate ) {
    if (htab_find(&h, plate) == NULL) {
        return false;
    }
    else {
        // printf("\n---------FOUND PLATE---------\n");
        // printf("VALID PLATE: %s\n", plate);
        // exit(0); // -------------COMMENT OUT LATER THIS IS FOR TESTING
        // sleep(3);
        return true;
    }
    return false;
}

/**
* GENERATING PLATES TO BE CHECKED
*/

bool *generatePlate(char *plate) {
    // Mutex locks to protect the global random
    pthread_mutex_lock(&plateGenerationMutex);

    int allowCarCheck = generateInRange(0,2);

    if (allowCarCheck == 0)
    {
        // strcpy(plate, randLine());
        randLine(plate);
        // printf("RAND: %s\n", plate);
        found = true;
    }
    else
    {
        // Uses current time as the random seed
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
    }
    
    pthread_mutex_unlock(&plateGenerationMutex);
    return true;
}

void *generatePlateTime() {
    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);
    
    // Signal that work is being done
    pthread_cond_signal(&cond);

    // -------------- REMOVE SLEEP TO TEST PLATE GENERATION
    // Generate time  
    int plateGenerateTime = generateInRange(1, 100);
    int plateGenerateMS;    
    // printf("RAND TIME: %d\n", plateGenerateTime); //-- printing rand time
    
    // Calculate sleep for ms
    plateGenerateMS = msSleep(plateGenerateTime);
    sleep(plateGenerateMS); 

    // Flip while condition for generateCars()
    flag = 1;
    
    // Unlock mutex protection
    pthread_mutex_unlock(&mutex);

    return 0;
}

void *generateCars() {
    char *plate = (char *)calloc(6, sizeof(char));

    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);

    // Wait for the time till new license plate to be generated
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
    } 

    generatePlate(plate);
    // Generate the plate and assign
    // strcpy(plate, generatePlate());
    // printf("%s\n", plate);

    // shared_memory_t shm;
    // get_shared_object(&shm, SHARE_NAME);

    int entranceRand = generateInRange(0, 4);
    
    pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);

    strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
    
    pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);

    // Once a car reaches the front of the queue, it will wait 2ms before triggering the
    // entrance LPR.
    int waitLPR = 2;
    sleep(msSleep(waitLPR));

    int levelRand = entranceRand;
    // If true save to shared memory entrance LPR 
    if (found) {
        // || checkplate(plate)
        found = false;

        // Randomly generating the entrance check the car will go to (1-5)
        int entranceDiff = 0;
        int* levelCounter = 0; // TODO: LOCK AND MUTEX AND SHARED MEM THIS SHIT

        // i -> entrance no.
        // level[i] -> capacity
        for (int i = 0; i < 5; i++) {
            if (i == levelRand) {
                if (level[i] < MAX_LEVEL_CAPACITY) {
                    level[i]++;
                    levelCounter = &level[i];
                    printf("LEVEL: %d, CAPACITY: %d\n", i, level[i]);
                }
                else {
                    // INFO SIGN 'F'
                    do {
                        entranceDiff = generateInRange(0,4);       
                    } 
                    while (entranceDiff == levelRand);       
                    levelRand = entranceDiff;      
                }
            }
        }


        // shared_memory_t shm;
        // printf("FILE DESCRIPTOR: %d\n",shm.fd);
        // get_shared_object(&shm, SHARE_NAME);
        // printf("LEVEL: %d\n", levelRand);

        // Copies the car plate over to shared memory
        // strcpy(shm.data->entrances[levelRand].sensor.plate, plate);
        strcpy(shm.data->levels[levelRand].sensor.plate, plate);

        flagPlateFound = true;

        // Initialise a thread for a car
        //@@@@@@@@@@@@@@@@@@@@ BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);

        initCars(plate, entranceRand, levelCounter);   
    
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);
        
        // sleep(1);
    }
    else {
        flagPlateFound = false;
        // INFO SIGN 'X'

    }
    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 
    free(plate);
    return 0;
}
