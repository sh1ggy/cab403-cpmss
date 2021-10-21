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

int insertPointer = 0, removePointer = 0;


void *sleepCarTime( ) {
	int driveParkingSpaceTime = 10;
	int parkingTime = 0;
	int driveExitTime = 10;
	// TODO: MUTEXES
    int parkingGenerateTime = generateInRange(100, 10000);
    // int plateGenerateMS;    
    // printf("RAND TIME: %d\n", plateGenerateTime); //-- printing rand time
    
	sleep(msSleep(driveParkingSpaceTime));

    parkingTime = msSleep(parkingGenerateTime);
    sleep(parkingTime); 
	
	sleep(msSleep(driveExitTime));

    return 0;
}

void *car(void *param)
{

	sleepCarTime(param);
	int *level = (int *)param;
	printf("PASSED FROM INIT INTO CAR THREAD: %d\n", *level);
	level--;

	printf("DECREMENTED LEVEL: %d\n", *level);
	// sleep(3);
	

	for (int x = 0; x < MESSAGE_REPEAT; x++)
    {
		// printf("Thread id is %s, pthread ID - %lu\n", string, pthread_self());
        // usleep(100000); //microsleep
    }
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
	// printf("\nStarting to produce - Semaphore empty value = %d\n", val1);
    // ----------------------- CHANGE
	// buffer[insertPointer++] = item;
	// insertPointer = insertPointer % BUFFER_SIZE;



    printf("\nCAR PLATE: %s\n", carPlate);
	//Release mutex lock and full semaphore
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	sem_getvalue(&full, &value);
	// printf("\nFinished producing - Semaphore full value = %d\n", value);

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

void initCars(char *plate, int *levelCounter)
{
    //consumerThreads, producerThreads
	int value1 = 0, value2 = 0;
    pthread_t carsThread;

	printf("INIT CAR LEVEL PASS: %d\n", *levelCounter);

	// char *threadPlateID = plate;

	// pthread_t tid[producerThreads];
	// pthread_t cid[consumerThreads];

	//Initialize the the locks
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	
	// printf("%d\n", pthread_mutex_init(&mutex, NULL));
	// printf("%d\n", sem_init(&empty, 0, BUFFER_SIZE));
	// printf("%d\n", sem_init(&full, 0, 0));

	sem_getvalue(&full, &value1);
	// printf("\n\nInitialised value in main of full semaphore = %d\n", value1);
	sem_getvalue(&empty, &value2);
	// printf("\n\nInitialised value in main of empty semaphore = %d\n", value2);

	//----------------------------KILL...@@@@@@@@@@@@@@@@@@@@@@@@@@###################
	/*for (int i = 0; i < producerThreads; i++)
	{
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tid[i], &attr, producer, NULL);
	}*/

	// for (int j = 0; j < consumerThreads; j++)
	// {

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&carsThread, &attr, car, levelCounter);
	


	// }

	return;
}