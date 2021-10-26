#include "shm.h"

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    // printf("CREATING SHARED MEMORY\n");
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
    carpark_t *carpark = mmap(NULL, PARKING_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = carpark;
    if (shm->data == MAP_FAILED)
    {
        return false;
    }

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