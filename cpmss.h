/*****************************************************************//**
 * \file   dbl_vector.h
 * \brief  API definition for the dbl_vector abstract data type.
 * 
 * \author Lawrence
 * \date   August 2021
 *********************************************************************/
#pragma once

#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>    


void managerTest( );

void simulatorTest ( );

int generateInRange(int lower, int upper);

int msSleep(long msec);

/**
 * Reads everything in a provided text file
 *
 * PRE: Need a .txt file in the same directory
 *
 * POST: reads
 * 
 * \param filename The address of a dbl_vector_t to initialise.
 */
void readPlates ( const char * filename, const char * mode );

#define TEST 5