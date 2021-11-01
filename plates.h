/*********************************************************************
 * \file   plates.h
 * \brief  API definition for the plate generation, checking and car
 *         thread generation
 * 
 * \authors Anna Nguyen, Peter Nguyen, Tyrone Nolasco
 * \date    October 2021
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>   
#include <unistd.h>  

#include "simulator.h"
#include "lpr.h"

extern bool flagPlateFound;   // Flag for manager to use for the While Loop
#define MAX_LEVEL_CAPACITY 20 // Max car capacity for a level
#define CHECK_TIME 2          // Time before triggering the entrance LPR: 2ms
#define RAISE_TIME 10         // Time for the boom gate to be raised: 10ms
#define OPEN_TIME 20          // Time duration for the boom gate to be opened: 20ms
#define LOWER_TIME 10         // Time for the boom gate to be lowered: 10ms
#define LOWER_TEMP 25         // Lower range for the temperature to be generated in 
#define HIGHER_TEMP 32        // Upper range for the temperature to be generated in 
int level[NUM_LEVELS];        // Setting the total amount of levels to be 5
int full;                     // Number to check if all levels are full

// Check the plate against the hash table made from plates.txt
bool checkPlate( char plate[6] );

// Thread function to generate the plates to be checked
void *generatePlate();

// Thread function to generate the random time between cars being generated
void *generatePlateTime();

// Generating a random temperature between a range on each floor for simulator
void *generateTemp();

// Function to check cars and generate a car thread 
void *generateCars();