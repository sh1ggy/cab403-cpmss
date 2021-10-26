#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h> 
#include <semaphore.h>
#include <unistd.h>   

#include "simulator.h"
#include "plates.h"
#include "lpr.h"
#include "shm.h"

typedef int buffer_item;
#define BUFFER_SIZE 5
#define DRIVE_TO_PARKING_TIME 10
#define DRIVE_TO_EXIT_TIME 10
#define BILLING_RATE 0.05

double carBill;

void calcBill(int totalCarTime, char *plate);

void *generateCarTime();

void *car();

void initCars(char *plate, int entrance); 

int sleepCarTime();

void createBillingFile(char *plate, double totalBill);
