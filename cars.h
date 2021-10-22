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

// typedef struct threadpool {

// }


void *generateCarTime();

// car cars[100];


// cars[0].plate -> 

//worker
void *car();

int insert_item(char plate[6]);
// int remove_item(buffer_item *item);
void initCars(char *plate, int entrance, int *levelCounter);

void sleepCarTime();


