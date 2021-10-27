/*********************************************************************
 * \file   shm.h
 * \brief  API definition for the creation and allocation of 
 *         shared memory 
 * 
 * \authors Anna Nguyen, Peter Nguyen, Tyrone Nolasco
 * \date    October 2021
 *********************************************************************/
 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>   
#include <stddef.h>   
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>       

#include "simulator.h"

// Shared memory name used
#define SHARE_NAME "PARKING"

// Guarding the header file
#ifndef SHM_H
#define SHM_H

// Struct to define storage of shared memory variables
typedef struct shared_memory {
    /// The name of the shared memory object.
    const char* name;

    /// The file descriptor used to manage the shared memory object.
    int fd;

    /// Address of the shared data block. 
    carpark_t* data;
} shared_memory_t;

// Making the shared memory created to be global variable
extern shared_memory_t shm;

// Create the shared memory object and allocate memory
bool create_shared_object( shared_memory_t* shm, const char* share_name );

// Get shared object based on the share_name
bool get_shared_object( shared_memory_t* shm, const char* share_name );

// Remove the shared memory object and free memory
void destroy_shared_object( shared_memory_t* shm );

#endif