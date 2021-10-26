#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"
#include "cars.h"

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t fileLock;
sem_t empty;
sem_t full;

typedef struct cars {
	char plate[6];
	int entrance;
} cars_t;

int insertPointer = 0, removePointer = 0;

void calcBill(int totalCarTime, char *plate) {
	double totalBill = totalCarTime * BILLING_RATE;
	carBill += totalBill;
	createBillingFile(plate, totalBill);
}

void createBillingFile(char *plate, double totalBill) {
	pthread_mutex_lock(&fileLock);
	FILE *fp = fopen("billing.txt", "a");
	fprintf(fp, "%s $%.2f\n", plate, totalBill);
	fclose(fp);
	pthread_mutex_unlock(&fileLock);
}

// DO WORK
int sleepCarTime( ) {
	int driveParkingSpaceTime = DRIVE_TO_PARKING_TIME;
	int parkingTime = 0;
	int driveExitTime = DRIVE_TO_EXIT_TIME;
    int parkingGenerateTime = generateInRange(100, 10000);
	int totalCarTime = driveParkingSpaceTime + parkingGenerateTime + driveExitTime;
	
	sleep(msSleep(driveParkingSpaceTime));

    parkingTime = msSleep(parkingGenerateTime);
    sleep(parkingTime); 
	
	sleep(msSleep(driveExitTime));

    return totalCarTime;
}

void *car(void *params)
{
	cars_t *carThreadParams = params;
	
	char *plate = carThreadParams->plate;
	calcBill(sleepCarTime(), plate);	
	int exit = carThreadParams->entrance;

	// BOOM GATE
	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	strcpy(shm.data->exits[exit].sensor.plate, plate);	
	shm.data->exits[exit].gate.status = 'R';
	sleep(msSleep(RAISE_TIME));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);

	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = 'O';
	sleep(msSleep(OPEN_TIME));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);

	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = 'L';
	sleep(msSleep(LOWER_TIME));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);

	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = 'C';
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);

	level[exit]--;


	free(carThreadParams);
	return 0;
}

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
	pthread_mutex_init(&fileLock, NULL);
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