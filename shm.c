
#include <unistd.h>

// shared memory libs
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>       
#include <fcntl.h>   
#include <stddef.h>  

#include "shm.h"

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    printf("CREATING SHARED MEMORY\n");
    // Remove any previous instance of the shared memory object, if it exists.
    shm_unlink(share_name);
    
    shm -> name = share_name;

    // shm init, saved to shm_fd
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 
    // Error check
    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    // ftruncate sets capacity of shm object + error check
    if (ftruncate(shm->fd, PARKING_SIZE) != 0){
        shm->data = NULL;
        return false;
    }

    // TO DO: SHARED MEMORY RETRIEVAL FUNCTION

    // NULL address ensures this is inserted appropriately into shared memory - the address is chosen by the OS
    carpark_t *carpark = mmap(NULL, sizeof(carpark_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = carpark;
    if (shm->data == MAP_FAILED)
    {
        return false;
    }

    // ------- TESTING SHARED MEMORY CODE
    // printf("CARPARK:\n");

    // printf("LEVEL 1 -> ENTRANCE -> LPR -> PLATE:\n");
    // char* plate = "123XXX";
    // strcpy(carpark->entrances[0].sensor.plate, plate);
    // printf("READING LPR PLATE: %s\n", carpark->entrances[0].sensor.plate);

    // printf("LEVEL 1 -> ENTRANCE -> GATE -> STATUS:\n");
    // char* status = "O";
    // strcpy(carpark->entrances[0].gate.status, status);
    // printf("READING BOOM GATE: %s\n", carpark->entrances[0].gate.status);
    
    // printf("READING LPR PLATE, AGAIN: %s\n", carpark->entrances[0].sensor.plate);

    // printf("entrances offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, entrances));

    // printf("exits offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, exits));

    // printf("levels offset = %ld byte in address structure.\n",
    // offsetof(carpark_t, levels));

    // int carparkInt = sizeof(carpark_t);
    // printf("ENTIRE CARPARK: %d\n", carparkInt);

    // printf("DONE CREATING SHARED MEMORY\n");
    return true;
}

bool get_shared_object( shared_memory_t* shm, const char* share_name ) {
    // Get a file descriptor connected to shared memory object and save in 
    // shm->fd. If the operation fails, ensure that shm->data is 
    // NULL and return false.
    int shm_fd = shm_open(share_name, O_RDWR, 0);
    shm -> fd = shm_fd;
    if (shm_fd < 0) 
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Otherwise, attempt to map the shared memory via mmap, and save the address
    // in shm->data. If mapping fails, return false.
    carpark_t *map = mmap(0, PARKING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = map;

    if (map == MAP_FAILED)
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    // Modify the remaining stub only if necessary.
    return true;
}

void destroy_shared_object( shared_memory_t* shm ) {
    // Remove the shared memory object.
    munmap(&shm, 48);
    shm_unlink(shm -> name);
    shm -> fd = -1;
    shm -> data = NULL; 
}