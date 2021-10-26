#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"
#include "cars.h"
#include "cpmss.h"

/* global mutex for our program. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag = 0;
bool flagPlateFound = false;
bool flagTempGenerated = false;

bool found = false;
shared_memory_t shm;
pthread_mutex_t plateGenerationMutex;
#define CHECK_TIME 2

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

void *generatePlate(char *plate) {
    // Mutex locks to protect the global random
    pthread_mutex_lock(&plateGenerationMutex);

    int allowCarCheck = generateInRange(0,1);

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
    return 0;
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

void *generateTemp() {
    int tempSleepTime = generateInRange(1,5);
    sleep(msSleep(tempSleepTime));
    
    for (int i = 0; i < NUM_LEVELS; i++) {
        int temperatureRand = generateInRange(56,70);
        shm.data->levels[i].temperature_sensor = temperatureRand;
    }
    flagTempGenerated = true;
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

    // Generate the plate and assign
    generatePlate(plate);

    int entranceRand = generateInRange(0, NUM_LEVELS-1);

	// BOOM GATE
	pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
    shm.data->entrances[entranceRand].gate.status = 'C';
	pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

	pthread_mutex_lock(&shm.data->exits[entranceRand].gate.lock);
    shm.data->exits[entranceRand].gate.status = 'C';
    pthread_mutex_unlock(&shm.data->exits[entranceRand].gate.lock);
    
    pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
    strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
    pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);

    // Once a car reaches the front of the queue, it will wait 2ms before triggering the entrance LPR.
    sleep(msSleep(CHECK_TIME));

    int levelRand = entranceRand;

    char levelRandChar = levelRand + 1 + '0';
    pthread_mutex_lock(&shm.data->entrances[entranceRand].sign.lock);
    shm.data->entrances[entranceRand].sign.status = levelRandChar;
    pthread_mutex_unlock(&shm.data->entrances[entranceRand].sign.lock);

    // If true save to shared memory entrance LPR 
    if (found) {
        // || checkplate(plate)
        found = false;

        // Randomly generating the entrance check the car will go to (1-5)
        int entranceDiff = 0;

        // i -> entrance no.
        // level[i] -> capacity
        // do {
            for (int i = 0; i < NUM_LEVELS; i++) {
                if (i == levelRand) {
                    if (level[i] < MAX_LEVEL_CAPACITY) {
                        level[i]++;
                        // printf("LEVEL: %d, CAPACITY: %d\n", i+1, level[i]);
                    }
                    else {
                        // INFO SIGN 'F'
                        shm.data->entrances[entranceRand].sign.status = 'F';
                        do {
                            entranceDiff = generateInRange(0,4);   
                        } 
                        while (entranceDiff == levelRand);       
                        levelRand = entranceDiff;
                        level[levelRand]++;
                        // printf("LEVEL: %d, CAPACITY: %d\n", levelRand+1, level[levelRand]);         
                    }
                }
            }
        // }
        // while (flag == 0);

        // Copies the car plate over to shared memory
        pthread_mutex_lock(&shm.data->levels[levelRand].sensor.lock);
        strcpy(shm.data->levels[levelRand].sensor.plate, plate);
        pthread_mutex_unlock(&shm.data->levels[levelRand].sensor.lock);

        flagPlateFound = true;

        // BOOM GATE 
        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'R';
        sleep(msSleep(RAISE_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'O';
        sleep(msSleep(OPEN_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
        initCars(plate, entranceRand);   
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);

        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);   
        shm.data->entrances[entranceRand].gate.status = 'L';
        sleep(msSleep(LOWER_TIME));
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);

        pthread_mutex_lock(&shm.data->entrances[entranceRand].gate.lock);
        shm.data->entrances[entranceRand].gate.status = 'C';
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].gate.lock);
    }
    else {
        flagPlateFound = false;
        
        // INFO SIGN 'X'
        pthread_mutex_lock(&shm.data->entrances[entranceRand].sign.lock);
        shm.data->entrances[entranceRand].sign.status = 'X';
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].sign.lock);
    }
    
    // Unlock the mutex protection
    pthread_mutex_unlock(&mutex); 

    free(plate);

    return 0;
}
