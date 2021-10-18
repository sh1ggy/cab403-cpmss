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

//worker
void *car(void *param);

int insert_item(char plate[6]);
// int remove_item(buffer_item *item);
void initCars();


