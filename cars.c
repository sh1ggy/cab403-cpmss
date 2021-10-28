#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"
#include "cars.h"

pthread_mutex_t fileLock;

// Struct for car
typedef struct cars {
	char plate[6];
	int entrance;
} cars_t;

// Calculate the bill based on time spent, adding to the total revenue of manager.c
void calcBill(int totalCarTime, char *plate) {
	double totalBill = totalCarTime * BILLING_RATE;
	carBill += totalBill;
	createBillingFile(plate, totalBill);
}

// Create / write to the billing.txt file with the plate and the total bill
void createBillingFile(char *plate, double totalBill) {
	pthread_mutex_lock(&fileLock);
	FILE *fp = fopen("billing.txt", "a");
	fprintf(fp, "%s $%.2f\n", plate, totalBill);
	fclose(fp);
	pthread_mutex_unlock(&fileLock);
}

// Calculate the total time a car is in the car park for after entering
int sleepCarTime( ) {
	int parkingTime = 0;
    int parkingGenerateTime = generateInRange(100, 10000);
	int totalCarTime = DRIVE_TO_PARKING_TIME + parkingGenerateTime + DRIVE_TO_EXIT_TIME;
	
	sleep(msSleep(DRIVE_TO_PARKING_TIME));
    parkingTime = msSleep(parkingGenerateTime);
    sleep(parkingTime); 
	sleep(msSleep(DRIVE_TO_EXIT_TIME));

    return totalCarTime;
}

// Work that the car does 
void *car(void *params)
{
	cars_t *carThreadParams = params;
	
	char *plate = carThreadParams->plate;
	// Calculate the bill and sleep the thread for the designated time
	calcBill(sleepCarTime(), plate);	
	int exit = carThreadParams->entrance;

	// Boom Gate Status: when the car exits the car park
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

	// Decrement the capacity for the level after the car leaves
	level[exit]--;

	free(carThreadParams);
	return 0;
}

// Initialise the variables to be sent into the car thread
void initCars(char *plate, int entrance) 
{
	pthread_t carsThread;

	cars_t *carThreadParams = malloc(sizeof(cars_t));
	strcpy(carThreadParams->plate, plate);
	carThreadParams->entrance = entrance;

	// Initialize the the locks
	pthread_mutex_init(&fileLock, NULL);

	// Creating a thread for each individual car which enters the car park
	// and passing through its plate and level information
	pthread_create(&carsThread, NULL, car, (void *)carThreadParams);

	return;
}