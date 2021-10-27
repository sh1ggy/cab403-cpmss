/*********************************************************************
 * \file   cars.h
 * \brief  API definition for creating cars and work they do.
 * 
 * \authors Peter Nguyen, Tyrone Nolasco
 * \date    October 2021
 *********************************************************************/

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

#define DRIVE_TO_PARKING_TIME 10 // Time taken to drive from the entrance to the parking spot
#define DRIVE_TO_EXIT_TIME 10    // Time taken to drive to the exit
#define BILLING_RATE 0.05        // Setting the parking price rate of $0.05 per ms

double carBill;

// Calculate the bill based on time spent, adding to the total revenue of manager.c
void calcBill(int totalCarTime, char *plate);

// Create / write to the billing.txt file with the plate and the total bill
void createBillingFile(char *plate, double totalBill);

// Calculate the total time a car is in the car park for after entering
int sleepCarTime();

// Work that the car does 
void *car();

// Initialise the variables to be sent into the car thread
void initCars(char *plate, int entrance); 



