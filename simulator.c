#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "cpmss.h"
#include "simulator.h"

pthread_mutex_t plateGenerationMutex;

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

