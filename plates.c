#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"
#include "cars.h"
#include "cpmss.h"

// Global mutex for our plates.c 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plateGenerationMutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

shared_memory_t shm;

// Intialising flags to be used
int flag = 0;
bool found = false;
bool flagPlateFound = false;
bool flagTempGenerated = false;
int full = 0;

// Check the plate against the hash table made from plates.txt
bool checkPlate( char *plate ) {
    if (htab_find(&h, plate) == NULL) {
        return false;
    }
    else {
        return true;
    }
    return false;
}

// Thread function to generate the plates to be checked
void *generatePlate(char *plate) {
    // Mutex locks to protect the global random
    pthread_mutex_lock(&plateGenerationMutex);

    int allowCarCheck = generateInRange(0,1);

    if (allowCarCheck == 0)
    {
        randLine(plate);
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
    
    // Unlock mutex
    pthread_mutex_unlock(&plateGenerationMutex);
    
    return 0;
}

// Thread function to generate the random time between cars being generated
void *generatePlateTime() {
    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);
    
    // Signal that work is being done
    pthread_cond_signal(&cond);

    // Generate time  
    int plateGenerateTime = generateInRange(1, 100);
    int plateGenerateMS;    
    
    // Calculate sleep for ms
    plateGenerateMS = msSleep(plateGenerateTime);
    sleep(plateGenerateMS); 

    // Flip while condition for generateCars()
    flag = 1;
    
    // Unlock mutex protection
    pthread_mutex_unlock(&mutex);

    return 0;
}

// Generating a random temperature between a range on each floor for simulator
void *generateTemp() {
    int tempSleepTime = generateInRange(1,5);
    sleep(msSleep(tempSleepTime));
    
    for (int i = 0; i < NUM_LEVELS; i++) {
        int temperatureRand = generateInRange(LOWER_TEMP,HIGHER_TEMP);
        shm.data->levels[i].temperature_sensor = temperatureRand;
    }
    flagTempGenerated = true;
    return 0;
}

// Function to check cars and generate a car thread 
void *generateCars() {
    char *plate = (char *)calloc(6, sizeof(char));

    // Lock mutex to protect variables
    pthread_mutex_lock(&mutex);

    // Wait for the time till new license plate to be generated
    while (flag == 0) {
        pthread_cond_wait(&cond, &mutex);
    } 

    // Generate the plate and assign
    generatePlate(plate);

    // Randomly generating the entrance check the car will go to (1-5)
    int entranceRand = generateInRange(0, NUM_LEVELS-1);

	// Boom Gate status being set as 'C' closed at the start
	pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
    shm.data->entrances[entranceRand].gate.status = 'C';
	pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

	pthread_mutex_lock(&shm.data->exits[entranceRand].gate.lock);
    shm.data->exits[entranceRand].gate.status = 'C';
    pthread_mutex_unlock(&shm.data->exits[entranceRand].gate.lock);
    
    // Obtain plate from shared memory
    pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
    strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
    pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);

    // Once a car reaches the front of the queue, it will wait 2ms before triggering the entrance LPR.
    sleep(msSleep(CHECK_TIME));

    char infoStatusChar = entranceRand + 1 + '0';

    // Update the information sign's status
    pthread_mutex_lock(&shm.data->entrances[entranceRand].sign.lock);
    shm.data->entrances[entranceRand].sign.status = infoStatusChar;
    pthread_mutex_unlock(&shm.data->entrances[entranceRand].sign.lock);


    if ((level[0] == MAX_LEVEL_CAPACITY) && (level[1] == MAX_LEVEL_CAPACITY) && (level[2] == MAX_LEVEL_CAPACITY) && (level[3] == MAX_LEVEL_CAPACITY) && (level[4] == MAX_LEVEL_CAPACITY)) {
        full = 5;
        flagPlateFound = true;
    }


    // If true save to shared memory entrance LPR 
    if (found && (full != 5)) {
        // Flipping the found bool once a car enters
        found = false;

        // Initalising variable to be used 
        int entranceDiff = 0;

        // Unusual behaviour of car to move to a different level than originally assigned: 5% chance
        int unusualBehaviour = generateInRange(0,20);

        if (unusualBehaviour == 0)
        {
            entranceRand = generateInRange(0, NUM_LEVELS-1);
        }

        // Checking if the assign level is full 
        for (int i = 0; i < NUM_LEVELS; i++) {
            if (i == entranceRand) {
                if (level[i] < MAX_LEVEL_CAPACITY) {
                    level[i]++; // Incrementing the level capacity
                }
                // Reassigning a level which is not full
                else {
                    shm.data->entrances[entranceRand].sign.status = 'F';
                    do {
                        entranceDiff = generateInRange(0,4);
                        if (level[i] < MAX_LEVEL_CAPACITY) {
                            level[entranceDiff]++;   
                            entranceRand = entranceDiff;
                        }    
                    } 
                    while (entranceDiff == entranceRand);       
                }
            }
        }

        // Copies the car plate over to shared memory
        pthread_mutex_lock(&shm.data->levels[entranceRand].sensor.lock);
        strcpy(shm.data->levels[entranceRand].sensor.plate, plate);
        pthread_mutex_unlock(&shm.data->levels[entranceRand].sensor.lock);

        // Signal to manager.c that a plate has been found and that it is safe to print
        flagPlateFound = true;

	    // Boom Gate status being set and slept for raising
        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'R';
        sleep(msSleep(RAISE_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        // Boom Gate status being set and slept for opening
        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'O';
        sleep(msSleep(OPEN_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        // Initialise the car thread, sending the appropriate plate and its entrance
        pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
        initCars(plate, entranceRand);   
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);

        // Boom Gate status being set and slept for lowering after the car enters
        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);   
        shm.data->entrances[entranceRand].gate.status = 'L';
        sleep(msSleep(LOWER_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        // Boom Gate status being set to closed
        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'C';
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);
    }
    else {
        // Signal to manager.c that a plate has not been found and that it is not safe to print
        flagPlateFound = false;

        if (full == 5) {
            shm.data->entrances[entranceRand].sign.status = 'F';
        }
        else {
            // Information Sign status being set to 'X' for unapproved cars
            pthread_mutex_lock(&shm.data->entrances[entranceRand].sign.lock);
            shm.data->entrances[entranceRand].sign.status = 'X';
            pthread_mutex_unlock(&shm.data->entrances[entranceRand].sign.lock);
        }
    }
    
    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 

    // Freeing plate memory allocation
    free(plate);

    return 0;
}
