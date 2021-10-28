/*********************************************************************
 * \file   cpmss.h
 * \brief  API definition for the whole system.
 * 
 * \authors Anna Nguyen, Peter Nguyen, Tyrone Nolasco
 * \date    October 2021
 *********************************************************************/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h> 

// The main thread function for the manager.c file
void *managerMain ( );

// The main thread function for the simulator.c file
void *simulatorMain ( );

// The main thread function for the firealarm.c fie
void *fireAlarmMain ( );
