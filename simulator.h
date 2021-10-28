/*********************************************************************
 * \file   simulator.h
 * \brief  API definition for the simulator
 * 
 * \authors Anna Nguyen, Peter Nguyen, Tyrone Nolasco
 * \date    October 2021
 *********************************************************************/
#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>   
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>      

#define PARKING_SIZE 2920 // Defining the shared memory structure size
#define NUM_LEVELS 5      // Defining the number of levels

// Generate and return a random number in the provided range
int generateInRange(int lower, int upper);

// Function to convert to milliseconds for sleep  
int msSleep (long msec);

#ifndef SIMULATOR_H
#define SIMULATOR_H

// Structs to be used in Shared Memory
typedef struct lpr_sensor { // LPR - 96 bytes
    pthread_mutex_t lock;   // 40 bytes
    pthread_cond_t cond;    // 48 bytes
    char plate[6];          // 6 bytes
    char padding[2];        // 2 bytes
} lpr_sensor_t;

typedef struct boom_gate { // BOOM GATE - 96 bytes
    pthread_mutex_t lock;  // 40 bytes
    pthread_cond_t cond;   // 48 bytes
    char status;           // 1 byte
    char padding[7];       // 9 bytes
} boom_gate_t;

typedef struct information_sign { // INFO SIGN - 96 bytes
    pthread_mutex_t lock;         // 40 bytes
    pthread_cond_t cond;          // 48 bytes
    char status;                  // 1 byte
    char padding[7];              // 7 bytes
} information_sign_t;


typedef struct entrance {    // ENTRANCE - 288 bytes
    lpr_sensor_t sensor;     // 96 bytes
    boom_gate_t gate;        // 96 bytes
    information_sign_t sign; // 96 bytes
} entrance_t;

typedef struct exit {    // EXIT - 192 bytes
    lpr_sensor_t sensor; // 96 bytes
    boom_gate_t gate;    // 96 bytes
} exit_t;

typedef struct level {        // Level - 104 bytes
    lpr_sensor_t sensor;      // 96 bytes
    short temperature_sensor; // 2 bytes
    bool alarm;               // 1 byte
    char padding[5];          // 5 bytes      
} level_t;

typedef struct carpark {     // Carpark - 2920 bytes
    entrance_t entrances[5]; // 1440 bytes
    exit_t exits[5];         // 960 bytes 
    level_t levels[5];       // 520 bytes
} carpark_t;

#endif