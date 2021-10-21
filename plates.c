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
pthread_mutex_t plateGenerationMutex;

bool checkPlate( char *plate ) {
    if (htab_find(&h, plate) == NULL) {
        return false;
    }
    else {
        printf("\n---------FOUND PLATE---------\n");
        printf("VALID PLATE: %s\n", plate);
        // exit(0); // -------------COMMENT OUT LATER THIS IS FOR TESTING
        sleep(3);
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

    int allowCarCheck = generateInRange(0,3);

    if (allowCarCheck == 0)
    {
        // strcpy(plate, randLine());
        randLine(plate);
        printf("RAND: %s\n", plate);
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

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@BRING BACK@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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
    char *plate = (char *)malloc(6);

    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);
    
    // Wait for the time till new license plate to be generated
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
    } 

    generatePlate(plate);
    // Generate the plate and assign
    // strcpy(plate, generatePlate());
    printf("%s\n", plate);

    // Once a car reaches the front of the queue, it will wait 2ms before triggering the
    // entrance LPR.
    int waitLPR = 2;
    sleep(msSleep(waitLPR));

    // If true save to shared memory entrance LPR 
    if (checkPlate(plate) || found) {
        found = false;

        // Randomly generating the entrance check the car will go to (1-5)
        int entranceRand = generateInRange(0, 4);
        int entranceDiff = 0;
        int* levelCounter; // TODO: LOCK AND MUTEX AND SHARED MEM THIS SHIT

        // i -> entrance no.
        // level[i] -> capacity
        for (int i = 0; i < 5; i++) {
            if (i == entranceRand) {
                if (level[i] < MAX_LEVEL_CAPACITY) {
                    level[i]++;
                    levelCounter = &level[i];
                    printf("LEVEL: %d, CAPACITY: %d\n", i, level[i]);
                }
                else {
                    do {
                        entranceDiff = generateInRange(0,4);       
                    } 
                    while (entranceDiff == entranceRand);       
                    entranceRand = entranceDiff;      
                }
            }
        }

        // switch(entranceRand) 
        // {
        //     case 0:
        //         if(level1 < MAX_LEVEL_CAPACITY) {
        //             level1++;
        //             levelCounter = level1;
        //             printf("Level1: %d\n", level1);
        //         }
        //         else {
        //             do {
        //                 entranceDiff = generateInRange(0,4);       
        //             } 
        //             while (entranceDiff == entranceRand);       
        //             entranceRand = entranceDiff;      
        //         }
        //         break;
        //     case 1:
        //         if(level2 < MAX_LEVEL_CAPACITY) {
        //             level2++; 
        //             levelCounter = level2;
        //             printf("Level2: %d\n", level2);
        //         }
        //         else {
        //             do {
        //                 entranceDiff = generateInRange(0,4);       
        //             } 
        //             while (entranceDiff == entranceRand);       
        //             entranceRand = entranceDiff;      
        //         }
        //         break;
        //     case 2:
        //         if(level3 < MAX_LEVEL_CAPACITY) {
        //             level3++; 
        //             levelCounter = level3;
        //             printf("Level3: %d\n", level3);
        //         }
        //         else {
        //             do {
        //                 entranceDiff = generateInRange(0,4);       
        //             } 
        //             while (entranceDiff == entranceRand);       
        //             entranceRand = entranceDiff;      
        //         }
        //         break;
        //     case 3:
        //         if(level4 < MAX_LEVEL_CAPACITY) {
        //             level4++; 
        //             levelCounter = level4;
        //             printf("Level4: %d\n", level4);
        //         }
        //         else {
        //             do {
        //                 entranceDiff = generateInRange(0,4);       
        //             } 
        //             while (entranceDiff == entranceRand);       
        //             entranceRand = entranceDiff;      
        //         }
        //         break;
        //     case 4:
        //         if(level5 < MAX_LEVEL_CAPACITY) {
        //             level5++;  
        //             levelCounter = level5;
        //             printf("Level5: %d\n", level5);
        //         }
        //         else {
        //             do {
        //                 entranceDiff = generateInRange(0,4);       
        //             } 
        //             while (entranceDiff == entranceRand);       
        //             entranceRand = entranceDiff;      
        //         }
        //         break;
        //     default: 
        //         printf("ERROR\n");
        //         break;
        // }

        shared_memory_t shm;
        // printf("FILE DESCRIPTOR: %d\n",shm.fd);
        get_shared_object(&shm, SHARE_NAME);
        printf("ENTRANCE: %d\n", entranceRand);

        // Copies the car plate over to shared memory
        strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
        
        flagPlateFound = true;

        // Initialise a thread for a car
        initCars(plate, levelCounter);   
        // insert_item(plate); 
        
        sleep(1);
    }
    else {
        flagPlateFound = false;
    }
    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 
    free(plate);
    return 0;
}
