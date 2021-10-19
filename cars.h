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
// #include "shm.h"

typedef int buffer_item;
#define BUFFER_SIZE 5

// typedef struct threadpool {

// }

typedef struct car {
	char plate;
	int id;
} car_t;

// car cars[100];


// cars[0].plate -> 

//worker
void *car(void *param);

int insert_item(char plate[6]);
// int remove_item(buffer_item *item);
void initCars();


