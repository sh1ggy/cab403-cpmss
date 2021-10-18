#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
// shared memory libs
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>       
#include <fcntl.h>   
#include <stddef.h>   

#include "simulator.h"

#define SHARE_NAME "PARKING"

/**
 * A shared memory control structure.
 */
typedef struct shared_memory {
    /// The name of the shared memory object.
    const char* name;

    /// The file descriptor used to manage the shared memory object.
    int fd;

    /// Address of the shared data block. 
    carpark_t* data;
} shared_memory_t;


bool get_shared_object( shared_memory_t* shm, const char* share_name );

bool create_shared_object( shared_memory_t* shm, const char* share_name );

void destroy_shared_object( shared_memory_t* shm );