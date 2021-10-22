/*****************************************************************//**
 * \file   dbl_vector.h
 * \brief  API definition for the dbl_vector abstract data type.
 * 
 * \author Lawrence
 * \date   August 2021
 *********************************************************************/
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

// shared memory libs
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>      


#define PARKING_SIZE 2920
#define NUM_LEVELS 5


int generateInRange(int lower, int upper);
int msSleep (long msec);

/**
* STRUCTS & SHARED MEMORY
*/
#ifndef SIMULATOR_H
#define SIMULATOR_H
// LPR - 96 bytes
typedef struct lpr_sensor {
    pthread_mutex_t lock; //40 bytes
    pthread_cond_t cond; //48 bytes
    char plate[6]; //6 bytes
    char padding[2]; //2 bytes
} lpr_sensor_t;

// BOOM GATE - 96 bytes
typedef struct boom_gate {
    pthread_mutex_t lock; //40 bytes
    pthread_cond_t cond; //48 bytes
    char status[1]; //1 byte
    char padding[7]; //9 bytes
} boom_gate_t;

// INFO SIGN - 96 bytes
typedef struct information_sign {
    pthread_mutex_t lock; //40 bytes
    pthread_cond_t cond; //48 bytes
    char status[1]; //1 byte
    char padding[7]; //7 bytes
} information_sign_t;

// ENTRANCE - 288 bytes
typedef struct entrance {
    lpr_sensor_t sensor; //96 bytes
    boom_gate_t gate; //96 bytes
    information_sign_t sign; //96 bytes
} entrance_t;

// EXIT - 192 bytes
typedef struct exit {
    lpr_sensor_t sensor; //96 bytes
    boom_gate_t gate; //96 bytes
} exit_t;

// Level - 104 bytes
typedef struct level {
    lpr_sensor_t sensor; //96 bytes
    short temperature_sensor; //2 bytes
    bool alarm; //1 byte
    char padding[5]; //5 bytes      
} level_t;

typedef struct carpark {
    entrance_t entrances[5];
    exit_t exits[5];
    level_t levels[5];
} carpark_t;



#endif