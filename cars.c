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

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
#define MESSAGE_REPEAT 20
typedef struct cars {
	char plate[6];
	int entrance;
	// int *levelCounter;
} cars_t;

int insertPointer = 0, removePointer = 0;

void sleepCarTime( ) {
	int driveParkingSpaceTime = 10;
	int parkingTime = 0;
	int driveExitTime = 10;
    int parkingGenerateTime = generateInRange(100, 10000);
	sleep(msSleep(driveParkingSpaceTime));

    parkingTime = msSleep(parkingGenerateTime);
    sleep(parkingTime); 
	
	sleep(msSleep(driveExitTime));

    return;
}

void *car(void *params)
{
	sleepCarTime();
	cars_t *carThreadParams = params;

	char *plate = carThreadParams->plate;
	int exit = carThreadParams->entrance;
	// int levelCounter = *carThreadParams->levelCounter;
	
	// printf("AFTER ----------- CAR THREAD PLATE: %s, CAR THREAD EXIT: %d, CAR THREAD LEVEL COUNTER: %d\n", plate, exit, levelCounter);
	pthread_mutex_lock(&shm.data->exits[exit].sensor.lock);
	strcpy(shm.data->exits[exit].sensor.plate, plate);
	
	shm.data->exits[exit].gate.status = 'R';
	sleep(msSleep(10));

	shm.data->exits[exit].gate.status = 'O';
	sleep(msSleep(20));

	shm.data->exits[exit].gate.status = 'L';
	sleep(msSleep(10));

	shm.data->exits[exit].gate.status = 'C';

	level[exit]--;

	pthread_mutex_unlock(&shm.data->exits[exit].sensor.lock);

	free(carThreadParams);
	return 0;
}

int insert_item(char plate[6])
{
	int value = 0, val1 = 0;
    char carPlate[6];
    strncpy(carPlate, plate, 6);
	//Acquire Empty Semaphore
	sem_wait(&empty);

	//Acquire mutex lock to protect buffer
	pthread_mutex_lock(&mutex);
	sem_getvalue(&empty, &val1);

    // printf("\nCAR PLATE: %s\n", carPlate);
	//Release mutex lock and full semaphore
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	sem_getvalue(&full, &value);

    sleep(5);

	return 0;
}

// int remove_item(buffer_item *item)
// {
// 	int fullSem = 0, emptySem = 0;
// 	//Acquire Full Semaphore
// 	sem_wait(&full);

// 	//Acquire mutex lock to protect buffer
// 	pthread_mutex_lock(&mutex);
// 	sem_getvalue(&full, &fullSem);
// 	printf("Currently consuming - Semaphore full value = %d", value);
//     // ----------------------- CHANGE
// 	*item = buffer[removePointer];
// 	buffer[removePointer++] = -1;
// 	removePointer = removePointer % BUFFER_SIZE;

// 	//Release mutex lock and empty semaphore
// 	pthread_mutex_unlock(&mutex);
// 	sem_post(&empty);
// 	sem_getvalue(&empty, &emptySem);
// 	printf("\nFinished consuming - Semaphore empty value = %d\n", val1);

// 	return 0;
// }

void initCars(char *plate, int entrance) //, int *levelCounter
{

    //consumerThreads, producerThreads
	// int value1 = 0, value2 = 0;
    pthread_t carsThread;

	// cars_t *carThreadParams;
	cars_t *carThreadParams = malloc(sizeof(cars_t));
	// carThreadParams->plate = plate;
	strcpy(carThreadParams->plate, plate);
	carThreadParams->entrance = entrance;
	// carThreadParams->levelCounter = levelCounter;
	
	// printf("CAR INIT PLATE: %s, CAR INIT ENTRANCE: %d, CAR INIT COUNTER: %d\n", carThreadParams->plate, carThreadParams->entrance, *carThreadParams->levelCounter);

	// char *threadPlateID = plate; 

	//Initialize the the locks
	pthread_mutex_init(&mutex, NULL);
	// sem_init(&empty, 0, BUFFER_SIZE);
	// sem_init(&full, 0, 0);
	

	// sem_getvalue(&full, &value1);
	// sem_getvalue(&empty, &value2);

	// pthread_attr_t attr;
	// pthread_attr_init(&attr);

	// printf("INIT CAR LEVEL: %d\n", *levelCounter);
	// printf("INIT CAR LEVEL ENTRANCE: %d\n", entrance);
	// printf("INIT CAR LEVEL CAPACITY FROM ARRY: %d\n", level[entrance]--);



	pthread_create(&carsThread, NULL, car, (void *)carThreadParams);

	// *levelCounter--;

	// printf("DECREMENTED LEVEL: %d\n", *levelCounter);
	
	
	// level[entrance]--;

	// }
	return;
}