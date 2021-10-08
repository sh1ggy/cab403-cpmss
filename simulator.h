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

void simulatorMain();

char *generatePlate();

/**
 * Reads everything in a provided text file
 *
 * PRE: Need a .txt file in the same directory
 * POST: reads
 * \param filename The address of a dbl_vector_t to initialise.
 */
void readPlates ( const char * filename, const char * mode );

// strcmp() - string compare
// bool checkPlate(char[] plate);

/**
 * A shared data block.
 */
typedef struct shared_data {
    /// Used by controller to notify worker when a task is available.
    // sem_t controller_semaphore;

    /// Used by worker to notify controller when a result is available.
    // sem_t worker_semaphore;

    
} shared_data_t;



/**
 * A shared memory control structure.
 */
typedef struct shared_memory {
    /// The name of the shared memory object.
    const char* name;

    /// The file descriptor used to manage the shared memory object.
    int fd;

    /// Address of the shared data block. 
    shared_data_t* data;
} shared_memory_t;

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


/**
 * Controller: initialise a shared_object_t, creating a block of shared memory
 * with the designated name, and setting its storage capacity to the size of a
 * shared data block.
 *
 * PRE: n/a
 *
 * POST: shm_unlink has been invoked to delete any previous instance of the
 *       shared memory object, if it exists.
 * AND   The share name has been saved in shm->name.
 * AND   shm_open has been invoked to obtain a file descriptor connected to a
 *       newly created shared memory object with size equal to the size of a
 *       shared_data_t struct, with support for read and write operations. The
 *       file descriptor should be saved in shm->fd, regardless of the final outcome.
 * AND   ftruncate has been invoked to set the size of the shared memory object
 *       equal to the size of a shared_data_t struct.
 * AND   mmap has been invoked to obtain a pointer to the shared memory, and the
 *       result has been stored in shm->data, regardless of the final outcome.
 * AND   (this code is provided for you, don't interfere with it) The shared
 *       semaphore has been initialised to manage access to the shared buffer.
 * AND   Semaphores have been initialised in a waiting state.
 *
 * \param shm The address of a shared memory control structure which will be
 *            populated by this function.
 * \param share_name The unique string used to identify the shared memory object.
 * \returns Returns true if and only if shm->fd >= 0 and shm->data != MAP_FAILED.
 *          Even if false is returned, shm->fd should contain the value returned
 *          by shm_open, and shm->data should contain the value returned by mmap.
 */
bool create_shared_object( shared_memory_t* shm, const char* share_name );

/**
 * Worker: Get a file descriptor which may be used to interact with a shared memory
 * object, and map the shared object to get its address.
 *
 * PRE: The Controller has previously invoked create_shared_fd to instantiate the
 *      shared memory object.
 *
 * POST: shm_open has been invoked to obtain a file descriptor connected to a
 *       shared_data_t struct, with support for read and write operations. The
 *       file descriptor should be saved in shm->fd, regardless of the final outcome.
 * AND   mmap has been invoked to obtain a pointer to the shared memory, and the
 *       result has been stored in shm->data, regardless of the final outcome.
 *
 * \param share_name The unique identification string of the shared memory object.
 * \return Returns true if and only if shm->fd >= 0 and
 *         shm->data != NULL.
 */
bool get_shared_object( shared_memory_t* shm, const char* share_name );


#define TEST 5